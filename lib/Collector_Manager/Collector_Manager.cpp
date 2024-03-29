#include <Collector_Manager.h>

void Collector_Manager::add_sample(const float value_, const int measurement_, const uint_fast64_t timestamp_)
{   
    float           value       = value_;
    int             measurement = measurement_;
    uint_fast64_t   timestamp   = timestamp_;
    
    memcpy(this->buffer_8_bytes, (uint8_t *)&timestamp, sizeof(timestamp));
        for (uint_fast8_t i = 0; i < 8; i++)
        {
            this->buffer_read[this->j] = this->buffer_8_bytes[i];
            this->j++;
        }
    memcpy(buffer_4_bytes, (uint8_t *)&value, sizeof(value));
        for (uint_fast8_t i = 0; i < 4; i++)
        {
            this->buffer_read[this->j] = this->buffer_4_bytes[i];
            this->j++;
        }
    memcpy(this->buffer_4_bytes, (uint8_t *)&measurement, sizeof(measurement));
        for (uint_fast8_t i = 0; i < 4; i++)
        {
            this->buffer_read[j] = this->buffer_4_bytes[i];
            this->j++;
        }
    if (this->j >= (Buffer_size))
    {
        this->j = 0;
        xSemaphoreTake(*this->xMutex, portMAX_DELAY);
        memcpy(this->buffer_write, this->buffer_read, sizeof(Buffer_size));
        xSemaphoreGive(*this->xMutex);

        this->logger_manager_ptr->debug("[Collector_Manager.cpp] Buffer ready");
        uint_fast64_t timer_now = esp_timer_get_time();
        this->logger_manager_ptr->debug(timer_now - this->timer_to_read);
        this->timer_to_read = timer_now;
        xSemaphoreGive(*this->xSemaphore);
    }
    
}