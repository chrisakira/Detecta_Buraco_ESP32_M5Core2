#include <SD_Manager.h>

void SD_Manager::double_to_uint8_array(double value, uint8_t *array)
{
    uint32_t intValue = *(uint32_t *)&value;
    for (int i = 0; i < sizeof(double); i++)
    {
        array[i] = (intValue >> (i * 8)) & 0xFF;
    }
}

void SD_Manager::uint32_to_uint8_array(uint32_t value, uint8_t *array)
{
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        array[i] = (value >> (i * 8)) & 0xFF;
    }
}

void SD_Manager::write_data_buffer(void *z)
{
    uint16_t max_buffer_size = 6000U;
    
    for (;;)
    {

        if(this->data_manager_ptr->data.size() > max_buffer_size){
            xSemaphoreTake(this->xMutex, portMAX_DELAY);
            std::vector<Data*> data(std::make_move_iterator(this->data_manager_ptr->data.begin()), std::make_move_iterator(this->data_manager_ptr->data.end()));
            this->data_manager_ptr->data.clear();
            this->data_manager_ptr->data.shrink_to_fit();
            xSemaphoreGive(this->xMutex);


        }
        else
            vTaskDelay(1);
        //     double_to_uint8_array(TMP->value, value_array);
        //     for (int i = 0; i < 8; i++)
        //     {
        //         Buffer[j] = value_array[i];
        //         j++;
        //     }
        //     uint32_to_uint8_array(TMP->timeStamp, timeStamp_array);
        //     for (int i = 0; i < 4; i++)
        //     {
        //         Buffer[j] = timeStamp_array[i];
        //         j++;
        //     }
        //     for (int i = 0; i < 10; i++)
        //     {
        //         Buffer[j] = TMP->measurement[i];
        //         j++;
        //     }
    
        // {
        //     myFile.write(Buffer, 6000U);
        //     myFile.flush();
        //     j = 0;
        // }
        vTaskDelay(100);
    }
}

void SD_Manager::receive_data_buffer(void *z)
{
    Data tmp;
    for (;;)
    {
        if (xqueue_manager_ptr->receive_data(&tmp))
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            data_manager_ptr->data.push_back(new Data(tmp));
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1);
    }
}

void SD_Manager::create_tasks()
{
    xTaskCreatePinnedToCore([](void* z) {
        static_cast<SD_Manager*>(z)->receive_data_buffer(z);
    }, "Receive Data Buffer", 10000, this, 1, &receive_data_buffer_task_handle, 1);
    xTaskCreatePinnedToCore([](void* z) {
        static_cast<SD_Manager*>(z)->write_data_buffer(z);
    }, "Write Data Buffer", 10000, this, 1, &write_data_buffer_task_handle, 1);
}