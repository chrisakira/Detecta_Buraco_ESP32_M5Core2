#include <Logger_Manager.h>

void Logger_Manager::info(String message)
{
    if (this->log_level >= INFO)
        Serial.print(message);
}

void Logger_Manager::debug(String message)
{
    if (this->log_level >= DEBUG)
        Serial.print(message);
}

void Logger_Manager::warning(String message)
{
    if (this->log_level >= WARNING)
        Serial.print(message);
}

void Logger_Manager::error(String message)
{
    if (this->log_level >= ERROR)
        Serial.print(message);
}

void Logger_Manager::critical(String message)
{
    if (this->log_level >= CRITICAL)
        Serial.print(message);
}

