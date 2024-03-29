#pragma once

#include <WiFi.h> 
#include <string.h>
#include <Arduino.h>
#include <HTTPClient.h>

#include "freertos/semphr.h"

#include "SdFat.h"
#include "Logger_Manager.h"
#include "Collector_Manager.h"
class Uploader_Manager {
public:
    Uploader_Manager(   String base_url = "https://192.168.18.127/akirapi", 
                        Logger_Manager* logger_manager_ptr = NULL,
                        SemaphoreHandle_t* xMutex = NULL,
                        SemaphoreHandle_t* xSemaphore_Uploader = NULL): 

                    base_url(base_url), 
                    get_alive_url(base_url + "/alive"), 
                    post_json_url(base_url + "/v1/data/json"), 
                    post_file_url(base_url + "/v1/data/file"), 
                    ping_device_url(base_url + "/v1/ping"),
                    logger_manager_ptr(logger_manager_ptr),
                    xMutex(xMutex){  
        this->http.setReuse(true);
        this->http.setTimeout(5000);     
    }

    ~Uploader_Manager(){} 

    bool get_alive();
    bool post_file(String file_path = "", SdFat *SDfat = NULL);
    void uploader_task(void *z);
    bool create_task();
    bool uploader();
    void alloc_buffers(){
        buffer = (uint8_t*) ps_malloc(Buffer_size);
    }
    uint8_t *buffer = NULL;
private: 
    bool is_task_created = false;

    SemaphoreHandle_t* xMutex;
    SemaphoreHandle_t* xSemaphore_Uploader;
    
    Logger_Manager* logger_manager_ptr;

    TaskHandle_t uploader_task_handler;
    WiFiClientSecure client;
    HTTPClient http; 
    String base_url = "";
    String get_alive_url = "";
    String post_json_url = ""; 
    String post_file_url = "";
    String ping_device_url = "";
};