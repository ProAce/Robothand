#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <Wire.h>
#include <ADC128D818.h>

const char *ssid = "IoT"; // The SSID (name) of the Wi-Fi network you want to connect to
const char *websockets_server = "ws://192.168.2.22:8080";

using namespace websockets;

WebsocketsClient client;

ADC128 adc(0x1D);

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid); // Connect to the network
    Serial.print("Connecting to ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
        delay(250);
    }

    Serial.println();
    Serial.println(WiFi.localIP());

    Wire.begin();
    adc.reset();

    ADC128_advanced_configuration advanced_configuration;
    advanced_configuration.selected_mode = 1;

    adc.write_advanced_configuration(advanced_configuration);

    ADC128_channels disabled_channels; // A channel which is true is disabled
    disabled_channels.channel_0 = true;
    disabled_channels.channel_1 = true;
    disabled_channels.channel_2 = true;

    adc.write_disabled_channels(disabled_channels);

    adc.set_conversion_rate(CONTINUOUS);
    adc.start();

    // Connect to server
    client.connect(websockets_server);
}

void loop()
{
    String data = marshall_message();

    client.send(data);
}

String marshall_message()
{
    String data = "f,";

    data += adc.read_ADC_channel(7);
    data += ",";
    data += adc.read_ADC_channel(6);
    data += ",";
    data += adc.read_ADC_channel(5);
    data += ",";
    data += adc.read_ADC_channel(4);
    data += ",";
    data += adc.read_ADC_channel(3);

    return data;
}
