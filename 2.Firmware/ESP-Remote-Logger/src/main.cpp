#include <Arduino.h>
#include <network.h>
#include <message.h>
#include <password.h>
#include <can.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <sd_card.h>

#include <sstream>
using namespace std;

Network wifi;
String ip;
WiFiServer server;
string tcpBuffer;
ClientHandler clientHandler;
bool tcpConnected = false;
WiFiClient client;
uint32_t lastTransmitTick = 0;
char wifiFile[] = "/wifi.txt";

CAN can;
int canMessageCount = 0;

TFT_eSPI tft = TFT_eSPI();

SdCard card;
SdCard tf;

HardwareSerial infoSerial(1);
hw_timer_t *timer = nullptr;

void screenUpdate();

void setup()
{
  // start debug serial port
  Serial.begin(460800);

  // init sd card
  bool cardState = card.init();
  if (cardState)
    card.listDir("/", 1);

  // init display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  if (!cardState)
  {
    tft.setCursor(5, 108);
    tft.println("SD card FAIL");
  }

  // start network
  String ssid = SSID_DEFAULT, username = USER_NAME_DEFAULT, password = PASSWORD_DEFAULT;
  bool networkType = 0;
  if (cardState)
  {
    networkType = tf.readFileLine(wifiFile, 1).equals("0"); // 1 for normal, 0 for username required
    ssid = tf.readFileLine(wifiFile, 2);
    if (networkType)
    {
      password = tf.readFileLine(wifiFile, 3);
    }
    else
    {
      username = tf.readFileLine(wifiFile, 3);
      password = tf.readFileLine(wifiFile, 4);
    }
  }

  // show network info
  tft.setCursor(5, 15);
  tft.println(F(" ESP Logger "));
  tft.setCursor(5, 55);
  tft.println(F("-Connect to-"));
  tft.setCursor(24, 80);
  tft.println(ssid);
  if (networkType)
    ip = wifi.init(ssid, password);
  else
    ip = wifi.init(ssid, username, password);
  server.begin(TCP_SERVER_PORT);

  // init CAN receive
  can.init(CAN_SPEED_1000KBPS, GPIO_NUM_32, GPIO_NUM_33);

  // init uart receive
  infoSerial.begin(460800, SERIAL_8E1, 22, 21);

  // show ip adddress
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 15);
  tft.println(F(" ESP Logger "));
  tft.setCursor(5, 55);
  tft.println(F("-Connected!-"));
  tft.setCursor(15, 80);
  tft.println(ip);
  delay(1000);

  // reset display
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);

  // start timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, screenUpdate, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  // register messages
  stringstream ss;
  ss << (char)UINT16 << "-"
     << "Encoder"
     << ",";
  ss << (char)UINT16 << "-"
     << "RPM"
     << ",";
  ss << (char)UINT16 << "-"
     << "Torque"
     << ",";
  ss << (char)UINT8 << "-"
     << "Temp";
  // Serial.println(ss.str().c_str());
  char name[10];
  for (int i = 1; i <= 8; i++)
  {
    sprintf(name, "Motor 0x20%d", i);
    tx.registerMsg(i, 0x04, 0x07, name, ss.str().c_str());
    infoSerial.print(tx.getMessageByID(i)->generateRegisterMsg().c_str());
  }
}

void screenUpdate()
{
  // update display
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 5);
  tft.printf("CAN Rate: %d", canMessageCount);
  tft.setCursor(5, 112);
  tft.println("-ESP Logger-      JK 2022");
  tft.setCursor(5, 120);
  tft.println("-IP:" + ip + "-");

  // update can Rate
  canMessageCount = 0;
}

void loop()
{
  // TCP Receive form client non-blocking
  if (tcpConnected)
  {
    while (client.available())
    {
      char c = client.read();
      if (c == '\n')
      {
        // Serial.printf("Received from TCP: %s\n", tcpBuffer.c_str());
        clientHandler.parseTCP(client, tcpBuffer);
        tcpBuffer = "";
        client.stop();
        tcpConnected = false;
        break;
      }
      else
        tcpBuffer += c;
    }
    tcpConnected = client.connected();
  }
  else
  {
    client = server.available();
    tcpConnected = client.connected();
  }

  // read motor data
  if (can.receive(1))
  {
    int canId = can.getRxID();
    if (canId >= 0x201 && canId <= 0x208)
    {
      canMessageCount++;
      uint8_t data[7];
      // big endian to little endian
      data[0] = can.getRxData()[1];
      data[1] = can.getRxData()[0];
      data[2] = can.getRxData()[3];
      data[3] = can.getRxData()[2];
      data[4] = can.getRxData()[5];
      data[5] = can.getRxData()[4];
      data[6] = can.getRxData()[6];
      tx.txLoadMessage(canId - 0x200, data);
    }
    // Serial.printf("%s is running at %d RPM \n", (tx.getMessageByID(canId-0x200)->name).c_str(), *(int16_t *)(tx.getMessageByID(canId-0x200)->getData("RPM")->value));
    tx.txTransmit((uint8_t)(canId - 0x200), infoSerial);
  }

  // send motor data
  // uint8_t dataCAN[CAN_MESSAGE_LENGTH] = {0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00};
  // can.setTxData(dataCAN);
  // can.setTxID(0x200);
  // can.transmit();

  // read uart data
  usartReceive(infoSerial);

  // send UDP test data, about 300Hz
  if (millis() - lastTransmitTick > 1)
  {
    tx.txTransmitViaNet(clientHandler);
    lastTransmitTick = millis();
  }
}
