#include <Arduino.h>
#include <M5Core2.h>
#include <vector>
#include "driver/gpio.h"
#include "SdFat.h"
#include "M5_Manager.h"
#include "SD_Manager.h"
#include "Data_Manager.h"
#include "XQueue_Manager.h"
#include "Logger_Manager.h"
#include "Uploader_Manager.h"

SemaphoreHandle_t xMutex;

Logger_Manager logger_manager(INFO);
M5_Manager m5_manager(&logger_manager);
XQueue_Manager xqueue_manager("Main_Queue");
SD_Manager sd_manager(&xqueue_manager, &logger_manager, &m5_manager, &xMutex);
Uploader_Manager uploader_manager("http://7e3a-177-220-186-45.ngrok-free.app", &logger_manager, &sd_manager, &xMutex);

TickType_t lastWakeTime = xTaskGetTickCount();
const TickType_t interval = pdMS_TO_TICKS(50);
Data data;

void setup() {
    xMutex = xSemaphoreCreateMutex();
    xqueue_manager.create_queue();
    m5_manager.M5_begin(false, false, false, false);
    m5_manager.connect_wifi();
    m5_manager.update_unix_time();
    uploader_manager.get_alive();
    sd_manager.connect_sd();
    
    logger_manager.info("Erasing SD Card");
    sd_manager.erase_all_files();
    logger_manager.info("Erased SD Card");
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

    if(uploader_manager.create_task())
        logger_manager.info("Uploader Task Created");
    else{
        logger_manager.error("Uploader Task not created");
        logger_manager.flush();
        exit(-1);
    }
    
}

void loop() {
    float pitch = m5_manager.pitch;
    float roll = m5_manager.roll;
    float yaw = m5_manager.yaw;
    
    uint_fast64_t  timestamp = ((uint_fast64_t )m5_manager.now)*1000000 + (esp_timer_get_time() - m5_manager.micros_now);

    data.time_stamp = timestamp;
    
    data.measurement = 0;
    data.value = pitch;
    if (xqueue_manager.send_data(&data) != pdTRUE)
        logger_manager.error("[Collector_Manager.cpp] Pitch Data not Sent");

    data.measurement = 1;
    data.value = roll;
    if (xqueue_manager.send_data(&data) != pdTRUE)
        logger_manager.error("[Collector_Manager.cpp] Roll Data not Sent");

    data.measurement = 2;
    data.value = yaw;
    if (xqueue_manager.send_data(&data) != pdTRUE)
        logger_manager.error("[Collector_Manager.cpp] Yaw Data not Sent");
    vTaskDelayUntil(&lastWakeTime, interval);

}
