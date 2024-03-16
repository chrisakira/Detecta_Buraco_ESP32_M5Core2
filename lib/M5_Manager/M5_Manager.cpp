#include "M5_Manager.h"

bool M5_Manager::check_LCD()
{
    if (this->lcd_initialized == false)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[M5_Manager] LCD Not initialized");
        return false;
    }
    return true;
}

bool M5_Manager::initialize_LCD()
{
    if (this->check_LCD() == false)
        return false;
    M5.Lcd.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

bool M5_Manager::reset_LCD()
{
    if (this->check_LCD() == false)
        return false;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

void M5_Manager::update_mpu_data(void *z)
{
    M5.IMU.Init();
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = pdMS_TO_TICKS(1);

    for (;;)
    {
        M5.IMU.getAhrsData(&this->gyro_X, &this->gyro_Y, &this->gyro_Z, &this->accel_X, &this->accel_Y, &this->accel_Z, &this->pitch, &this->roll, &this->yaw);
        M5.IMU.getTempData(&this->temperature);

        vTaskDelayUntil(&lastWakeTime, interval);
    }
}

void M5_Manager::M5_begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable)
{
    M5.begin(LCDEnable, SDEnable, SerialEnable, I2CEnable);
    if (this->logger_manager_ptr != NULL)
        this->logger_manager_ptr->info("[M5_Manager] M5Core2 initialized");

    if (LCDEnable == true)
    {
        this->initialize_LCD();
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[M5_Manager] LCD initialized");
    }
}

bool M5_Manager::create_tasks()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreate([](void *z)
                        { static_cast<M5_Manager *>(z)->update_mpu_data(z); },
                        "Update the MPU data", 10000, this, 1, &this->update_mpu_data_task_handle);
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[M5_Manager] Update MPU Task created");
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[M5_Manager] MPU Updater Task not created \n[M5_Manager] " + String(e.what()));
        return false;
    }
    return false;
}


