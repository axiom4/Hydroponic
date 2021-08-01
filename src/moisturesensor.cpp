#include <hydroponic.h>

int calibration = 0;

int sensorRead() {
  int soilMoistureValue = 0;

  if (!calibration) {
    for (int i = 0; i < SENSOR_CALIBRATE_STEPS; i++) {
      int soilMoistureRead = analogRead(moisturePin);  // put Sensor insert into soil
      // Serial.println(soilMoistureRead);
      soilMoistureValue += soilMoistureRead;
      delay(100);
    }
    soilMoistureValue = soilMoistureValue / SENSOR_CALIBRATE_STEPS;
    Serial.println(soilMoistureValue);
  }
  return soilMoistureValue;
}

int sensorCalibrate() {
  int soilMoistureValue = 0;

  calibration = 1;

  for (int i = 0; i < SENSOR_CALIBRATE_STEPS; i++) {
    int soilMoistureRead = analogRead(moisturePin);  // put Sensor insert into soil
    // Serial.println(soilMoistureRead);
    soilMoistureValue += soilMoistureRead;
    delay(100);
  }
  soilMoistureValue = soilMoistureValue / SENSOR_CALIBRATE_STEPS;
  Serial.println(soilMoistureValue);

  calibration = 0;

  return soilMoistureValue;
}

void calibrateAirValue() {
  struct hydroponicConfig *config;
  config = eepromReadData();

  Serial.print("Calibrate Air Value: ");

  config->config_u.config.AirValue = sensorCalibrate();

  eepromWriteData(config);
  free(config);
}

void calibrateWaterValue() {
  struct hydroponicConfig *config;
  config = eepromReadData();

  Serial.print("Calibrate Water Value: ");

  config->config_u.config.WaterValue = sensorCalibrate();

  eepromWriteData(config);
  free(config);
}