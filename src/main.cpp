#include <Arduino.h>

#include "M5_Manager.h"
#include "SD_Manager.h"
#include "Collector_Manager.h"
#include "Logger_Manager.h"
#include "Uploader_Manager.h"
#include <TinyGPSPlus.h>


// The TinyGPSPlus object


TinyGPSPlus gps;
SemaphoreHandle_t xMutex = NULL;
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore_Uploader = NULL;

Logger_Manager logger_manager(INFO);
M5_Manager m5_manager(&logger_manager);
Collector_Manager collector_manager(&logger_manager, &xMutex, &xSemaphore);
Uploader_Manager uploader_manager("http://d627-177-92-54-149.ngrok-free.app", &logger_manager, &xMutex, &xSemaphore_Uploader);
SD_Manager sd_manager(&logger_manager, &m5_manager, &collector_manager, &uploader_manager, &xMutex, &xSemaphore, &xSemaphore_Uploader);

TickType_t lastWakeTime = xTaskGetTickCount();
const TickType_t interval = pdMS_TO_TICKS(5);
void displayInfo();

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, SDA, SCL);
    while (1)
    {
        while (Serial2.available() > 0)
            if (gps.encode(Serial2.read()))
                displayInfo();
        if (millis() > 5000 && gps.charsProcessed() < 10)
        {
            Serial.println(F("No GPS detected: check wiring."));
        }
    }
    disableCore0WDT();
    xMutex = xSemaphoreCreateMutex();
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphore_Uploader = xSemaphoreCreateBinary();

    sd_manager.set_logger_manager(&logger_manager);
    m5_manager.M5_begin(false, false, false, false);
    m5_manager.connect_wifi();
    m5_manager.update_unix_time();
    // uploader_manager.get_alive();
    sd_manager.connect_sd();
    sd_manager.erase_all_files();
    uploader_manager.alloc_buffers();
    collector_manager.alloc_buffers();

    log_d("Total heap: %d", ESP.getHeapSize());
    log_d("Free heap: %d", ESP.getFreeHeap());
    log_d("Total PSRAM: %d", ESP.getPsramSize());
    log_d("Free PSRAM: %d", ESP.getFreePsram());
    if (m5_manager.create_tasks())
        logger_manager.info("M5 manager Tasks created");

    else
    {
        logger_manager.error("M5 Manager Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    if (sd_manager.create_tasks())
        logger_manager.info("SD Tasks Created");
    else
    {
        logger_manager.error("SD Tasks not created");
        logger_manager.flush();
        exit(-1);
    }

    // if(uploader_manager.create_task())
    //     logger_manager.info("Uploader Task Created");
    // else{
    //     logger_manager.error("Uploader Task not created");
    //     logger_manager.flush();
    //     exit(-1);
    // }
}

void loop()
{
    float pitch = m5_manager.pitch;
    float roll = m5_manager.roll;
    float yaw = m5_manager.yaw;
    uint_fast64_t timestamp = ((uint_fast64_t)m5_manager.now) * 1000000 + (esp_timer_get_time() - m5_manager.micros_now);
    collector_manager.add_sample(pitch, 0, timestamp);
    collector_manager.add_sample(roll, 1, timestamp);
    collector_manager.add_sample(yaw, 2, timestamp);
    vTaskDelayUntil(&lastWakeTime, interval);
}

void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
        Serial.print("Lat: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print("Lng: ");
        Serial.print(gps.location.lng(), 6);
        Serial.println();
    }
    else
    {
        Serial.println(F("INVALID"));
    }
}
