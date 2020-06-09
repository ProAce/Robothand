#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // Include the Arduino JSON parser library.
#include <Wire.h>

const char *ssid = "Robothand";       // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "VeryRobotic"; // The password of the Wi-Fi network
const char *serverName = "http://192.168.4.1/fingers";

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    delay(10);

    WiFi.begin(ssid, password); // Connect to the network
    Serial.print("Connecting to ");
    Serial.println(ssid);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
        delay(250);
        Serial.print('.');
    }
    Serial.println();

    Serial.println();
    Serial.println(WiFi.localIP());
}

void loop()
{
    int j = 0;
    unsigned long last = millis();
    while ((millis() - last) < 1000)
    {
        Wire.requestFrom(0x18, 10);

        uint8_t readings[10];
        uint8_t i = 0;

        while (Wire.available()) // slave may send less than requested
        {
            readings[i++] = Wire.read();
        }

        HTTPClient http;

        http.begin(serverName);
        http.addHeader("content-type", "text/plain");

        DynamicJsonDocument doc(100);

        JsonArray data = doc.createNestedArray("f"); // Create a JSON object.
        data.add((uint16_t)(readings[1] << 8) + readings[0]);        // Add all the measurements.
        data.add((uint16_t)(readings[3] << 8) + readings[2]);
        data.add((uint16_t)(readings[5] << 8) + readings[4]);
        data.add((uint16_t)(readings[7] << 8) + readings[6]);
        data.add((uint16_t)(readings[9] << 8) + readings[8]);

        String JSON;
        serializeJson(data, JSON); // Serialize the JSON object into a String.
        // Serial.println(JSON);

        int response = http.POST(JSON); // Post the HTTP message.

        http.end(); // End the HTTP client.

        j++;
    }

    Serial.print("Number of requests done in 1 second: ");
    Serial.println(j);
}
