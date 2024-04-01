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
Uploader_Manager uploader_manager("http://bbad-177-92-54-149.ngrok-free.app", &logger_manager, &xMutex, &xSemaphore_Uploader);
SD_Manager sd_manager(&logger_manager, &m5_manager, &collector_manager, &uploader_manager, &xMutex, &xSemaphore, &xSemaphore_Uploader);

TickType_t lastWakeTime = xTaskGetTickCount();
const TickType_t interval = pdMS_TO_TICKS(5);
void displayInfo();

void setup()
{
    disableCore0WDT();
    xMutex = xSemaphoreCreateMutex();
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphore_Uploader = xSemaphoreCreateBinary();

    sd_manager.set_logger_manager(&logger_manager);
    m5_manager.M5_begin(true, false, false, false);
    m5_manager.reset_LCD();
    M5.Lcd.println("Initializing...");
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
    M5.Lcd.println("Starting the data collection");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    m5_manager.reset_LCD();
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
    double Latitude = m5_manager.Latitude;
    double Longitude = m5_manager.Longitude;
    double Altitude = m5_manager.Altitude;
    double Speed = m5_manager.Speed;


    uint_fast64_t timestamp = ((uint_fast64_t)m5_manager.now) * 1000000 + (esp_timer_get_time() - m5_manager.micros_now);
    collector_manager.add_sample(pitch, 0, timestamp);
    collector_manager.add_sample(roll, 1, timestamp);
    collector_manager.add_sample(yaw, 2, timestamp);
    if(Latitude != 0.0F)
        collector_manager.add_sample(Latitude, 3, timestamp);

    if(Longitude != 0.0F)
        collector_manager.add_sample(Longitude, 4, timestamp);
    
    if(Altitude != 0.0F)
        collector_manager.add_sample(Altitude, 5, timestamp);
    
    if(Speed != 0.0F)
        collector_manager.add_sample(Speed, 6, timestamp);
        
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
