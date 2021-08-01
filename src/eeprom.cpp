#include <hydroponic.h>

int addr = 0;

void eepromSetup() { EEPROM.begin(512); }

void eepromWriteData(struct hydroponicConfig *config) {
  EEPROM.put(0, config->config_u.config_data);
  EEPROM.commit();
}

struct hydroponicConfig *eepromReadData() {
  struct hydroponicConfig *config;

  if (!(config = (struct hydroponicConfig *)malloc(sizeof(struct hydroponicConfig)))) {
    return NULL;
  }

  EEPROM.get(0, config->config_u.config_data);

  return config;
}