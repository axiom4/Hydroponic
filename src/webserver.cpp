#include <hydroponic.h>

#include <AsyncJson.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/events");

struct hydroponicConfig* s_config;

void handleNotFound(AsyncWebServerRequest* request) {
  request->redirect("/");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient* client) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      const uint8_t size = JSON_OBJECT_SIZE(1);
      StaticJsonDocument<size> json;
      DeserializationError err = deserializeJson(json, data);
      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
        return;
      }

      const char* action = json["data"];

      if(action) {
        if(strcmp(action, "ping") == 0) {
          client->text("{ \"data\": \"pong\" }");
        }
      }
    }
}

void onWsEvent(AsyncWebSocket* server,
               AsyncWebSocketClient* client,
               AwsEventType type,
               void* arg,
               uint8_t* data,
               size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    client->text("{ \"connection\": \"Hello from ESP32 Server\" }");

  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  } else if (type == WS_EVT_DATA) {
    handleWebSocketMessage(arg, data, len, client);

    // for (int i = 0; i < len; i++) {
    //   Serial.print(data[i]);
    //   Serial.print("|");
    // }

    // Serial.println();
  }
}

void handleGetConfig(AsyncWebServerRequest* request) {
  struct hydroponicConfig* config;
  config = eepromReadData();

  String json = "{";
  json +=
      "\t\"device\": \"" + String(s_config->config_u.config.device) + "\",\n";
  json += "\t\"wifi_ssid\": \"" + String(s_config->config_u.config.wifi_ssid) +
          "\",\n";
  json += "\t\"wifi_password\": \"" +
          String(s_config->config_u.config.wifi_password) + "\",\n";
  json += "\t\"ntp_server\": \"" +
          String(s_config->config_u.config.ntp_server) + "\",\n";
  json += "\t\"hostname\": \"" + String(s_config->config_u.config.hostname) +
          "\",\n";
  json += "\t\"moisture_air_value\": " +
          String(s_config->config_u.config.AirValue) + ",\n";
  json += "\t\"moisture_water_value\": " +
          String(s_config->config_u.config.WaterValue) + "\n";
  json += "}";

  free(config);
  request->send(200, "application/json", json);
}

void handleConfigSave(AsyncWebServerRequest* request, JsonVariant& json) {
  StaticJsonDocument<200> data;

  if (json.is<JsonArray>()) {
    Serial.println("Json Array");
    data = json.as<JsonArray>();
  } else if (json.is<JsonObject>()) {
    Serial.println("Json Array");
    data = json.as<JsonObject>();
  }

  String hostname = data["hostname"];
  String wifi_ssid = data["wifi_ssid"];
  String wifi_password = data["wifi_password"];
  String ntp_server = data["ntp_server"];
  int moisture_air_value = data["moisture_air_value"];
  int moisture_water_value = data["moisture_water_value"];

  Serial.printf("hostname: %s\n", hostname.c_str());
  Serial.printf("wifi_ssid: %s\n", wifi_ssid.c_str());
  Serial.printf("wifi_password: %s\n", wifi_password.c_str());
  Serial.printf("ntp_server: %s\n", ntp_server.c_str());
  Serial.printf("moisture_air_value: %d\n", moisture_air_value);
  Serial.printf("moisture_water_value: %d\n", moisture_water_value);

  strcpy(s_config->config_u.config.wifi_ssid, wifi_ssid.c_str());
  strcpy(s_config->config_u.config.wifi_password, wifi_password.c_str());
  strcpy(s_config->config_u.config.ntp_server, ntp_server.c_str());
  strcpy(s_config->config_u.config.hostname, hostname.c_str());

  s_config->config_u.config.AirValue = moisture_air_value;
  s_config->config_u.config.WaterValue = moisture_water_value;

  eepromWriteData(s_config);

  String response;

  serializeJson(data, response);

  request->send(200, "application/json", response);
  Serial.println(response);
}

void handleSystemReboot(AsyncWebServerRequest* request) {
  Serial.println("Reboot request");

  // if (!request->authenticate("admin", "password"))
  //   return request->requestAuthentication();

  String json = "{";
  json += "\t\"status\": 0,\n";
  json += "\t\"description\": \"Device reboot.\"\n";
  json += "}";
  request->send(200, "application/json", json);

  toggle_reboot = true;
}

float mapfloat(float x,
               float in_min,
               float in_max,
               float out_min,
               float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void handleSystemBattery(AsyncWebServerRequest* request) {
  Serial.println("Battery request");
  float raw = 0.0;
  float volt = 0.0;

  pinMode(A0, INPUT);
  raw = analogRead(A0);
  volt = raw / 1023;
  volt = 4.2 * volt;

  Serial.println(volt);
  Serial.println(raw);
  // int analogInPin  = A0;    // Analog input pin

  // float sensorValue;          // Analog Output of Sensor
  float calibration =
      0.36;  // Check Battery voltage using multimeter & add/subtract the value
  int bat_percentage;

  // sensorValue = analogRead(analogInPin);
  volt = (((raw * 3.3) / 1024) * 2 +
          calibration);  // multiply by two as voltage divider network is 100K &
                         // 100K Resistor

  // Serial.println(voltage);

  bat_percentage = mapfloat(
      volt, 2.8, 4.2, 0,
      100);  // 2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage

  if (bat_percentage >= 100) {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0) {
    bat_percentage = 1;
  }

  String json = "{";
  json += "\t\"status\": " + String(bat_percentage) + ",\n";
  json += "\t\"voltage\": " + String(volt) + ",\n";
  json += "\t\"sensor_value\": " + String(raw) + ",\n";
  json += "\t\"description\": \"Device battery status\"\n";
  json += "}";
  request->send(200, "application/json", json);
}

void handleGetRelayStatus(AsyncWebServerRequest* request) {
  Serial.println("Relay status request");

  // if (!request->authenticate("admin", "password"))
  //   return request->requestAuthentication();
  String status = "off";

  if (relay_status == LOW)
    status = "on";

  String json = "{";
  json += "\t\"status\": \"" + status + "\",\n";
  json += "\t\"description\": \"Relay status\"\n";
  json += "}";
  request->send(200, "application/json", json);
}

void handleWifiScan(AsyncWebServerRequest* request) {
  String json = "[";
  int n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(true);
  } else if (n) {
    for (int i = 0; i < n; ++i) {
      if (i)
        json += ",";
      json += "{";
      json += "\"rssi\":" + String(WiFi.RSSI(i));
      json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
      json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
      json += ",\"channel\":" + String(WiFi.channel(i));
      json += ",\"secure\":" + String(WiFi.encryptionType(i));
      // json += ",\"hidden\":" + String(WiFi.isHidden(i) ? "true" : "false");
      json += "}";
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }
  json += "]";
  request->send(200, "application/json", json);
  json = String();
}

void handleSetRelayStatus(AsyncWebServerRequest* request, JsonVariant& json) {
  StaticJsonDocument<200> data;

  if (json.is<JsonArray>()) {
    data = json.as<JsonArray>();
  } else if (json.is<JsonObject>()) {
    data = json.as<JsonObject>();
  }

  String status = data["status"];

  if (status == "on") {
    relay_status = LOW;
  } else {
    relay_status = HIGH;
  }
  digitalWrite(RELAY_PIN, relay_status);

  String json_out = "{";
  json_out += "\t\"status\": \"" + String(status) + "\",\n";
  json_out += "\t\"description\": \"Relay status\"\n";
  json_out += "}";
  request->send(200, "application/json", json_out);
}

void webserverSetup(struct hydroponicConfig* config) {
  s_config = config;

  /* Angular.io Web Pagees */
  server.serveStatic("/favicon.ico", HYDROPONICFS, "/favicon.ico");
  server.serveStatic("/", HYDROPONICFS, "/")
      .setCacheControl("max-age=31536000")
      .setDefaultFile("index.html");

  /*  Configuration Service */
  server.on("/rest/system/config", HTTP_GET, handleGetConfig);

  server.on("/rest/system/config", HTTP_OPTIONS,
            [](AsyncWebServerRequest* request) {
              AsyncWebServerResponse* response = request->beginResponse(204);

              // response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
              response->addHeader("Access-Control-Allow-Methods",
                                  "OPTIONS, GET, HEAD, POST");
              request->send(response);
            });

  server.addHandler(
      new AsyncCallbackJsonWebHandler("/rest/system/config", handleConfigSave));

  /* Reboot Service */
  server.on("/rest/system/reboot", HTTP_GET, handleSystemReboot);

  /* Battery Service */
  server.on("/rest/system/battery", HTTP_GET, handleSystemBattery);

  /* Relay Service */
  server.on("/rest/relay", HTTP_GET, handleGetRelayStatus);
  server.addHandler(
      new AsyncCallbackJsonWebHandler("/rest/relay", handleSetRelayStatus));

  /* Wi-Fi scan Service */
  server.on("/rest/wifi/scan", HTTP_GET, handleWifiScan);

  /* WebSocket Service */
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.onNotFound(handleNotFound);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.begin();  // Actually start the server
  Serial.println("HTTP server started");
}
