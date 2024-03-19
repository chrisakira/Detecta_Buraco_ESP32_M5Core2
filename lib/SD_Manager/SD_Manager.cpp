#include <SD_Manager.h>

void SD_Manager::receive_data_buffer(void *z)
{
    Data tmp;
    char filename[50];
    char filename_ready[50];
    uint8_t buffer[(sizeof(uint_fast64_t) + sizeof(float) + sizeof(int)) * MAX_BUFFER_SIZE * 2] = {0};
    uint8_t buffer_timestamp[sizeof(uint_fast64_t)+1] = {0};
    uint8_t buffer_value[sizeof(float)+1] = {0};
    uint8_t buffer_measurement[sizeof(int)+1] = {0};
    uint_fast32_t j = 0;
   
    for (;;)
    {
        if (this->xqueue_manager_ptr->receive_data(&tmp))
        {
            if (j >= (sizeof(uint_fast64_t) + sizeof(float) + sizeof(int)) * MAX_BUFFER_SIZE)
            {   
                xSemaphoreTake(*this->xMutex, portMAX_DELAY);     
                sprintf(filename, "%s%s", m5_manager_ptr->get_current_time(), "not-ready.aki");
                sprintf(filename_ready, "%s%s", m5_manager_ptr->get_current_time(), ".aki");
                if (myFile.open(filename, O_WRITE | O_CREAT))
                    logger_manager_ptr->debug("[SD_Manager.cpp] File opened");
                else
                {
                    logger_manager_ptr->critical("[SD_Manager.cpp] File not opened");
                    exit(-1);
                }
                myFile.write(buffer, j);
                myFile.print("#EOF");
                myFile.flush();
                myFile.rename(filename_ready);
                myFile.close();
                j = 0;
                xSemaphoreGive(*this->xMutex);
            }
             
            memcpy(buffer_timestamp, (uint8_t *)&tmp.time_stamp, sizeof(tmp.time_stamp));
            memcpy(buffer_value, (uint8_t *)&tmp.value, sizeof(tmp.value));
            memcpy(buffer_measurement, (uint8_t *)&tmp.measurement, sizeof(tmp.measurement));
            for (uint_fast8_t i = 0; i < sizeof(tmp.time_stamp); i++)
            {
                buffer[j] = buffer_timestamp[i];
                j++;
            }
            for (uint_fast8_t i = 0; i < sizeof(tmp.value); i++)
            {
                buffer[j] = buffer_value[i];
                j++;
            }
            for (uint_fast8_t i = 0; i < sizeof(tmp.measurement); i++)
            {
                buffer[j] = buffer_measurement[i];
                j++;
            }
        }
        else
        {
            vTaskDelay(1);
        }
            
    }
}

bool SD_Manager::create_tasks()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<SD_Manager *>(z)->receive_data_buffer(z); },
                                    "Receive Data Buffer", 20000, this, 4, &receive_data_buffer_task_handle, 0);
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Collector_Manager] Collector Task not created \n[Collector_Manager] " + String(e.what()));
        return false;
    }
    return false;
}

String SD_Manager::get_oldest_file(const char *dirname)
{
    SdFile_ dir;
    SdFile_ file;

    if (!dir.open(dirname))
    {
        Serial.println("Failed to open directory");
        return "";
    }

    dir.rewind();
    SdFile_ oldestFile;
    uint32_t oldestTime = UINT32_MAX;

    while (file.openNext(&dir, O_READ))
    {
        dir_t dirEntry;

        file.dirEntry(&dirEntry);

        uint32_t fileTime = (uint32_t)dirEntry.creationDate << 16 | dirEntry.creationTime;

        if (fileTime < oldestTime)
        {
            char filename[50];
            file.getName(filename, sizeof(filename));
            String filenameStr(filename);
            if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
            {
                oldestTime = fileTime;
                oldestFile = file;
            }
        }

        file.close();
    }

    if (oldestFile.isOpen())
    {
        char filename[50];
        oldestFile.getName(filename, sizeof(filename));
        String filenameStr(filename);
        oldestFile.close();
        if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
        {
            return String("/") + String(filename);
        }
        else{            
            return "";
        }
    }
    else
    { 
        return "";
    }
}

void SD_Manager::erase_all_files()
{
    
    SdFile_ dir;
    if (!dir.open("/"))
    {
        Serial.println("Failed to open directory"); 
        return;
    }

    dir.rewind();
    SdFile_ file;

    while (file.openNext(&dir, O_READ))
    {   
        dir_t dirEntry;
        file.dirEntry(&dirEntry);

        char filename[50];
        file.getName(filename, sizeof(filename));
        String filenameStr(filename);

        if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
        {
            if (!SDfat.remove(filename))
            {
                Serial.println("Failed to delete file: " + filenameStr);
            }
        }

        file.close();
    }
    
}

bool SD_Manager::delete_file(String filename)
{     
    if (SDfat.remove(filename.c_str()))
    {
        return true;
    }
    else{
        return false;
    }
}

bool SD_Manager::connect_sd(){
    if (this->SDfat.begin(4, SD_SCK_MHZ(26))){
        this->logger_manager_ptr->debug("[SD_Manager.cpp] SD card initialized");
        return true;
    }
    else
    {

        this->logger_manager_ptr->critical("[SD_Manager.cpp] SD card not initialized");
        exit(-1);
    }
    return false;
}
