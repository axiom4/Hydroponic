#include <hydroponic.h>

void setupWiFiAP() {
  Serial.printf("Setup WiFi AP: %s (%s)\n", WIFI_DEFAULT_SSID, WIFI_DEFAULT_PASSWD);

  WiFi.setHostname(DEFAULT_HOSTAME);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_DEFAULT_SSID, WIFI_DEFAULT_PASSWD);
}

bool setupWiFiClient(struct hydroponicConfig *config) {
  bool state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(config->config_u.config.hostname);

  WiFi.begin(config->config_u.config.wifi_ssid, config->config_u.config.wifi_password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(config->config_u.config.wifi_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection failed.");
  }
  return state;
}