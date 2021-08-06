#include <hydroponic.h>

#include <AsyncJson.h>
#include <ArduinoJson.h>

static AsyncWebServer server(80);

struct hydroponicConfig *s_config;

String getPage(String body) {
  String title = "Hydroponic Configuration Page";
  String HTML_PAGE = "<!doctype html><html lang =\"en\">";
  HTML_PAGE += "<head>";
  HTML_PAGE += "<meta charset=\"utf-8\">";

  HTML_PAGE += "<title>";
  HTML_PAGE += title;
  HTML_PAGE += "</title> ";

  HTML_PAGE += "<meta name =\"description\" content=\"" + title + "\">";

  HTML_PAGE += "<style>";
  HTML_PAGE += "body{color:#333;font-family:helvetica,arial,sans-serif;line-height:1.5;margin:0 auto;max-width:40em;padding:0 1em;}";
  HTML_PAGE += ".container{font: 95% Arial, Helvetica, sans-serif;	max-width: 400px; margin: 10px auto;padding: 16px;background: #F7F7F7;}";
  HTML_PAGE += ".container h1{background: #43D1AF;padding: 20px 0;font-size: 140%;font-weight: 300;text-align: center;color: #fff;margin: -16px -16px 16px -16px;}";
  HTML_PAGE +=
      ".container input[type=\"text\"],.container input[type=\"date\"],.container input[type=\"datetime\"],.container input[type=\"email\"],.container "
      "input[type=\"number\"],.container[input[type=\"search\"],.container input[type=\"time\"],.container input[type=\"url\"],.container textarea,.container select "
      "{-webkit-transition: all 0.30s ease-in-out;-moz-transition: all 0.30s ease-in-out;-ms-transition: all 0.30s ease-in-out;-o-transition: all 0.30s ease-in-out;outline: "
      "none;box-sizing: border-box;-webkit-box-sizing: border-box;-coz-box-sizing: border-box;width: 100%;background: #fff;margin-bottom: 4%;border: 1px solid #ccc;padding: "
      "3%;color: "
      "#555;font: 95% Arial, Helvetica, sans-serif;}";
  HTML_PAGE +=
      ".container input[type=\"text\"]:focus,.container input[type=\"date\"]:focus,.container input[type=\"datetime\"]:focus,.container input[type=\"email\"]:focus,.container "
      "input[type=\"number\"]:focus,.container input[type=\"search\"]:focus,.container input[type=\"time\"]:focus,.container input[type=\"url\"]:focus,.container "
      "textarea:focus,.container select:focus {box-shadow: 0 0 5px #43D1AF;	padding: 3%;border: 1px solid #43D1AF;}";

  HTML_PAGE +=
      ".container input[type=\"submit\"],.container input[type=\"button\"]{box-sizing: border-box;-webkit-box-sizing: border-box;-moz-box-sizing: border-box;width: 100%;padding: "
      "3%;background: #43D1AF;border-bottom: 2px solid #30C29E;border-top-style: none;border-right-style: none;border-left-style: none;color: #fff;}";

  HTML_PAGE += "label {margin-top: 10px}";

  HTML_PAGE += ".container input[type=\"submit\"]:hover,.container input[type=\"button\"]:hover{background: #2EBC99;}";

  HTML_PAGE += "</style>";

  HTML_PAGE += "</head>";

  HTML_PAGE += " <body>";
  HTML_PAGE += body;

  HTML_PAGE += "</body>";
  HTML_PAGE += "</html>";

  return HTML_PAGE;
}

void handleRoot(AsyncWebServerRequest *request) {  // When URI / is requested, send a web page with a button to toggle the LED
  String form = "";

  form += "<div class=\"container\">";
  form += "<h1>Hydroponic Configuration Page</h1>";

  form += "<form action=\"/setup\" method=\"POST\">";

  form += "<label for=\"hostname\">Hostname:</label><br/><input type=\"text\" id=\"hostname\" name=\"hostname\" placeholder=\"Hostname\" value=\"";
  form += String(s_config->config_u.config.hostname);
  form += "\">";

  form += "</br>";

  form += "<label for=\"ssid\">Wi-Fi SSID:</label><br/><input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"Wi-Fi SSID\" value=\"";
  form += String(s_config->config_u.config.wifi_ssid);
  form += "\">";

  form += "</br>";

  form += "<label for=\"password\">Wi-Fi Password:</label><br/><input type=\"text\" id=\"password\" name=\"password\" placeholder=\"Wi-Fi Password\" value=\"";
  form += String(s_config->config_u.config.wifi_password);
  form += "\">";

  form += "</br>";

  form += "<label for=\"ntpserver\">NTP Sever:</label><br/><input type=\"text\" id=\"ntpserver\" name=\"ntpserver\" placeholder=\"Ntp Server\" value=\"";
  form += String(s_config->config_u.config.ntp_server);
  form += "\">";

  form += "</br>";

  form += "<input type=\"submit\" value=\"Setup\">";
  form += "</form>";
  form += "<p>Configure data.</p>";

  form +=
      "<label for=\"moisture_air_value\">Moisture Sensor Air Value:</label><br/><input disabled type=\"text\" id=\"moisture_air_value\" name=\"moisture_air_value\" "
      "placeholder=\"Moisture "
      "Sensor Air Value\" "
      "value=\"";
  form += String(s_config->config_u.config.AirValue);
  form += "\">";

  form += "</br>";

  form +=
      "<label for=\"moisture_water_value\">Moisture Sensor Water Value:</label><br/><input disabled type=\"text\" id=\"moisture_water_value\" name=\"moisture_water_value\" "
      "placeholder=\"Moisture "
      "Sensor Water Value\" "
      "value=\"";
  form += String(s_config->config_u.config.WaterValue);
  form += "\">";

  form += "</br>";

  form += "<p><a href=\"/calibrate\">Calibrate Moinsture Sensor</a>.</p>";
  form += "</div>";

  request->send(200, "text/html", getPage(form));
}

void handleSetup(AsyncWebServerRequest *request) {
  if (!request->hasArg("hostname") || request->arg("hostname") == NULL || !request->hasArg("ssid") || request->arg("ssid") == NULL || !request->hasArg("password") ||
      request->arg("password") == NULL || !request->hasArg("ntpserver") || request->arg("ntpserver") == NULL) {
    request->send(400, "text/plain", "400: Invalid Request");
    return;
  }

  struct hydroponicConfig *config;
  config = eepromReadData();

  strcpy(config->config_u.config.wifi_ssid, request->arg("ssid").c_str());
  strcpy(config->config_u.config.wifi_password, request->arg("password").c_str());
  strcpy(config->config_u.config.ntp_server, request->arg("ntpserver").c_str());
  strcpy(config->config_u.config.hostname, request->arg("hostname").c_str());

  eepromWriteData(config);

  free(config);
  String body = "<h1>Configuration for device \"<b>" + request->arg("hostname") + "</b>\" has been saved.</h1><p>restart device</p>";
  request->send(200, "text/html", getPage(body));

  delay(5000);

  ESP.restart();
}

void handleNotFound(AsyncWebServerRequest *request) { request->redirect("/"); }

void handleCalibrate(AsyncWebServerRequest *request) {
  String body = "<h1>Step1: Calibrate Air Value</h1>";
  body += "<p>Please clean the humidity sensor and keep outdoors.</p>";
  body += "<p>Go to <a href=\"/calibrate_air\">next step</a>.</p>";
  request->send(200, "text/html", getPage(body));
}

void handleCalibrateAir(AsyncWebServerRequest *request) {
  calibrateAirValue();

  String body = "<h1>Step2: Calibrate Water Value</h1>";
  body += "<p>Please insert humidity sensor into a cup of water.</p>";
  body += "<p>Go to <a href=\"/calibrate_water\">next step</a>.</p>";
  request->send(200, "text/html", getPage(body));
}

void handleCalibrateWater(AsyncWebServerRequest *request) {
  calibrateWaterValue();

  String body = "<h1>Congratulations</h1>";
  body += "<p>Moisture sensor calibration has been completed.</p>";
  body += "<p>Return to <a href=\"/\">main configuration page</a>.</p>";
  request->send(200, "text/html", getPage(body));
}

void handleGetConfig(AsyncWebServerRequest *request) {
  struct hydroponicConfig *config;
  config = eepromReadData();

  String json = "{";
  json += "\t\"device\": \"" + String(s_config->config_u.config.device) + "\",\n";
  json += "\t\"wifi_ssid\": \"" + String(s_config->config_u.config.wifi_ssid) + "\",\n";
  json += "\t\"wifi_password\": \"" + String(s_config->config_u.config.wifi_password) + "\",\n";
  json += "\t\"ntp_server\": \"" + String(s_config->config_u.config.ntp_server) + "\",\n";
  json += "\t\"hostname\": \"" + String(s_config->config_u.config.hostname) + "\",\n";
  json += "\t\"moisture_air_value\": " + String(s_config->config_u.config.AirValue) + ",\n";
  json += "\t\"moisture_water_value\": " + String(s_config->config_u.config.WaterValue) + "\n";
  json += "}";

  free(config);
  request->send(200, "application/json", json);
}

void handleConfigSave(AsyncWebServerRequest *request, JsonVariant &json) {
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

// class CaptiveRequestHandler : public AsyncWebHandler {
//  public:
//   CaptiveRequestHandler() {
//     server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=31536000").setDefaultFile("index.html");
//     server.on("/rest/config", HTTP_GET, handleGetConfig);

//     server.on("/rest/config", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
//       AsyncWebServerResponse *response = request->beginResponse(204);

//       // response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
//       response->addHeader("Access-Control-Allow-Methods", "OPTIONS, GET, HEAD, POST");
//       request->send(response);
//     });

//     server.addHandler(new AsyncCallbackJsonWebHandler("/rest/config", handleConfigSave));

//     // server.onNotFound(handleNotFound);
//   }
//   virtual ~CaptiveRequestHandler() {}

//   bool canHandle(AsyncWebServerRequest *request) {
//     // request->addInterestingHeader("ANY");
//     return true;
//   }

//   void handleRequest(AsyncWebServerRequest *request) { request->send(SPIFFS, "/index.html", String(), false); }
// };
void handleSystemReboot(AsyncWebServerRequest *request) { 
  Serial.println("Reboot request");

  if(!request->authenticate("admin", "password"))
        return request->requestAuthentication();

  String json = "{";
  json += "\t\"status\": 0,\n";
  json += "\t\"description\": \"Device reboot.\"\n";
  json += "}";
  request->send(200, "application/json", json);

  delay(5000);
  ESP.restart(); 
}

void webserverSetup(struct hydroponicConfig *config) {
  s_config = config;
  //   server.on("/", HTTP_GET, [](struct idroponicConfig *config) { handleSetup(config); });  // Call the 'handleRoot' function when a client requests URI "/"

  server.on("/old/settings", handleRoot);
  server.on("/setup", HTTP_POST, handleSetup);
  server.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico");
  server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=31536000").setDefaultFile("index.html");

  server.on("/rest/system/config", HTTP_GET, handleGetConfig);

  server.on("/rest/system/config", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(204);

    // response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
    response->addHeader("Access-Control-Allow-Methods", "OPTIONS, GET, HEAD, POST");
    request->send(response);
  });

  server.on("/rest/system/reboot", HTTP_GET, handleSystemReboot).setAuthentication("user", "pass");

  server.addHandler(new AsyncCallbackJsonWebHandler("/rest/system/config", handleConfigSave));

  server.onNotFound(handleNotFound);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);  // only when requested from AP

  server.begin();  // Actually start the server
  Serial.println("HTTP server started");
}

void Webserver() {
  // server.handleClient();  // Listen for HTTP requests from clients
}