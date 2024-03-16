#include <Uploader_Manager.h>

bool Uploader_Manager::get_alive()
{
    // Make the GET request
    http.begin(client, url);
    int httpCode = http.GET();

    // Check the response
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String response = http.getString();
            Serial.println(response);
        } else {
            Serial.printf("HTTP request failed with error code: %d\n", httpCode);
        }
    } else {
        Serial.println("Connection failed");
    }

    // Disconnect from WiFi
    http.end();
    
    return true;
}

void makeGetRequest(const char* url) {
    WiFiClient client;
    HTTPClient http;

    // Your WiFi credentials
    const char* ssid = "your_SSID";
    const char* password = "your_PASSWORD";

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

        WiFi.disconnect();
}
