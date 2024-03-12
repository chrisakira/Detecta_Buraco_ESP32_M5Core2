#include <Arduino.h> 
#include <M5Core2.h>
#include <vector>
#include "driver/gpio.h"
#include "SdFat.h"
#include "M5_Manager.h"
#include "SD_Manager.h"
#include "Data_Manager.h"
#include "XQueue_Manager.h"
#include "Collector_Manager.h"
#include "Logger_Manager.h"

Logger_Manager logger_manager;
M5_Manager m5_manager(true, true, false, true);
Data_Manager SD_data;
Data_Manager Collector_data;
XQueue_Manager xqueue_manager("Main_Queue");
Collector_Manager collector_manager(&Collector_data, &xqueue_manager, &logger_manager);
SD_Manager sd_manager(&SD_data, &xqueue_manager, &logger_manager);




void setup() {  
}

void loop() {
  
}
