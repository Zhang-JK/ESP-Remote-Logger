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
	const unsigned char* identity_c = reinterpret_cast<const unsigned char *>(identity.c_str());
	const unsigned char* password_c = reinterpret_cast<const unsigned char *>(password.c_str());

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
	
	return WiFi.localIP().toString();;
}
