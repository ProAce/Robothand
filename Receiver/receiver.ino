#include <ESP8266WiFi.h>
#include <Servo.h>
#include <ArduinoWebsockets.h>

const char *ssid = "Robothand";       // The SSID (name) of the Wi-Fi network you want to create
const char *password = "VeryRobotic"; // The password of the Wi-Fi network

using namespace websockets;

WebsocketsServer server;

Servo thumb;
Servo index_finger;
Servo middle_finger;
Servo ring_finger;
Servo pinky;

void setup()
{
  // Start the serial port.
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  // Start an accespoint at channel 6 with a hidden ssid.
  WiFi.softAP(ssid, password, 6, true);

  // Attach all the servo's to the ESP8266 hardware.
  thumb.attach(16);
  index_finger.attach(4);
  middle_finger.attach(14);
  ring_finger.attach(12);
  pinky.attach(13);

  server.listen(80); // Listen to websocket requests at port 80.
}

void loop()
{
  auto client = server.accept();
  while (client.available())
  {
    auto msg = client.readBlocking();

    if (msg.isText())
    {
      unmarshal_message(msg.data());
    }
  }
}

void unmarshal_message(String json)
{
  Serial.println(json);

  uint8_t index = json.indexOf("[");
  uint8_t next_index = json.indexOf(",");

  thumb.write(adc_to_rotation(json.substring(index, next_index).toInt(), 180, 80));

  index = next_index;
  next_index = json.indexOf(",", index);

  index_finger.write(adc_to_rotation(json.substring(index, next_index).toInt(), 180, 60));

  index = next_index;
  next_index = json.indexOf(",", index);

  middle_finger.write(adc_to_rotation(json.substring(index, next_index).toInt(), 180, 60));

  index = next_index;
  next_index = json.indexOf(",", index);

  ring_finger.write(adc_to_rotation(json.substring(index, next_index).toInt(), 180, 60));

  index = next_index;
  next_index = json.indexOf("]", index);

  pinky.write(adc_to_rotation(json.substring(index, next_index).toInt(), 180, 60));
}

int adc_to_rotation(int value, int min, int max)
{
  return  value * (max - min) / 900 + min; // 900 is the maximum ADC value.
}
