#pragma once

#include <Arduino.h>
#include <String>
#include <vector>

#include "Data_Manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

class XQueue_Manager
{
public:
  XQueue_Manager(String queue_name = "Default"): queue_name(queue_name) {
   
  }

  ~XQueue_Manager() {
  }
  
  QueueHandle_t* get_queue_ptr();

  BaseType_t receive_data(Data *data);

  BaseType_t send_data(Data* data);
  void create_queue(){
    xQueue = xQueueCreate(1000, sizeof(Data));
  }
  String get_queue_name() {
    return this->queue_name;
  }
  
  QueueHandle_t xQueue;
private:
  String queue_name = "Default";
};