#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <Wire.h>
#include <ADC128D818.h>

const char *ssid = "Robothand";       // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "VeryRobotic"; // The password of the Wi-Fi network
const char *websocket_server = "http://192.168.4.1:80";

using namespace websockets;

WebsocketsClient client;

ADC128 adc(0x1D); // Add the adc at address 0x1D.

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    delay(10);

    WiFi.begin(ssid, password); // Connect to the network of the Hand-unit

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
        delay(250);
        Serial.print('.');
    }

    adc.start();

    ADC128_channels temp = {0, 0, 0, 0, 0, 1, 1, 1}; // Turn off the last three channels for better performance and current usage.
    adc.write_disabled_channels(temp);

    while (!client.connect(websocket_server)) // Keep trying to connect to the websocket server.
    {
        delay(100);
    }
}

void loop()
{
    int j = 0;
    unsigned long last = millis();
    while ((millis() - last) < 1000)
    {
        String data = "[";

        data += String(adc.read_ADC_channel(0));
        data += ",";
        data += String(adc.read_ADC_channel(1));
        data += ",";
        data += String(adc.read_ADC_channel(2));
        data += ",";
        data += String(adc.read_ADC_channel(3));
        data += ",";
        data += String(adc.read_ADC_channel(4));
        data += "]";

        client.send(data);

        j++;
    }

    Serial.print("Number of requests done in 1 second: ");
    Serial.println(j);
}
