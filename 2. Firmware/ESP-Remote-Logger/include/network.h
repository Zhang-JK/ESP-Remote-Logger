#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <esp_wpa2.h>
#include <HTTPClient.h>
 
class Network
{
private:
	 
public:
	String init(String ssid, String password);
	String init(String ssid, String identity, String password);
};

#endif