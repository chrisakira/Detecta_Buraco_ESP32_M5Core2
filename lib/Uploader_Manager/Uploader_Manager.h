#pragma once

#include <WiFi.h> 
#include <string.h>
#include <Arduino.h>
#include <HTTPClient.h>
class Uploader_Manager {
public:
    Uploader_Manager(String base_url = "https://www.akira-maker.com/iot-metrics-hub"): 
                    base_url(base_url), 
                    get_alive_url(base_url + "/alive"), 
                    post_json_url(base_url + "/data/json"), 
                    post_file_url(base_url + "/data/file"), 
                    ping_device_url(base_url + "/ping")
                    {}

    ~Uploader_Manager(){} 

    bool get_alive();

private: 
    String base_url = "";
    String get_alive_url = "";
    String post_json_url = ""; 
    String post_file_url = "";
    String ping_device_url = "";
};