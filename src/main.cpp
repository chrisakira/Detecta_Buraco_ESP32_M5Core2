#include <Arduino.h> 
#include <M5Core2.h>
#include <vector>
#include "driver/gpio.h"
#include "SdFat.h"
#include "M5_Manager.h"

M5_Manager m5_manager;

// Semaphore Key
SemaphoreHandle_t Key;

// TASK CONTROLLERS
TaskHandle_t sdBurner;

SdFat SD2;
SdFile_ myFile;
typedef struct
{
  volatile uint_least16_t readSens1 = 0;
  volatile uint_least16_t readSens2 = 0;
  volatile uint_least16_t timeStamp = 0;
} data;

std::vector<data*> Data;
data *TMP;
uint16_t j = 0;
uint8_t Buffer[6000U] = {};
void SDBurner(void *z)
{
  uint16_t size = 0;
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

        Buffer[j] = TMP->timeStamp >> 8;
        j++;
        Buffer[j] = TMP->timeStamp;
        j++;
        Buffer[j] = TMP->readSens1 >> 8;
        j++;
        Buffer[j] = TMP->readSens1;
        j++;
        Buffer[j] = TMP->readSens2 >> 8;
        j++;
        Buffer[j] = TMP->readSens2;
        j++;
      }
      else
      {
        xSemaphoreGive(Key);
      }
    }
    else
    {
      myFile.write(Buffer, 6000U);
      j = 0;
    }
  }
}

void setup() {  
  
}

void loop() {
  
}
