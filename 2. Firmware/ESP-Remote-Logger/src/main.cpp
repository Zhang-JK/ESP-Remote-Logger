#include <Arduino.h>
#include <network.h>
#include <password.h>
#include <can.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <sd_card.h>

Network wifi;
String ip;
WiFiServer server;
CAN can;
TFT_eSPI tft = TFT_eSPI();
SdCard card;
SdCard tf;
char wifiFile[] = "/wifi.txt";

void setup()
{
  // start debug serial port
  Serial.begin(115200);

  // init sd card
  bool cardState = card.init();
  if(cardState) card.listDir("/", 1);

  // init display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  if(!cardState) {
    tft.setCursor(5, 108);
    tft.println("SD card FAIL");
  }
  // start network
  String ssid = SSID, username = USER_NAME, password = PASSWORD;
  bool networkType = 0;
  if (cardState) {
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

  // show ip adddress
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 15);
  tft.println(F(" ESP Logger "));
  tft.setCursor(5, 55);
  tft.println(F("-Connected!-"));
  tft.setCursor(15, 80);
  tft.println(ip);
  delay(10000);

  // reset display
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
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
  uint8_t data[CAN_MESSAGE_LENGTH] = {0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00};
  can.setTxData(data);
  can.setTxID(0x200);
  can.transmit();

  // display test
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 112);
  tft.println("-ESP Logger-      JK 2022");
  tft.setCursor(5, 120);
  tft.println("-IP:" + ip + "-");

  delay(500);
}
