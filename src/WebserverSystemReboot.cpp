#include <Arduino.h>
#include <WebserverSystemReboot.h>
#include <unistd.h>

WebserverSystemReboot::WebserverSystemReboot(AsyncWebServer* server) {
  server->on("/rest/reboot", HTTP_GET, handleWebserverSystemReboot);
}

WebserverSystemReboot::~WebserverSystemReboot() {}

bool WebserverSystemReboot::canHandle(AsyncWebServerRequest* request) {
  // request->addInterestingHeader("ANY");
  return true;
}

// void WebserverSystemReboot::handleRequest(AsyncWebServerRequest* request) {}

void WebserverSystemReboot::handleWebserverSystemReboot(
    AsyncWebServerRequest* request) {
  Serial.println("Reboot request");

  // if (!request->authenticate("admin", "password"))
  //   return request->requestAuthentication();

  String json = "{";
  json += "\t\"status\": 0,\n";
  json += "\t\"description\": \"Device reboot.\"\n";
  json += "}";
  request->send(200, "application/json", json);

  ESP.restart();
}
