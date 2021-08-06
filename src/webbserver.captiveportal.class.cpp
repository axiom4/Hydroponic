#include <hydroponic.h>

class CaptiveRequestHandler : public AsyncWebHandler {
 public:
  CaptiveRequestHandler() {
    // server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=31536000").setDefaultFile("index.html");
    // server.on("/rest/config", HTTP_GET, handleGetConfig);

    // server.on("/rest/config", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    //   AsyncWebServerResponse *response = request->beginResponse(204);

    //   // response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
    //   response->addHeader("Access-Control-Allow-Methods", "OPTIONS, GET, HEAD, POST");
    //   request->send(response);
    // });

    // server.addHandler(new AsyncCallbackJsonWebHandler("/rest/config", handleConfigSave));

    // server.onNotFound(handleNotFound);
  }
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) { request->send(SPIFFS, "/index.html", String(), false); }
};