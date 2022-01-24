#include <Arduino.h>
#include <network.h>
#include <password.h>

Network wifi;
WiFiServer server;

void setup() {
  // start debug serial port
  Serial.begin(115200);

  // start network
  wifi.init(SSID, USER_NAME, PASSWORD);
  server.begin(23333);

}

void loop() {
  // find available clients
  WiFiClient client = server.available();
  if(client.connected())
    client.println("Data Test");
}
