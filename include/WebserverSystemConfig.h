#include <hydroponic.h>

class WebserverSystemConfig : public AsyncWebHandler {
 public:
  WebserverSystemConfig();
  virtual ~WebserverSystemConfig();

  bool canHandle(AsyncWebServerRequest* request);

  void handleRequest(AsyncWebServerRequest* request);
};