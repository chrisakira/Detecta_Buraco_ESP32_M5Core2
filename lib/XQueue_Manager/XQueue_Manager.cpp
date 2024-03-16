#include <XQueue_Manager.h>


QueueHandle_t* XQueue_Manager::get_queue_ptr()
{
    return &this->xQueue;
}

BaseType_t XQueue_Manager::receive_data(Data *data)
{
    return xQueueReceive(this->xQueue, data, portMAX_DELAY);
}

BaseType_t XQueue_Manager::send_data(Data* data)
{
    return xQueueSend(this->xQueue, data, portMAX_DELAY);
}