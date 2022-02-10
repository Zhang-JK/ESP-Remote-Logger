#include <Arduino.h>
#include <network.h>
#include <password.h>
#include <can.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <sd_card.h>
#include <message.h>

#include <sstream>
using namespace std;

Network wifi;
String ip;
WiFiServer server;
CAN can;
TFT_eSPI tft = TFT_eSPI();
SdCard card;
SdCard tf;
HardwareSerial infoSerial(1);
hw_timer_t *timer = nullptr;
char wifiFile[] = "/wifi.txt";

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
  String ssid = SSID, username = USER_NAME, password = PASSWORD;
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
  server.begin(23333);

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
  delay(5000);

  // reset display
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);

  // start timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, screenUpdate, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);

  // register messages
  stringstream ss;
  ss << (char)UINT16 << "-" << "Encoder" << ",";
  ss << (char)UINT16 << "-" << "RPM" << ",";
  ss << (char)UINT16 << "-" << "Torque" << ",";
  ss << (char)UINT8  << "-" << "Temp";
  // Serial.println(ss.str().c_str());
  char name[10];
  for (int i=0; i<7; i++)
  {
    sprintf(name, "Motor 0x20%d", i);
    tx.registerMsg(i+1, 0x04, 0x07, name, ss.str().c_str());
    infoSerial.print(tx.getMessageByID(i+1)->generateRegisterMsg().c_str());
  }
}

void screenUpdate()
{
  // update display
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 112);
  tft.println("-ESP Logger-      JK 2022");
  tft.setCursor(5, 120);
  tft.println("-IP:" + ip + "-");
}

void loop()
{
  // find available clients
  WiFiClient client = server.available();
  if (client.connected())
    client.println("Data Test");

  // read motor data
  if (can.receive(1))
  {
    Serial.printf("Received from 0x%08X: ", can.getRxID());
    for (int i = 0; i < CAN_MESSAGE_LENGTH; i++)
    {
      Serial.print(can.getRxData()[i]);
      Serial.print(" ");
    }
    Serial.println("");
  }

  // send motor data
  // uint8_t dataCAN[CAN_MESSAGE_LENGTH] = {0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00};
  // can.setTxData(dataCAN);
  // can.setTxID(0x200);
  // can.transmit();

  // read uart data
  usartReceive(infoSerial);

  // if(rx.getMessageByID(0x01) != nullptr)
  // {
  //   Serial.print(rx.getMessageByID(0x01)->generateRegisterMsg().c_str());
  //   delay(100000);
  // }
}
