#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "SdFat.h"

#include "Data_Manager.h"
#include "XQueue_Manager.h"
#include "Logger_Manager.h"
#include "M5_Manager.h"


class Collector_Manager
{
public:
    Collector_Manager(  Data_Manager* data_manager_ptr = NULL, 
                        XQueue_Manager* xqueue_manager_ptr = NULL, 
                        Logger_Manager* logger_manager_ptr = NULL,
                        M5_Manager* m5_manager_ptr = NULL): 
                        data_manager_ptr(data_manager_ptr), 
                        xqueue_manager_ptr(xqueue_manager_ptr), 
                        logger_manager_ptr(logger_manager_ptr),
                        m5_manager_ptr(m5_manager_ptr) {}

    ~Collector_Manager() {}

    void collector_thread(void *z);


private: 
    M5_Manager* m5_manager_ptr;
    Logger_Manager* logger_manager_ptr;
    Data_Manager* data_manager_ptr;
    XQueue_Manager* xqueue_manager_ptr;
};