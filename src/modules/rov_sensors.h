#pragma once

#include <stdint.h>

namespace ROVSensors {

struct State {
  bool i2cReady;
  bool imuReady;
  bool compassReady;
  bool pressureReady;
  bool vsysReady;
  char compassType[12];
  float accelG[3];
  float gyroDps[3];
  float magnetometer[3];
  float attitudePitch;
  float attitudeRoll;
  float compassHeading;
  float pressureMbar;
  float temperatureC;
  float vsysVoltage;
};

void begin();
void update();
void snapshot(State& destination);

}  // namespace ROVSensors
