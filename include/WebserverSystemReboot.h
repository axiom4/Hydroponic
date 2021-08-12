#include <ESPAsyncWebServer.h>

class WebserverSystemReboot : public AsyncWebHandler {
 public:
  WebserverSystemReboot(AsyncWebServer* server);
  ~WebserverSystemReboot();

  bool canHandle(AsyncWebServerRequest* request);

  // void handleRequest(AsyncWebServerRequest* request);

  static void handleWebserverSystemReboot(AsyncWebServerRequest* request);
};