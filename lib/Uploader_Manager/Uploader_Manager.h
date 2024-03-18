#pragma once

#include <WiFi.h> 
#include <string.h>
#include <Arduino.h>
#include <HTTPClient.h>

#include "Logger_Manager.h"
#include "SD_Manager.h"

class Uploader_Manager {
public:
    Uploader_Manager(   String base_url = "https://192.168.18.127/akirapi", 
                        Logger_Manager* logger_manager_ptr = NULL,
                        SD_Manager* sd_manager_ptr = NULL): 
                    base_url(base_url), 
                    get_alive_url(base_url + "/alive"), 
                    post_json_url(base_url + "/v1/data/json"), 
                    post_file_url(base_url + "/v1/data/file"), 
                    ping_device_url(base_url + "/v1/ping"),
                    logger_manager_ptr(logger_manager_ptr),
                    sd_manager_ptr(sd_manager_ptr){       
    }

    ~Uploader_Manager(){} 

    bool get_alive();
    bool post_file(String file_path = "");
    void uploader_task(void *z);
    bool create_task();
    bool uploader();

private: 
    bool is_task_created = false;
    SD_Manager* sd_manager_ptr;
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