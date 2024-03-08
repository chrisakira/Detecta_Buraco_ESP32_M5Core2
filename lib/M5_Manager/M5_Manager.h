#pragma once

#include <Arduino.h>
#include <M5Core2.h>

class M5_Manager {
public:
    M5_Manager( bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, 
                bool I2CEnable = true, mbus_mode_t mode = kMBusModeOutput, bool SpeakerEnable = false){
        M5.begin(LCDEnable, SDEnable, SerialEnable, I2CEnable, mode, SpeakerEnable);
        this->initialize_LCD();
    }

    ~M5_Manager(){}

    bool initialize_LCD();

    bool reset_LCD();
};