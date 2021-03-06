#include <hydroponic.h>

extern struct hydroponicConfig* s_config;

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
  HTML_PAGE +=
      "body{color:#333;font-family:helvetica,arial,sans-serif;line-height:1.5;"
      "margin:0 auto;max-width:40em;padding:0 1em;}";
  HTML_PAGE +=
      ".container{font: 95% Arial, Helvetica, sans-serif;	max-width: "
      "400px; margin: 10px auto;padding: 16px;background: #F7F7F7;}";
  HTML_PAGE +=
      ".container h1{background: #43D1AF;padding: 20px 0;font-size: "
      "140%;font-weight: 300;text-align: center;color: #fff;margin: -16px "
      "-16px 16px -16px;}";
  HTML_PAGE +=
      ".container input[type=\"text\"],.container "
      "input[type=\"date\"],.container input[type=\"datetime\"],.container "
      "input[type=\"email\"],.container "
      "input[type=\"number\"],.container[input[type=\"search\"],.container "
      "input[type=\"time\"],.container input[type=\"url\"],.container "
      "textarea,.container select "
      "{-webkit-transition: all 0.30s ease-in-out;-moz-transition: all 0.30s "
      "ease-in-out;-ms-transition: all 0.30s ease-in-out;-o-transition: all "
      "0.30s ease-in-out;outline: "
      "none;box-sizing: border-box;-webkit-box-sizing: "
      "border-box;-coz-box-sizing: border-box;width: 100%;background: "
      "#fff;margin-bottom: 4%;border: 1px solid #ccc;padding: "
      "3%;color: "
      "#555;font: 95% Arial, Helvetica, sans-serif;}";
  HTML_PAGE +=
      ".container input[type=\"text\"]:focus,.container "
      "input[type=\"date\"]:focus,.container "
      "input[type=\"datetime\"]:focus,.container "
      "input[type=\"email\"]:focus,.container "
      "input[type=\"number\"]:focus,.container "
      "input[type=\"search\"]:focus,.container "
      "input[type=\"time\"]:focus,.container "
      "input[type=\"url\"]:focus,.container "
      "textarea:focus,.container select:focus {box-shadow: 0 0 5px "
      "#43D1AF;	padding: 3%;border: 1px solid #43D1AF;}";

  HTML_PAGE +=
      ".container input[type=\"submit\"],.container "
      "input[type=\"button\"]{box-sizing: border-box;-webkit-box-sizing: "
      "border-box;-moz-box-sizing: border-box;width: 100%;padding: "
      "3%;background: #43D1AF;border-bottom: 2px solid "
      "#30C29E;border-top-style: none;border-right-style: "
      "none;border-left-style: none;color: #fff;}";

  HTML_PAGE += "label {margin-top: 10px}";

  HTML_PAGE +=
      ".container input[type=\"submit\"]:hover,.container "
      "input[type=\"button\"]:hover{background: #2EBC99;}";

  HTML_PAGE += "</style>";

  HTML_PAGE += "</head>";

  HTML_PAGE += " <body>";
  HTML_PAGE += body;

  HTML_PAGE += "</body>";
  HTML_PAGE += "</html>";

  return HTML_PAGE;
}

void handleRoot(
    AsyncWebServerRequest* request) {  // When URI / is requested, send a web
                                       // page with a button to toggle the LED
  String form = "";

  form += "<div class=\"container\">";
  form += "<h1>Hydroponic Configuration Page</h1>";

  form += "<form action=\"/setup\" method=\"POST\">";

  form +=
      "<label for=\"hostname\">Hostname:</label><br/><input type=\"text\" "
      "id=\"hostname\" name=\"hostname\" placeholder=\"Hostname\" value=\"";
  form += String(s_config->config_u.config.hostname);
  form += "\">";

  form += "</br>";

  form +=
      "<label for=\"ssid\">Wi-Fi SSID:</label><br/><input type=\"text\" "
      "id=\"ssid\" name=\"ssid\" placeholder=\"Wi-Fi SSID\" value=\"";
  form += String(s_config->config_u.config.wifi_ssid);
  form += "\">";

  form += "</br>";

  form +=
      "<label for=\"password\">Wi-Fi Password:</label><br/><input "
      "type=\"text\" id=\"password\" name=\"password\" placeholder=\"Wi-Fi "
      "Password\" value=\"";
  form += String(s_config->config_u.config.wifi_password);
  form += "\">";

  form += "</br>";

  form +=
      "<label for=\"ntpserver\">NTP Sever:</label><br/><input type=\"text\" "
      "id=\"ntpserver\" name=\"ntpserver\" placeholder=\"Ntp Server\" value=\"";
  form += String(s_config->config_u.config.ntp_server);
  form += "\">";

  form += "</br>";

  form += "<input type=\"submit\" value=\"Setup\">";
  form += "</form>";
  form += "<p>Configure data.</p>";

  form +=
      "<label for=\"moisture_air_value\">Moisture Sensor Air "
      "Value:</label><br/><input disabled type=\"text\" "
      "id=\"moisture_air_value\" name=\"moisture_air_value\" "
      "placeholder=\"Moisture "
      "Sensor Air Value\" "
      "value=\"";
  form += String(s_config->config_u.config.AirValue);
  form += "\">";

  form += "</br>";

  form +=
      "<label for=\"moisture_water_value\">Moisture Sensor Water "
      "Value:</label><br/><input disabled type=\"text\" "
      "id=\"moisture_water_value\" name=\"moisture_water_value\" "
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

void handleSetup(AsyncWebServerRequest* request) {
  if (!request->hasArg("hostname") || request->arg("hostname") == NULL ||
      !request->hasArg("ssid") || request->arg("ssid") == NULL ||
      !request->hasArg("password") || request->arg("password") == NULL ||
      !request->hasArg("ntpserver") || request->arg("ntpserver") == NULL) {
    request->send(400, "text/plain", "400: Invalid Request");
    return;
  }

  struct hydroponicConfig* config;
  config = eepromReadData();

  strcpy(config->config_u.config.wifi_ssid, request->arg("ssid").c_str());
  strcpy(config->config_u.config.wifi_password,
         request->arg("password").c_str());
  strcpy(config->config_u.config.ntp_server, request->arg("ntpserver").c_str());
  strcpy(config->config_u.config.hostname, request->arg("hostname").c_str());

  eepromWriteData(config);

  free(config);
  String body = "<h1>Configuration for device \"<b>" +
                request->arg("hostname") +
                "</b>\" has been saved.</h1><p>restart device</p>";
  request->send(200, "text/html", getPage(body));

  delay(5000);

  ESP.restart();
}

void handleCalibrate(AsyncWebServerRequest* request) {
  String body = "<h1>Step1: Calibrate Air Value</h1>";
  body += "<p>Please clean the humidity sensor and keep outdoors.</p>";
  body += "<p>Go to <a href=\"/calibrate_air\">next step</a>.</p>";
  request->send(200, "text/html", getPage(body));
}

void handleCalibrateAir(AsyncWebServerRequest* request) {
  calibrateAirValue();

  String body = "<h1>Step2: Calibrate Water Value</h1>";
  body += "<p>Please insert humidity sensor into a cup of water.</p>";
  body += "<p>Go to <a href=\"/calibrate_water\">next step</a>.</p>";
  request->send(200, "text/html", getPage(body));
}

void handleCalibrateWater(AsyncWebServerRequest* request) {
  calibrateWaterValue();

  String body = "<h1>Congratulations</h1>";
  body += "<p>Moisture sensor calibration has been completed.</p>";
  body += "<p>Return to <a href=\"/\">main configuration page</a>.</p>";
  request->send(200, "text/html", getPage(body));
}
