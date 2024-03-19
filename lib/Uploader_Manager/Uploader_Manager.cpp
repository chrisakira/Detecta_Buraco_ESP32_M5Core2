#include <Uploader_Manager.h>
bool Uploader_Manager::get_alive()
{
    // if(WiFi.status() != WL_CONNECTED)
    //     return false;

    // this->http.begin(this->get_alive_url); // Remove the SSL certificate parameter
    // int httpCode = http.GET();
    // if(this->logger_manager_ptr!=NULL){
    //     this->logger_manager_ptr->info("[Uploader_Manager] GET alive returned code: " + String(httpCode));
    // }
    // http.end();
    // if(httpCode > 0)
    //     return true;
    // else
    //     return false;
    return true;
}

bool Uploader_Manager::post_file(String file_path)
{
    if (WiFi.status() != WL_CONNECTED)
        return false;
    
    this->logger_manager_ptr->info("[Uploader_Manager] Openning file " + file_path);


    String file_header = "attachment; filename= " + file_path;
    this->http.begin(this->post_file_url);
    this->http.addHeader("Content-Type", "application/octet-stream");
    this->http.addHeader("mac_address", WiFi.macAddress().c_str());
    this->http.addHeader("Content-Disposition", file_header.c_str());
    File_ file = this->sd_manager_ptr->SDfat.open(file_path.c_str(), O_READ);
    int httpCode = this->http.POST(file.readString());
    if (this->logger_manager_ptr != NULL)
    {
        if (httpCode > 0)
        {
            this->logger_manager_ptr->info("[Uploader_Manager] POST file returned code: " + String(httpCode));
        }
        else
        {
            this->logger_manager_ptr->error("[Uploader_Manager] POST file returned code: failed, error: " + this->http.errorToString(httpCode));
        }
    }
    this->http.end();
    if (httpCode > 0)
    {
        this->sd_manager_ptr->delete_file(file_path);
        this->logger_manager_ptr->info("[Uploader_Manager] File " + file_path + " posted");
        return true;
    }
    else{
        this->sd_manager_ptr->delete_file(file_path);
        return false;
    }
}

bool Uploader_Manager::uploader()
{
    if (this->get_alive() == false)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Uploader_Manager] Not Alive"); 
        return false;
    } 
    xSemaphoreTake(*this->xMutex, portMAX_DELAY); 
     
    String file_path = this->sd_manager_ptr->get_oldest_file("/");
    if (file_path != "" && file_path.indexOf("/System") == -1 && file_path.indexOf("not-ready") == -1)
    {
        this->logger_manager_ptr->info("[Uploader_Manager] File to upload BAAAATATA: " + file_path);
        if (this->post_file(file_path) == true)
        {
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[Uploader_Manager] File posted");
             
            xSemaphoreGive(*this->xMutex);
            return true;
        }
        else
        {
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->error("[Uploader_Manager] File not posted"); 
            xSemaphoreGive(*this->xMutex);
            return false;
        }
    }
    else
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[Uploader_Manager] No file to upload"); 
        xSemaphoreGive(*this->xMutex);
        return false;
    }
    xSemaphoreGive(*this->xMutex); 
}

void Uploader_Manager::uploader_task(void *z)
{
    for (;;)
    {
        uploader();
        vTaskDelay(5000);
    }
}

bool Uploader_Manager::create_task()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<Uploader_Manager *>(z)->uploader_task(z); },
                                    "Uploader", 10000, this, 1, &this->uploader_task_handler, 0);
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[Uploader_Manager] Uploader Task Created");
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Uploader_Manager] Uploader Task not created \n[Uploader_Manager] " + String(e.what()));
        return false;
    }
    return false;
}
