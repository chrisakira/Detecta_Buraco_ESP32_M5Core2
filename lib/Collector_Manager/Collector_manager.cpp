#include <Collector_Manager.h>

bool Collector_Manager::collector(Data *data)
{
    float pitch = m5_manager_ptr->pitch;
    float roll = m5_manager_ptr->roll;
    float yaw = m5_manager_ptr->yaw;
    
    uint_fast64_t  timestamp = ((uint_fast64_t )m5_manager_ptr->now)*1000000 + (esp_timer_get_time() - m5_manager_ptr->micros_now);

    data->time_stamp = timestamp;
    
    data->measurement = 0;
    data->value = pitch;
    if (xqueue_manager_ptr->send_data(data) != pdTRUE)
        logger_manager_ptr->error("[Collector_Manager.cpp] Pitch Data not Sent");

    data->measurement = 1;
    data->value = roll;
    if (xqueue_manager_ptr->send_data(data) != pdTRUE)
        logger_manager_ptr->error("[Collector_Manager.cpp] Roll Data not Sent");

    data->measurement = 2;
    data->value = yaw;
    if (xqueue_manager_ptr->send_data(data) != pdTRUE)
        logger_manager_ptr->error("[Collector_Manager.cpp] Yaw Data not Sent");

    return true;
}

void Collector_Manager::collector_thread(void *z)
{
    Data data;
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = pdMS_TO_TICKS(100);

    for (;;)
    {
        collector(&data);
        vTaskDelayUntil(&lastWakeTime, interval);
    }
}

bool Collector_Manager::create_task()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<Collector_Manager *>(z)->collector_thread(z); },
                                    "Data Collector", 10000, this, 1, &this->collector_task_handler, 0);
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[Collector_Manager] Collector Task Created");
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Collector_Manager] Collector Task not created \n[Collector_Manager] " + String(e.what()));
        return false;
    }
    return false;
}
