#include <hydroponic.h>
#include <DNSServer.h>

int wifi_ap = 0;
DNSServer dnsServer;

bool toggle_reboot = false;
int relay_status = HIGH;

byte relON[] = {0xA0, 0x01, 0x01,
                0xA2};  // Hex command to send to serial for open relay
byte relOFF[] = {0xA0, 0x01, 0x00,
                 0xA1};  // Hex command to send to serial for close relay

void Off() {
  Serial.write(relOFF, sizeof(relOFF));
};
void On() {
  Serial.write(relON, sizeof(relON));
};

void setupMDNS(const char* hostname) {
  if (MDNS.begin(hostname)) {  // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
}

void startMDNS(const char* hostname) {
#if defined(ESP8266)

  if (!MDNS.begin(hostname, WiFi.softAPIP())) {
    Serial.println("[ERROR] MDNS responder did not setup");
    while (1) {
      delay(1000);
    }
  } else {
    Serial.println("[INFO] MDNS setup is successful!");
  }
#elif defined(ESP32)

  // initialize mDNS service
  esp_err_t err = mdns_init();
  if (err) {
    printf("MDNS Init failed: %d\n", err);
    return;
  }

  // set hostname
  mdns_hostname_set(hostname);
  // set default instance
  mdns_instance_name_set(hostname);
#endif
}

void setup() {
  eepromSetup();
  HYDROPONICFS.begin();
  Serial.begin(SERIAL_SPEED);
  Serial.println("Init Hydroponic");
  struct hydroponicConfig* config;

  config = eepromReadData();

  if (!config) {
    ESP.restart();
  }

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  if (strcmp(config->config_u.config.device, DEVICE)) {
    strcpy(config->config_u.config.device, DEVICE);
    strcpy(config->config_u.config.wifi_ssid, WIFI_DEFAULT_SSID);
    strcpy(config->config_u.config.wifi_password, WIFI_DEFAULT_PASSWD);
    strcpy(config->config_u.config.ntp_server, DEFAULT_NTP_SERVER);
    strcpy(config->config_u.config.hostname, DEFAULT_HOSTAME);
    config->config_u.config.AirValue = DEFAULT_AIR_VALUE;
    config->config_u.config.WaterValue = DEFAULT_WATER_VALUE;

    eepromWriteData(config);
  }

  // strcpy(config->config_u.config.wifi_ssid, "silenthill-2g");
  // strcpy(config->config_u.config.wifi_password, "gibsonlespaul");

  Serial.printf("ssid: %s\npassword: %s\n", config->config_u.config.wifi_ssid,
                config->config_u.config.wifi_password);

  if (!strcmp(config->config_u.config.wifi_ssid, WIFI_DEFAULT_SSID)) {
    setupWiFiAP();
  } else {
    if (!setupWiFiClient(config)) {
      setupWiFiAP();
      wifi_ap = 1;
      dnsServer.start(53, "*", WiFi.softAPIP());

      startMDNS(config->config_u.config.hostname);
    } else {
      setupMDNS(config->config_u.config.hostname);
      ntpSetup(config);
    }
  }

  webserverSetup(config);
}

void loop() {
  if (wifi_ap)
    dnsServer.processNextRequest();
    // struct hydroponicConfig *config;
    // config = eepromReadData();

#if defined(ESP8266)
  MDNS.update();
#endif

  if (toggle_reboot) {
    delay(1000);
    ESP.restart();
  }

  ws.textAll(
    "{\"command\":\"result\",\"resultof\":\"userlist\",\"result\": false}"
  );

  delay(5000);
  // Serial.println("Status on");
  // int soilMoistureValue = 20000;
  // int soilmoisturepercent = 100;

  // soilMoistureValue = sensorRead();
  // Serial.println(soilMoistureValue);

  // soilmoisturepercent = map(soilMoistureValue,
  // config->config_u.config.AirValue, config->config_u.config.WaterValue, 0,
  // 100);

  // if (soilmoisturepercent > 100) {
  //   soilmoisturepercent = 100;
  // } else if (soilmoisturepercent < 0) {
  //   soilmoisturepercent = 0;
  // }

  // if (soilmoisturepercent < 50) {
  //   digitalWrite(led_gpio, LOW);
  // } else {
  //   digitalWrite(led_gpio, HIGH);
  // }

  // Serial.println(soilmoisturepercent);
  // delay(500);
}