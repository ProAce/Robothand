#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // Include the Arduino JSON parser library.
#include <Servo.h>

const char *ssid = "Robothand";       // The SSID (name) of the Wi-Fi network you want to create
const char *password = "VeryRobotic"; // The password of the Wi-Fi network

ESP8266WebServer server(80);

int fingers_positions[5];

Servo finger_1;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.softAP(ssid, password, 6, true); // Start an accespoint at channel 6 with a hidden ssid.
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP()); // Send the IP address of the ESP8266 to the computer

  server.onNotFound([]() {
    server.send(404, "text/plain", "404: Not Found"); // respond with a 404 (Not Found) error if the requested URI doesn't exist.
  });

  server.on("/fingers", HTTP_POST, handle_post);

  server.begin();

  finger_1.attach(14);
}

void loop()
{
  server.handleClient();

  uint16_t temp = map(fingers_positions[0], 0, 900, 180, 80);
  finger_1.write(temp);
}

void handle_post()
{
  String json = server.arg("plain");
  Serial.println(json);

  uint8_t comma_index = json.indexOf(",");

  fingers_positions[0] = json.substring(1, comma_index).toInt(); // 1
  Serial.println(fingers_positions[0]);
  // fingers_positions[1] = data[1]; // 2
  // fingers_positions[2] = data[2]; // 3
  // fingers_positions[3] = data[3]; // 4
  // fingers_positions[4] = data[4]; // 5

  server.send(202);
}
