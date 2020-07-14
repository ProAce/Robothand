#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoWebsockets.h>
#include <FS.h>
#include <Servo.h>

char ssid[] = "IoT"; // your network SSID (name)

using namespace websockets;

WebsocketsServer wsServer;
WebsocketsClient client;
ESP8266WebServer server(80);

Servo thumb;
Servo index_finger;
Servo middle_finger;
Servo ring_finger;
Servo pinky;

int finger_positions[5];

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(250);
  }

  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

  // start the Websocket server on port 8080.
  wsServer.listen(8080);

  SPIFFS.begin(); // Start the SPI Flash Files System.

  server.onNotFound([]() {                              // If the client requests any URI.
    if (!handle_file_read(server.uri()))                // send it if it exists.
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error.
  });

  // Start the server on port 80.
  server.begin();

  thumb.attach(16);
  index_finger.attach(4);
  middle_finger.attach(14);
  ring_finger.attach(12);
  pinky.attach(13);
}

void loop()
{
  server.handleClient();

  if (wsServer.poll()) // Check if there is a new client trying to connect.
  {
    Serial.println("New client");
    client = wsServer.accept();

    // Setup Callbacks for the new client.
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
  }

  if (client.available()) // If there is a client check if it has new messages.
  {
    client.poll();
  }
}

// Functions to handle websocket messages and events

void onEventsCallback(WebsocketsEvent event, String data)
{
  switch (event)
  {
  case WebsocketsEvent::ConnectionOpened:
    Serial.println("Connnection Opened");
    break;

  case WebsocketsEvent::ConnectionClosed:
    Serial.println("Connnection Closed");
    client.close();
    break;
  }
}

void onMessageCallback(WebsocketsMessage message)
{
  Serial.print("Got Message: ");
  Serial.println(message.data());
  String data = message.data();

  switch (data[0]) // Parse the incoming message.
  {
  case 'f':
    unmarshall_finger_positions(data.substring(2));
    update_finger_positions();
    break;

  case 'i':
    data = marshall_finger_positions();
    client.send(data);
    break;
  }
}

// Functions to handle the webserver file serving

String get_content_type(String filename)
{ // Convert the file extension to the correct MIME type.
  if (filename.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filename.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  return "text/plain";
}

bool handle_file_read(String path)
{ // Send the correct file to the client (if it exists).
  if (path.endsWith("/"))
  {
    path += "index.html"; // If a folder is requested, send the index file.
  }

  String contentType = get_content_type(path); // Get the MIME type.

  if (SPIFFS.exists(path))
  {                                                     // If the file exists.
    File file = SPIFFS.open(path, "r");                 // Open it.
    size_t sent = server.streamFile(file, contentType); // And send it to the client.
    file.close();                                       // Then close the file again.
    return true;
  }

  return false; // If the file doesn't exist, return false.
}

void unmarshall_finger_positions(String message)
{
  uint8_t index = 0;
  uint8_t next_index = message.indexOf(",");

  finger_positions[0] = message.substring(index, next_index).toInt();

  index = next_index + 1;
  next_index = message.indexOf(",", index);

  finger_positions[1] = message.substring(index, next_index).toInt();

  index = next_index + 1;
  next_index = message.indexOf(",", index);

  finger_positions[2] = message.substring(index, next_index).toInt();

  index = next_index + 1;
  next_index = message.indexOf(",", index);

  finger_positions[3] = message.substring(index, next_index).toInt();

  finger_positions[4] = message.substring(next_index + 1).toInt();
}

String marshall_finger_positions()
{
  String data = "f,";

  data += finger_positions[0];
  data += ",";
  data += finger_positions[1];
  data += ",";
  data += finger_positions[2];
  data += ",";
  data += finger_positions[3];
  data += ",";
  data += finger_positions[4];

  return data;
}

void update_finger_positions(void)
{
  thumb.write(adc_to_rotation(finger_positions[0], 80, 180));
  index_finger.write(adc_to_rotation(finger_positions[1], 60, 180));
  middle_finger.write(adc_to_rotation(finger_positions[2], 60, 180));
  ring_finger.write(adc_to_rotation(finger_positions[3], 60, 180));
  pinky.write(adc_to_rotation(finger_positions[4], 60, 180));
}

int adc_to_rotation(int value, int min, int max)
{
  return value * (max - min) / 4095.00 + min; // 4095 is the maximum ADC value.
}