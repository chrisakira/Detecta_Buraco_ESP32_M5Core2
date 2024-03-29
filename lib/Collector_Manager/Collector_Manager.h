#pragma once

#include <Arduino.h>

#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"

#include "Logger_Manager.h" 
#define Buffer_size 200000U

class Collector_Manager
{
public:
  Collector_Manager(
                    Logger_Manager *logger_manager_ptr = NULL,
                    SemaphoreHandle_t *xMutex = NULL, 
                    SemaphoreHandle_t *xSemaphore = NULL): 
                    
                    logger_manager_ptr(logger_manager_ptr),
                    xMutex(xMutex),
                    xSemaphore(xSemaphore){}

  ~Collector_Manager(){}

  void add_sample(double value, int measurement, uint_fast64_t timestamp);
  bool alloc_buffers(){ 
    buffer_read  = (uint8_t*) heap_caps_malloc(Buffer_size, MALLOC_CAP_SPIRAM);
    buffer_write = (uint8_t*) heap_caps_malloc(Buffer_size, MALLOC_CAP_SPIRAM);
    log_d("Total PSRAM: %d", ESP.getPsramSize());
    log_d("Used PSRAM: %d", ESP.getPsramSize() - ESP.getFreePsram()); 
    log_d("Free PSRAM: %d", ESP.getFreePsram());
    return true;
  }
  uint8_t* buffer_read  = NULL;
  uint8_t* buffer_write = NULL;
  bool write = false;
private:

  uint_fast64_t timer_to_read = 0;
  
  Logger_Manager *logger_manager_ptr = NULL;

  SemaphoreHandle_t *xMutex;
  SemaphoreHandle_t *xSemaphore;

  uint32_t j = 0;
  uint8_t buffer_8_bytes[8U] = {0};
  uint8_t buffer_4_bytes[4U] = {0};
};