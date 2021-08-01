#include <hydroponic.h>

#include <WiFiUdp.h>
#include <TimeLib.h>
#include <time.h>
#include <Timezone.h>

static tm getDateTimeByParams(long time) {
  struct tm *newtime;
  const time_t tim = time;
  newtime = localtime(&tim);
  return *newtime;
}

static String getDateTimeStringByParams(tm *newtime, char *pattern = (char *)"%d/%m/%Y %H:%M:%S") {
  char buffer[30];
  strftime(buffer, 30, pattern, newtime);
  return buffer;
}

static String getEpochStringByParams(long time, char *pattern = (char *)"%d/%m/%Y %H:%M:%S") {
  //    struct tm *newtime;
  tm newtime;
  newtime = getDateTimeByParams(time);
  return getDateTimeStringByParams(&newtime, pattern);
}

WiFiUDP ntpUDP;

bool ntpInitiate = false;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
// NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
int GTMOffset = 0;  // SET TO UTC TIME

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};  // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};    // Central European Standard Time
Timezone CE(CEST, CET);

void ntpSetup(struct hydroponicConfig *config) {
  Serial.printf("Setup NTP server: %s\n", config->config_u.config.ntp_sever);
  NTPClient timeClient(ntpUDP, config->config_u.config.ntp_sever, GTMOffset * 60 * 60, 60 * 60 * 1000);

  timeClient.begin();

  delay(1000);

  if (timeClient.update()) {
    Serial.println("Adjust local clock");
    unsigned long epoch = timeClient.getEpochTime();
    setTime(epoch);
    ntpInitiate = true;
  } else {
    Serial.println("NTP Update not WORK!!");
  }
}

void ntpLoop() {
  if (ntpInitiate) {
    Serial.println(getEpochStringByParams(CE.toLocal(now())));
  }
}