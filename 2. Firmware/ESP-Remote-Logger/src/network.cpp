#include <network.h>

String Network::init(String ssid, String password)
{
	Serial.print("Connecting: ");
	Serial.println(ssid.c_str());

	WiFi.disconnect(true);
	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.begin(ssid.c_str(), password.c_str());

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	return WiFi.localIP().toString();
}

String Network::init(String ssid, String identity, String password)
{
	Serial.print("Connecting: ");
	Serial.print(ssid.c_str());
	Serial.print(" with ");
	Serial.println(identity.c_str());
	const unsigned char *identity_c = reinterpret_cast<const unsigned char *>(identity.c_str());
	const unsigned char *password_c = reinterpret_cast<const unsigned char *>(password.c_str());

	WiFi.disconnect(true);
	WiFi.mode(WIFI_MODE_APSTA);
	esp_wifi_sta_wpa2_ent_set_identity(identity_c, identity.length());
	esp_wifi_sta_wpa2_ent_set_username(identity_c, identity.length());
	esp_wifi_sta_wpa2_ent_set_password(password_c, password.length());
	esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
	esp_wifi_sta_wpa2_ent_enable(&config);
	WiFi.begin(ssid.c_str());

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	return WiFi.localIP().toString();
}

UDPClient::UDPClient(string ip, uint16_t port, string name, uint16_t selfPort) : ip(ip), port(port), name(name), selfPort(selfPort), lastBeatTick(millis())
{
	udp.begin(selfPort);
	Serial.printf("Established UDP connection on port %d to %s with IP: %s:%d", selfPort, name.c_str(), ip.c_str(), port);
}

bool UDPClient::isConnected()
{
	return true;
	return millis() - lastBeatTick < 3000;
}

bool UDPClient::sendUDP(uint8_t *message, int length)
{
	if (!isConnected())
		return false;

	udp.beginPacket(ip.c_str(), port);
	udp.write(message, length);
	udp.endPacket();

	return true;
}

void UDPClient::heartbeat()
{
	lastBeatTick = millis();
	Serial.printf("Heartbeat from %s", name.c_str());
}

ClientHandler::~ClientHandler()
{
	for (auto client : clients)
		delete client;
}

const UDPClient *ClientHandler::getClient(string name)
{
	for (auto client : clients)
	{
		if (client->getName() == name)
			return client;
	}

	return nullptr;
}

UDPClient *ClientHandler::getClient(string ip, uint16_t port)
{
	for (auto client : clients)
	{
		if (client->getIp() == ip && client->getPort() == port)
			return client;
	}

	return nullptr;
}

const list<UDPClient *> *ClientHandler::getAllClients()
{
	return &clients;
}

void ClientHandler::addClient(string ip, uint16_t port, string name, uint16_t selfPort)
{
	clients.push_back(new UDPClient(ip, port, name, selfPort));
}

void ClientHandler::sendAllUDP(uint8_t *message, int length)
{
	for (auto client : clients)
		client->sendUDP(message, length);
}

bool ClientHandler::parseTCP(IPAddress ip, string message)
{
	int ind = message.find_first_of(":");
	if(ind == -1)
		return false;
	switch (message[0])
	{
	case 'N':
		if(getClient(ip.toString().c_str(), UDP_PORT) != nullptr)
			return false;
		if(getClient(message.substr(ind, message.length()-ind-1)) != nullptr)
			return false;
		addClient(ip.toString().c_str(), UDP_PORT, message.substr(ind, message.length()-ind-1), udpPort++);
		break;
	case 'H':
		if(getClient(ip.toString().c_str(), UDP_PORT) == nullptr)
			return false;
		getClient(ip.toString().c_str(), UDP_PORT)->heartbeat();
		break;

	default:
		return false;
	}

	return true;
}
