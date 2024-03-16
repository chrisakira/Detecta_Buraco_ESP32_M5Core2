#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>

#include "driver/gpio.h"
#include "SdFat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "Data_Manager.h"
#include "XQueue_Manager.h"
#include "Logger_Manager.h"

class SD_Manager
{
public:
    SD_Manager( Data_Manager* data_manager_ptr = NULL, 
                XQueue_Manager* xqueue_manager_ptr = NULL, 
                Logger_Manager* logger_manager_ptr = NULL): 
                data_manager_ptr(data_manager_ptr), 
                xqueue_manager_ptr(xqueue_manager_ptr), 
                logger_manager_ptr(logger_manager_ptr) {
        this->xMutex = xSemaphoreCreateMutex();
    }

    ~SD_Manager() {}

    bool create_tasks();

    void double_to_uint8_array(double value, uint8_t* array);
    
    void uint32_to_uint8_array(uint32_t value, uint8_t* array);

    void receive_data_buffer(void *z);

    void write_data_buffer(void *z);
    
private: 
    bool is_task_created = false;
    Logger_Manager* logger_manager_ptr;
    XQueue_Manager* xqueue_manager_ptr;
    Data_Manager* data_manager_ptr;
    
    SemaphoreHandle_t xMutex;

    uint8_t Buffer[6000U] = {};
    TaskHandle_t receive_data_buffer_task_handle;
    TaskHandle_t write_data_buffer_task_handle;
    SdFat SD2;
    SdFile_ myFile;
};