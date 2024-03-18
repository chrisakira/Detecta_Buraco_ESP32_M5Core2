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

Logger_Manager logger_manager(INFO);
M5_Manager m5_manager(&logger_manager);
Data_Manager SD_data;
Data_Manager Collector_data;
XQueue_Manager xqueue_manager("Main_Queue");
Collector_Manager collector_manager(&Collector_data, &xqueue_manager, &logger_manager, &m5_manager);
SD_Manager sd_manager(&SD_data, &xqueue_manager, &logger_manager, &m5_manager);
Uploader_Manager uploader_manager("http://7e3a-177-220-186-45.ngrok-free.app", &logger_manager, &sd_manager);



void setup() {
    
    m5_manager.M5_begin(true, true, false, true);
    m5_manager.connect_wifi();
    m5_manager.update_unix_time();
    uploader_manager.get_alive();
    
    if (sd_manager.SDfat.begin(4, SD_SCK_MHZ(26)))
        logger_manager.debug("[SD_Manager.cpp] SD card initialized");
    else
    {
        logger_manager.critical("[SD_Manager.cpp] SD card not initialized");
        exit(-1);
    }

    if (m5_manager.create_tasks()) // Não interage com SD, Nucleo 0
        logger_manager.info("M5 manager Tasks created");

    else{
        logger_manager.error("M5 Manager Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    if(sd_manager.create_tasks()) // Escritor manipula SD, Nucleo 1. Leitor não manipula SD, Nucleo 0.
        logger_manager.info("SD Tasks Created");
    else{
        logger_manager.error("SD Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    if(collector_manager.create_task()) // Não manipula SD, Nucleo 0
        logger_manager.info("Collector Task Created");
    else{
        logger_manager.error("Collector Task not created");
        logger_manager.flush();
        exit(-1);
    }

    if(uploader_manager.create_task()) // Manipula SD, Nucleo 1
        logger_manager.info("Uploader Task Created");
    else{
        logger_manager.error("Uploader Task not created");
        logger_manager.flush();
        exit(-1);
    }

}

void loop() {

}
