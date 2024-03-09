#pragma once

#include <Arduino.h>
#include <vector>

typedef struct
{
  volatile double value = 0;
  volatile char measurement[10] = {""};
  volatile uint_least64_t timeStamp = 0;
} data;

std::vector<data*> Data;

class Data_Manager
{
public:
    Data_Manager()
    {
    }

    ~Data_Manager() {}

private: 

};