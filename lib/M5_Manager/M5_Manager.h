#pragma once

#include <Arduino.h>
#include <M5Core2.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class M5_Manager {
public:
    M5_Manager( bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, 
                bool I2CEnable = true, mbus_mode_t mode = kMBusModeOutput, bool SpeakerEnable = false){
        M5.begin(LCDEnable, SDEnable, SerialEnable, I2CEnable, mode, SpeakerEnable);
        this->initialize_LCD();
        
        xTaskCreate([](void* z) {
            static_cast<M5_Manager*>(z)->update_mpu_data(z);
        }, "Update the MPU data", 10000, this, 1, &update_mpu_data_task_handle);
    }

    ~M5_Manager(){}

    bool initialize_LCD();

    bool reset_LCD();
    
    void update_mpu_data(void *z);
    
    

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
private:

    TaskHandle_t update_mpu_data_task_handle;
};