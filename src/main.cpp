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
#include "Uploader_Manager.h"

Logger_Manager logger_manager(ERROR);
M5_Manager m5_manager(&logger_manager);
Data_Manager SD_data;
Data_Manager Collector_data;
XQueue_Manager xqueue_manager("Main_Queue");
Collector_Manager collector_manager(&Collector_data, &xqueue_manager, &logger_manager, &m5_manager);
SD_Manager sd_manager(&SD_data, &xqueue_manager, &logger_manager);
Uploader_Manager uploader_manager();



void setup() {  
    m5_manager.M5_begin(true, false, false, true);
    if (m5_manager.create_tasks())
        logger_manager.info("M5 manager Tasks created");
    
    else{
        logger_manager.error("M5 Manager Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    if(sd_manager.create_tasks())
        logger_manager.info("SD Tasks Created");
    else{
        logger_manager.error("SD Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    if(collector_manager.create_task())
        logger_manager.info("Collector Task Created");
    else{
        logger_manager.error("Collector Task not created");
        logger_manager.flush();
        exit(-1);
    }

}

void loop() {
    
}
