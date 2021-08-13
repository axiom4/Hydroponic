#include <Arduino.h>
#include <EEPROM.h>
#include <Esp.h>
#include <ESPAsyncWebServer.h>

#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <FS.h>
#define HYDROPONICFS LittleFS
#include <LittleFS.h>

#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>

#include <SPIFFS.h>

#define HYDROPONICFS SPIFFS

#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif

#include <NTPClient.h>

#define CONFIG_SIZE sizeof(struct hydroponic_config)
#define SERIAL_SPEED 9600

#define DEVICE "lEdHkyxSH"

#define WIFI_DEFAULT_SSID "hydroponic"
#define WIFI_DEFAULT_PASSWD "1234567890"

#define DEFAULT_NTP_SERVER "europe.pool.ntp.org"

#define DEFAULT_HOSTAME "hydroponic"

#define DEFAULT_AIR_VALUE 3800
#define DEFAULT_WATER_VALUE 1470
#define SENSOR_CALIBRATE_STEPS 20

#define ADC1_CH4 32

#define RELAY_PIN 2

const int moisturePin = ADC1_CH4;
extern bool toggle_reboot;
extern int relay_status;

struct hydroponic_config {
  char device[20];
  char wifi_ssid[100];
  char wifi_password[100];
  char ntp_server[100];
  char hostname[100];
  int AirValue;
  int WaterValue;
} __attribute__((packed));

typedef struct hydroponicConfig {
  union {
    struct hydroponic_config config;
    unsigned char config_data[CONFIG_SIZE];
  } config_u;
} __attribute__((packed)) pkt;

void eepromSetup();
void eepromWriteData(struct hydroponicConfig* config);
struct hydroponicConfig* eepromReadData();

void setupWiFiAP();
bool setupWiFiClient(struct hydroponicConfig* config);

void ntpSetup(struct hydroponicConfig* config);
void ntpLoop();

void webserverSetup(struct hydroponicConfig* config);
void Webserver();

int sensorRead();
void calibrateAirValue();
void calibrateWaterValue();