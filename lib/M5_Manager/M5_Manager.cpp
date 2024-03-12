#include "M5_Manager.h"


bool M5_Manager::initialize_LCD(){
    M5.Lcd.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

bool M5_Manager::reset_LCD(){ 
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

void M5_Manager::update_mpu_data(void *z){
    
    M5.IMU.Init();
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = pdMS_TO_TICKS(500);

    for (;;) {
        M5.IMU.getGyroData(&gyro_X, &gyro_Y, &gyro_Z);
        M5.IMU.getAccelData(&accel_X, &accel_Y, &accel_Z);
        M5.IMU.getAhrsData(&pitch, &roll, &yaw);
        M5.IMU.getTempData(&temperature);

        vTaskDelayUntil(&lastWakeTime, interval);
    }
}
