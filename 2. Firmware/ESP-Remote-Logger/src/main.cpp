#include <Arduino.h>
#include <network.h>
#include <password.h>
#include <can.h>

Network wifi;
WiFiServer server;
CAN can;

void setup() {
  // start debug serial port
  Serial.begin(115200);

  // start network
  wifi.init(SSID, USER_NAME, PASSWORD);
  server.begin(23333);

  // init CAN receive
  can.init(CAN_SPEED_1000KBPS, GPIO_NUM_32, GPIO_NUM_33);
}

void loop() {
  // find available clients
  WiFiClient client = server.available();
  if(client.connected())
    client.println("Data Test");

  // read motor data
  if (can.receive(1)) {
    Serial.printf("Received from 0x%08X: ", can.getRxID());
    for (int i = 0; i < CAN_MESSAGE_LENGTH; i++) {
      Serial.print(can.getRxData()[i]);
      Serial.print(" ");
    }
    Serial.println("");
  }

  // send motor data
  uint8_t data[CAN_MESSAGE_LENGTH] = {0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00};
  can.setTxData(data);
  can.setTxID(0x200);
  can.transmit();

  delay(50);
}
