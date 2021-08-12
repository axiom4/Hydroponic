#include <WebserverSystemConfig.h>

WebserverSystemConfig::WebserverSystemConfig() {
  // server.serveStatic("/", HYDROPONICFS,
  // "/").setCacheControl("max-age=31536000").setDefaultFile("index.html");
  // server.on("/rest/config", HTTP_GET, handleGetConfig);

  // server.on("/rest/config", HTTP_OPTIONS, [](AsyncWebServerRequest *request)
  // {
  //   AsyncWebServerResponse *response = request->beginResponse(204);

  //   // response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
  //   response->addHeader("Access-Control-Allow-Methods", "OPTIONS, GET, HEAD,
  //   POST"); request->send(response);
  // });

  // server.addHandler(new AsyncCallbackJsonWebHandler("/rest/config",
  // handleConfigSave));

  // server.onNotFound(handleNotFound);
}

WebserverSystemConfig::~WebserverSystemConfig() {}

bool WebserverSystemConfig::canHandle(AsyncWebServerRequest* request) {
  // request->addInterestingHeader("ANY");
  return true;
}

void WebserverSystemConfig::handleRequest(AsyncWebServerRequest* request) {
  //   request->send(HYDROPONICFS, "/index.html", String(), false);
}
