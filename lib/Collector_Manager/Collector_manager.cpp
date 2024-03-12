#include <Collector_Manager.h>
 
void Collector_Manager::collector_thread(void *z)
{ 
    Data data;
    for (;;)
    {
        data.measurement = 0;
        data.value = m5_manager_ptr->pitch;
        data.time_stamp = esp_timer_get_time();
        vTaskDelay(1000);
    }
}
