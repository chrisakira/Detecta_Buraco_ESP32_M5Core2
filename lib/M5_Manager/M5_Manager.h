#pragma once

#include <Arduino.h>
#include <M5Core2.h>
#include <TinyGPS++.h> 
#include <WiFi.h>
#include <ctime>
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Logger_Manager.h"
class M5_Manager {
public:
    M5_Manager(Logger_Manager *logger_manager_ptr): logger_manager_ptr(logger_manager_ptr){}

    ~M5_Manager(){}

    void M5_begin(bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, bool I2CEnable = true);
    
    bool check_LCD();

    bool initialize_LCD();

    bool reset_LCD();
    
    bool create_tasks();

    void update_mpu_data(void *z);
    
    void write_mpu_data(void *z);
    
    const char *get_current_time();
    
    bool connect_wifi();

    void update_unix_time();

    float accel_X = 0.0F;
    float accel_Y = 0.0F;
    float accel_Z = 0.0F;

    float gyro_X = 0.0F;
    float gyro_Y = 0.0F;
    float gyro_Z = 0.0F;

    float pitch = 0.0F;
    float roll  = 0.0F;
    float yaw   = 0.0F;

    float temperature = 0.0F;
    time_t now = 0;
    uint_fast64_t micros_now = 0;
private:

    static const int RXPin = SCL, TXPin = SDA;
    static const uint32_t GPSBaud = 9600;

    TinyGPSPlus gps; 
    const char* ssid             = "Maekawa";
    const char* password         = "GETWICKED";
    const char* ntpServer        = "pool.ntp.org"; 
    const long gmtOffset_sec     = -10800; // GMT offset for Sao Paulo (UTC-3)
    const int daylightOffset_sec = 3600;
    RTC_TimeTypeDef RTCtime;
    RTC_DateTypeDef RTCDate;


    Logger_Manager *logger_manager_ptr = NULL;
    bool lcd_initialized = false;
    bool is_task_created = false;
    TaskHandle_t update_mpu_data_task_handle;
};