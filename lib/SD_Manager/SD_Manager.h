#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>

#include "driver/gpio.h"
#include "SdFat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Data_Manager.h"
#include "XQueue_Manager.h"
#include "Logger_Manager.h"
#include "M5_Manager.h"
#define MAX_BUFFER_SIZE 400U
class SD_Manager
{
public:
    SD_Manager( XQueue_Manager* xqueue_manager_ptr = NULL, 
                Logger_Manager* logger_manager_ptr = NULL,
                M5_Manager* m5_manager_ptr = NULL,
                SemaphoreHandle_t* xMutex = NULL): 
                xqueue_manager_ptr(xqueue_manager_ptr), 
                logger_manager_ptr(logger_manager_ptr),
                m5_manager_ptr(m5_manager_ptr),
                xMutex(xMutex) {
    }

    ~SD_Manager() {}

    bool create_tasks();

    void receive_data_buffer(void *z);

    void write_data_buffer(void *z);
    String get_oldest_file(const char* dirname);
    bool delete_file(String filename);
    void erase_all_files();
    bool connect_sd();


    SdFat SDfat;
private: 
    uint16_t file_counter = 0;
    bool is_task_created = false;
    SemaphoreHandle_t* xMutex;
    
    
    Logger_Manager* logger_manager_ptr;
    XQueue_Manager* xqueue_manager_ptr;
    M5_Manager* m5_manager_ptr;
    
    
    TaskHandle_t receive_data_buffer_task_handle;
    TaskHandle_t write_data_buffer_task_handle;
    SdFile_ myFile;
};