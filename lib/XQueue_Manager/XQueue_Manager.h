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
  XQueue_Manager(String queue_name = "Default"): queue_name(queue_name) {}

  ~XQueue_Manager() {
  }
  
  QueueHandle_t* get_queue_ptr();

  int receive_data(Data *data);

  bool send_data(Data* data);

  String get_queue_name() {
    return this->queue_name;
  }
private:
  String queue_name = "Default";
  QueueHandle_t xQueue;
};