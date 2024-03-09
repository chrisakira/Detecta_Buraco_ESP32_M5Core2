#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>
#include "Data_Manager.h"
#include "driver/gpio.h"
#include "SdFat.h"


class SD_Manager
{
public:
    SD_Manager()
    {
    }

    ~SD_Manager() {}

     

    void double_to_uint8_array(double value, uint8_t* array)
    {
        uint32_t intValue = *(uint32_t*)&value;
        for (int i = 0; i < sizeof(double); i++)
        {
            array[i] = (intValue >> (i * 8)) & 0xFF;
        }
    }

    void uint32_to_uint8_array(uint32_t value, uint8_t* array)
    {
        for (int i = 0; i < sizeof(uint32_t); i++)
        {
            array[i] = (value >> (i * 8)) & 0xFF;
        }
    }

    void SDBurner(void *z)
    {
        uint16_t size = 0;
        uint8_t value_array[4] = {};
        uint8_t timeStamp_array[4] = {};
        for (;;)
        {
            if (j <= 6000U)
            {

                while (xSemaphoreTake(Key, 0) != true)
                {
                }
                size = Data.size();
                if (size > 0)
                {
                    size--;
                    TMP = Data.front();
                    Data.erase(Data.begin());
                    xSemaphoreGive(Key);
                    
                    double_to_uint8_array(TMP->value, value_array);
                    for (int i = 0; i < 8; i++)
                    {
                        Buffer[j] = value_array[i];
                        j++;
                    }
                    uint32_to_uint8_array(TMP->timeStamp, timeStamp_array);
                    for (int i = 0; i < 4; i++)
                    {
                        Buffer[j] = timeStamp_array[i];
                        j++;
                    }
                    for (int i = 0; i < 10; i++)
                    {
                        Buffer[j] = TMP->measurement[i];
                        j++;
                    }
                }
                else
                {
                    xSemaphoreGive(Key);
                }
            }
            else
            {
                myFile.write(Buffer, 6000U);
                myFile.flush();
                j = 0;
            }
        }
    }

private: 

    SemaphoreHandle_t Key;
    data *TMP;
    
    uint16_t j = 0;
    uint8_t Buffer[6000U] = {};
    TaskHandle_t sdBurner;
    SdFat SD2;
    SdFile_ myFile;
};