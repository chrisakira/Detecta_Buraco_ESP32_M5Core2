#include <SD_Manager.h>

void SD_Manager::double_to_uint8_array(double value, char *array)
{
    uint32_t intValue = *(uint32_t *)&value;
    for (int i = 0; i < sizeof(double); i++)
    {
        array[i] = (intValue >> (i * 8)) & 0xFF;
    }
}

void SD_Manager::float_to_uint8_array(float value, char *array)
{
    uint32_t intValue = *(uint32_t *)&value;
    for (int i = 0; i < sizeof(float); i++)
    {
        array[i] = (intValue >> (i * 8)) & 0xFF;
    }
}

void SD_Manager::uint32_to_uint8_array(uint32_t value, char *array)
{
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        array[i] = (value >> (i * 8)) & 0xFF;
    }
}

void SD_Manager::write_data_buffer(void *z)
{
    std::vector<Data *> data;
    data.clear();
    data.shrink_to_fit();
    for (;;)
    {

        if (this->data_manager_ptr->data.size() >= MAX_BUFFER_SIZE)
        {
            xSemaphoreTake(this->xMutex, portMAX_DELAY);
            auto temp = std::vector<Data *>(std::make_move_iterator(this->data_manager_ptr->data.begin()), std::make_move_iterator(this->data_manager_ptr->data.end()));
            this->data_manager_ptr->data.clear();
            this->data_manager_ptr->data.shrink_to_fit();
            xSemaphoreGive(this->xMutex);
            this->logger_manager_ptr->debug("[SD_Manager.cpp] Data size: " + String(temp.size()));
            
            String filename = String(m5_manager_ptr->get_current_time()) + String("not-ready.aki");
            String filename_ready = String(m5_manager_ptr->get_current_time()) + String(".aki");
            logger_manager_ptr->debug("[SD_Manager.cpp] Filename: " + filename);
            if (myFile.open(filename.c_str(), O_WRITE | O_CREAT))
                logger_manager_ptr->debug("[SD_Manager.cpp] File opened");
            else
            {
                logger_manager_ptr->critical("[SD_Manager.cpp] File not opened");
                exit(-1);
            }
            while (temp.empty() != true)
            { 
                if (temp.empty())
                    break;

                Data *tmp = temp.back();

                myFile.write((uint8_t *)&tmp->value, sizeof(tmp->value));
                myFile.write((uint8_t *)&tmp->time_stamp, sizeof(tmp->time_stamp));
                myFile.write((uint8_t *)&tmp->measurement, sizeof(tmp->measurement));

                delete tmp;
                temp.pop_back();
            }
            myFile.print("#EOF");
            myFile.flush();
            myFile.rename(filename_ready.c_str());

            myFile.close();
            logger_manager_ptr->info("[SD_Manager.cpp] Data written to SD");
        }
        vTaskDelay(10);
    }
}

void SD_Manager::receive_data_buffer(void *z)
{
    Data tmp;
    for (;;)
    {
        if (this->xqueue_manager_ptr->receive_data(&tmp))
        {
            xSemaphoreTake(this->xMutex, portMAX_DELAY);
            if (this->data_manager_ptr->data.size() >= MAX_BUFFER_SIZE * 2)
            {
                delete *this->data_manager_ptr->data.begin();
                this->data_manager_ptr->data.erase(this->data_manager_ptr->data.begin());
                this->data_manager_ptr->data.shrink_to_fit();
            }
            this->data_manager_ptr->data.push_back(new Data(tmp));
            xSemaphoreGive(this->xMutex);
        }
        else 
            vTaskDelay(1);
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
                        "Receive Data Buffer", 10000, this, 4, &receive_data_buffer_task_handle,0);
            xTaskCreatePinnedToCore([](void *z)
                        { static_cast<SD_Manager *>(z)->write_data_buffer(z); },
                        "Write Data Buffer", 10000, this, 1, &write_data_buffer_task_handle,1);
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
        if(filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
            return String("/") + String(filename);
        else
            return "";
    }
    else
    {
        return "";
    }
}

bool SD_Manager::delete_file(String filename)
{ 
    if (SDfat.remove(filename.c_str()))
        return true;
    else
        return false;
}
