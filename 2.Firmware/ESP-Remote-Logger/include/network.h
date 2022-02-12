#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <esp_wpa2.h>
#include <HTTPClient.h>
#include <iostream>
#include <list>
// #include <message.h>
using namespace std;
 
class Network
{
private:
	 
public:
	String init(String ssid, String password);
	String init(String ssid, String identity, String password);
};

class UDPClient
{
   private:
	string ip;
	uint16_t port;
	string name;
	uint16_t selfPort;
	uint32_t lastBeatTick; 
	WiFiUDP udp;

   public:
	UDPClient(string ip, uint16_t port, string name, uint16_t selfPort);

	string getIp() { return ip; }
	uint16_t getPort() { return port; }
	string getName() { return name; }

	bool isConnected();
	bool sendUDP(uint8_t *message, int length);
	void heartbeat();
};

#define UDP_PORT 8888
#define TCP_SERVER_PORT 23333

class ClientHandler
{
   private:
	list<UDPClient *> clients;
	uint16_t udpPort = 8081;
	UDPClient *getClient(string ip, uint16_t port);

   public:
    ~ClientHandler();
	const UDPClient *getClient(string name);
	const list<UDPClient *> *getAllClients();
	void addClient(string ip, uint16_t port, string name, uint16_t selfPort);

	void sendAllUDP(uint8_t *message, int length);
	bool parseTCP(WiFiClient& client, string message);
};

#endif