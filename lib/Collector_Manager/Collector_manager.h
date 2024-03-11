#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>
#include "Data_Manager.h"
#include "driver/gpio.h"  
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

class Collector_manager
{
public:
    Collector_manager()
    {
    }

    ~Collector_manager() {}

    bool first_read = true;
    uint64_t timer_now = 0;
    uint64_t timer_previous = 0;
    volatile double value = 0;
    volatile char measurement[10] = {""};
    volatile uint_least64_t time_stamp = 0;
    data TMP;
    void read_mode1()
    { 
        if (this->first_read)
        {
            this->first_read = false;

            this->TMP.value = 32;
            memccpy((void *)this->TMP.measurement, "Dado 1", sizeof("Dado 1"));
            
            this->timer_now = esp_timer_get_time();
            this->TMP.time_stamp = 0;
            this->timer_previous = timer_now;
        }
        else
        {
            
            this->TMP.value = 32;
            memccpy((void *)this->TMP.measurement, "Dado 1", sizeof("Dado 1"));
            
            this->timer_now = esp_timer_get_time();
            this->TMP.time_stamp = timer_now - timer_previous;
            this->timer_previous = timer_now;
            
        }
        xQueueSend(this->xQueue, &TMP, portMAX_DELAY); 
    }

private:
    QueueHandle_t xQueue;
    SemaphoreHandle_t Key;
    uint32_t NLeituras = 5000;
    unsigned long long readings = 0;
    unsigned long repeticoes = 1;
};