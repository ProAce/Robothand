#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // Include the Arduino JSON parser library.

const char *ssid = "Robothand";       // The SSID (name) of the Wi-Fi network you want to create
const char *password = "VeryRobotic"; // The password of the Wi-Fi network

ESP8266WebServer server(80);

int fingers_positions[5];

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
}

void loop()
{
    server.handleClient();
}

void handle_post()
{
    String json = server.arg("plain");

    Serial.println(json);

    DynamicJsonDocument doc(104);

    deserializeJson(doc, json);

    JsonArray data = doc["fingers"];
    fingers_positions[0] = data[0]; // 1
    fingers_positions[1] = data[1]; // 2
    fingers_positions[2] = data[2]; // 3
    fingers_positions[3] = data[3]; // 4
    fingers_positions[4] = data[4]; // 5

    server.send(202);
}