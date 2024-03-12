#pragma once

#include <Arduino.h>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct
{
  volatile float value               = 0;
  volatile uint_least8_t measurement = 0;
  volatile uint_least64_t time_stamp = 0;
} Data;


class Data_Manager
{
public:
  Data_Manager()
  {
  }

  ~Data_Manager() {}
  
  std::vector<Data *> data;
private:
};