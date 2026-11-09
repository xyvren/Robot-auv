#include <Arduino.h>
#include <EEPROM.h>
#include <Servo.h>
#include <WebServer.h>
#include <WiFi.h>
#include <atomic>
#include <math.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "modules/rov_config.h"
#include "modules/rov_sensors.h"
#include "modules/web_pages.h"

namespace {

using namespace ROVConfig;

const IPAddress ACCESS_POINT_IP(192, 168, 4, 1);
const IPAddress ACCESS_POINT_SUBNET(255, 255, 255, 0);
constexpr uint32_t TEST_SEQUENCE_STEP_MS = 10000;
constexpr size_t TEST_SEQUENCE_STEP_COUNT = 6;
constexpr uint8_t TEST_SEQUENCE_AXES[TEST_SEQUENCE_STEP_COUNT] = {0, 3, 0, 0, 1, 1};
constexpr int8_t TEST_SEQUENCE_DIRECTIONS[TEST_SEQUENCE_STEP_COUNT] = {1, 1, -1, 1, 1, -1};

struct Settings {
  uint32_t magic;
  uint8_t version;
  uint8_t pins[THRUSTER_COUNT];
  uint16_t pulseMin[THRUSTER_COUNT];
  uint16_t pulseNeutral[THRUSTER_COUNT];
  uint16_t pulseMax[THRUSTER_COUNT];
  uint8_t reverse[THRUSTER_COUNT];
  float mix[THRUSTER_COUNT][AXIS_COUNT];
  uint8_t maxPowerPercent;
  uint8_t rampPercentPerSecond;
  uint16_t failsafeMs;
  char wifiSsid[33];
  char wifiPassword[64];
  char webPassword[33];
  float stabilizationKp;
  float stabilizationKi;
  float stabilizationKd;
  uint8_t stabilizationMaxOutputPercent;
  uint8_t stabilizationMaxAngleDegrees;
  uint8_t stabilizationTargetRateDps;
  float depthKp;
  float depthKi;
  float depthKd;
  uint8_t depthMaxOutputPercent;
  uint8_t depthDeadbandCm;
  uint16_t depthTargetCm;
  uint32_t checksum;
};

struct LegacySettings {
  uint32_t magic;
  uint8_t version;
  uint8_t pins[THRUSTER_COUNT];
  uint16_t pulseMin[THRUSTER_COUNT];
  uint16_t pulseNeutral[THRUSTER_COUNT];
  uint16_t pulseMax[THRUSTER_COUNT];
  uint8_t reverse[THRUSTER_COUNT];
  float mix[THRUSTER_COUNT][AXIS_COUNT];
  uint8_t maxPowerPercent;
  uint8_t rampPercentPerSecond;
  uint16_t failsafeMs;
  char wifiSsid[33];
  char wifiPassword[64];
  char webPassword[33];
  uint32_t checksum;
};

Settings settings{};
Servo thrusters[THRUSTER_COUNT];
WebServer server(80);
float axisInput[AXIS_COUNT]{};
float motorOutputApplied[THRUSTER_COUNT]{};
float individualThrusterInput[THRUSTER_COUNT]{};
bool individualThrusterMode = false;
bool setupMode = false;
bool armed = false;
bool outputsReady = false;
bool serverStarted = false;
bool accessPointReady = false;
bool serialWasConnected = false;
bool stabilizationEnabled = false;
bool stabilizationFaulted = false;
bool depthHoldEnabled = false;
bool autoModeEnabled = false;
bool testSequenceActive = false;
uint8_t testSequenceStep = 0;
uint32_t testSequenceStepStartedAt = 0;
float testSequenceStrength = 0.30f;
uint32_t lastControlAt = 0;
uint32_t lastOutputAt = 0;
uint32_t lastSetupMessageAt = 0;
uint32_t lastStatusMessageAt = 0;
float stabilizationTargetPitch = 0.0f;
float stabilizationTargetRoll = 0.0f;
float stabilizationPitchIntegral = 0.0f;
float stabilizationRollIntegral = 0.0f;
float depthIntegral = 0.0f;
float previousDepthCm = 0.0f;
bool previousDepthValid = false;
String setupApName;
String setupApPassword;
char activeWebPassword[33]{};
std::atomic<bool> sensorsInitialized{false};
float depthReferencePressureMbar = 0.0f;
bool depthReferenceSet = false;

uint32_t checksumSettings(const Settings& value) {
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
  uint32_t hash = 2166136261u;
  for (size_t i = 0; i < offsetof(Settings, checksum); ++i) {
    hash = (hash ^ bytes[i]) * 16777619u;
  }
  return hash;
}

uint32_t checksumLegacySettings(const LegacySettings& value) {
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
  uint32_t hash = 2166136261u;
  for (size_t i = 0; i < offsetof(LegacySettings, checksum); ++i) {
    hash = (hash ^ bytes[i]) * 16777619u;
  }
  return hash;
}

void appendJsonString(String& json, const char* value) {
  static const char HEX_DIGITS[] = "0123456789abcdef";
  json += '"';
  for (const unsigned char* character =
           reinterpret_cast<const unsigned char*>(value);
       *character != '\0'; ++character) {
    switch (*character) {
      case '"': json += "\\\""; break;
      case '\\': json += "\\\\"; break;
      case '\b': json += "\\b"; break;
      case '\f': json += "\\f"; break;
      case '\n': json += "\\n"; break;
      case '\r': json += "\\r"; break;
      case '\t': json += "\\t"; break;
      default:
        if (*character < 0x20) {
          json += "\\u00";
          json += HEX_DIGITS[*character >> 4];
          json += HEX_DIGITS[*character & 0x0f];
        } else {
          json += static_cast<char>(*character);
        }
        break;
    }
  }
  json += '"';
}

bool validPin(uint8_t pin) {
  return isThrusterPinAllowed(pin);
}

bool validateSettings(const Settings& value) {
  if (value.magic != CONFIG_MAGIC || value.version != CONFIG_VERSION ||
      value.maxPowerPercent < 1 || value.maxPowerPercent > 100 ||
      value.rampPercentPerSecond < 5 || value.rampPercentPerSecond > 200 ||
      value.failsafeMs < MIN_FAILSAFE_MS || value.failsafeMs > MAX_FAILSAFE_MS ||
      !isfinite(value.stabilizationKp) || value.stabilizationKp < 0.0f ||
      value.stabilizationKp > 1.0f || !isfinite(value.stabilizationKi) ||
      value.stabilizationKi < 0.0f || value.stabilizationKi > 0.1f ||
      !isfinite(value.stabilizationKd) || value.stabilizationKd < 0.0f ||
      value.stabilizationKd > 1.0f ||
      value.stabilizationMaxOutputPercent < 5 ||
      value.stabilizationMaxOutputPercent > 100 ||
      value.stabilizationMaxAngleDegrees < 5 ||
      value.stabilizationMaxAngleDegrees > 45 ||
      value.stabilizationTargetRateDps < 1 ||
      value.stabilizationTargetRateDps > 60 || !isfinite(value.depthKp) ||
      value.depthKp < 0.0f || value.depthKp > 2.0f ||
      !isfinite(value.depthKi) || value.depthKi < 0.0f ||
      value.depthKi > 0.5f || !isfinite(value.depthKd) ||
      value.depthKd < 0.0f || value.depthKd > 1.0f ||
      value.depthMaxOutputPercent < 5 || value.depthMaxOutputPercent > 100 ||
      value.depthDeadbandCm > 25 || value.depthTargetCm > 500) {
    return false;
  }

  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (!validPin(value.pins[i]) ||
        value.pulseMin[i] < 900 || value.pulseMin[i] >= value.pulseNeutral[i] ||
        value.pulseNeutral[i] > 1600 || value.pulseNeutral[i] < 1400 ||
        value.pulseMax[i] > 2100 || value.pulseMax[i] <= value.pulseNeutral[i] ||
        value.reverse[i] > 1) {
      return false;
    }
    for (size_t j = 0; j < i; ++j) {
      if (value.pins[i] == value.pins[j]) {
        return false;
      }
    }
    for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
      if (!isfinite(value.mix[i][axis]) || value.mix[i][axis] < -1.0f ||
          value.mix[i][axis] > 1.0f) {
        return false;
      }
    }
  }

  return value.wifiSsid[0] != '\0' &&
         strnlen(value.wifiPassword, sizeof(value.wifiPassword)) == 0 &&
         strnlen(value.webPassword, sizeof(value.webPassword)) >=
             MIN_WEB_PASSWORD_LENGTH &&
         strnlen(value.wifiSsid, sizeof(value.wifiSsid)) < sizeof(value.wifiSsid) &&
         strnlen(value.wifiPassword, sizeof(value.wifiPassword)) < sizeof(value.wifiPassword) &&
         strnlen(value.webPassword, sizeof(value.webPassword)) < sizeof(value.webPassword);
}

void setDefaults() {
  memset(&settings, 0, sizeof(settings));
  settings.magic = CONFIG_MAGIC;
  settings.version = CONFIG_VERSION;
  memcpy(settings.wifiSsid, DEFAULT_HOTSPOT_NAME,
         sizeof(DEFAULT_HOTSPOT_NAME));
  memcpy(settings.pins, DEFAULT_THRUSTER_PINS, sizeof(DEFAULT_THRUSTER_PINS));

  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    settings.pulseMin[i] = DEFAULT_PULSE_MIN_US;
    settings.pulseNeutral[i] = DEFAULT_PULSE_NEUTRAL_US;
    settings.pulseMax[i] = DEFAULT_PULSE_MAX_US;
  }

  memcpy(settings.mix, DEFAULT_MIX, sizeof(DEFAULT_MIX));
  settings.maxPowerPercent = DEFAULT_MAX_POWER_PERCENT;
  settings.rampPercentPerSecond = DEFAULT_RAMP_PERCENT_PER_SECOND;
  settings.failsafeMs = DEFAULT_FAILSAFE_MS;
  settings.stabilizationKp = DEFAULT_STABILIZATION_KP;
  settings.stabilizationKi = DEFAULT_STABILIZATION_KI;
  settings.stabilizationKd = DEFAULT_STABILIZATION_KD;
  settings.stabilizationMaxOutputPercent =
      DEFAULT_STABILIZATION_MAX_OUTPUT_PERCENT;
  settings.stabilizationMaxAngleDegrees =
      DEFAULT_STABILIZATION_MAX_ANGLE_DEGREES;
  settings.stabilizationTargetRateDps = DEFAULT_STABILIZATION_TARGET_RATE_DPS;
  settings.depthKp = DEFAULT_DEPTH_KP;
  settings.depthKi = DEFAULT_DEPTH_KI;
  settings.depthKd = DEFAULT_DEPTH_KD;
  settings.depthMaxOutputPercent = DEFAULT_DEPTH_MAX_OUTPUT_PERCENT;
  settings.depthDeadbandCm = DEFAULT_DEPTH_DEADBAND_CM;
  settings.depthTargetCm = DEFAULT_DEPTH_TARGET_CM;
}

bool persistSettings();

bool loadSettings() {
  EEPROM.begin(sizeof(Settings));
  EEPROM.get(0, settings);
  if (settings.magic == CONFIG_MAGIC && settings.version == CONFIG_VERSION &&
      settings.checksum == checksumSettings(settings)) {
    if (validateSettings(settings)) {
      return true;
    }
  }

  LegacySettings legacy{};
  EEPROM.get(0, legacy);
  if (legacy.magic != CONFIG_MAGIC || legacy.version < 2 ||
      legacy.version > 3 ||
      legacy.checksum != checksumLegacySettings(legacy)) {
    setDefaults();
    return false;
  }

  if (legacy.version == 2) {
    setDefaults();
    return false;
  }

  setDefaults();
  memcpy(settings.pins, legacy.pins, sizeof(settings.pins));
  memcpy(settings.pulseMin, legacy.pulseMin, sizeof(settings.pulseMin));
  memcpy(settings.pulseNeutral, legacy.pulseNeutral,
         sizeof(settings.pulseNeutral));
  memcpy(settings.pulseMax, legacy.pulseMax, sizeof(settings.pulseMax));
  memcpy(settings.reverse, legacy.reverse, sizeof(settings.reverse));
  memcpy(settings.mix, legacy.mix, sizeof(settings.mix));
  settings.maxPowerPercent = legacy.maxPowerPercent;
  settings.rampPercentPerSecond = legacy.rampPercentPerSecond;
  settings.failsafeMs = legacy.failsafeMs;
  memcpy(settings.wifiSsid, legacy.wifiSsid, sizeof(settings.wifiSsid));
  memcpy(settings.wifiPassword, legacy.wifiPassword,
         sizeof(settings.wifiPassword));
  memcpy(settings.webPassword, legacy.webPassword,
         sizeof(settings.webPassword));
  if (!validateSettings(settings) || !persistSettings()) {
    Serial.printf("ERROR: migrasi konfigurasi v%u gagal; gunakan setup hotspot.\n",
                  static_cast<unsigned>(legacy.version));
    setDefaults();
    return false;
  }
  Serial.printf("Konfigurasi v%u dimigrasikan dengan PID default.\n",
                static_cast<unsigned>(legacy.version));
  return true;
}

bool persistSettings() {
  settings.magic = CONFIG_MAGIC;
  settings.version = CONFIG_VERSION;
  settings.checksum = checksumSettings(settings);
  EEPROM.put(0, settings);
  return EEPROM.commit();
}

bool copyField(const String& source, char* destination, size_t capacity,
               bool allowEmpty = true) {
  if ((!allowEmpty && source.isEmpty()) || source.length() >= capacity) {
    return false;
  }
  memset(destination, 0, capacity);
  source.toCharArray(destination, capacity);
  return true;
}

bool readIntegerArg(const String& name, long minimum, long maximum, long& result) {
  if (!server.hasArg(name)) {
    return false;
  }
  String input = server.arg(name);
  if (input.isEmpty()) {
    return false;
  }
  char* end = nullptr;
  const long value = strtol(input.c_str(), &end, 10);
  if (end == input.c_str() || *end != '\0' || value < minimum || value > maximum) {
    return false;
  }
  result = value;
  return true;
}

bool readBoundedFloatArg(const String& name, float minimum, float maximum,
                         float& result) {
  if (!server.hasArg(name)) {
    return false;
  }
  String input = server.arg(name);
  if (input.isEmpty()) {
    return false;
  }
  char* end = nullptr;
  const float value = strtof(input.c_str(), &end);
  if (end == input.c_str() || *end != '\0' || !isfinite(value) ||
      value < minimum || value > maximum) {
    return false;
  }
  result = value;
  return true;
}

bool readFloatArg(const String& name, float& result) {
  return readBoundedFloatArg(name, -1.0f, 1.0f, result);
}

bool requireAuthentication() {
  if (activeWebPassword[0] != '\0' &&
      server.authenticate(DEFAULT_WEB_USERNAME, activeWebPassword)) {
    return true;
  }
  server.requestAuthentication(BASIC_AUTH, "ROV control");
  return false;
}

void sendError(int status, const String& message) {
  server.send(status, "text/plain; charset=utf-8", message);
}

void resetPidState() {
  stabilizationPitchIntegral = 0.0f;
  stabilizationRollIntegral = 0.0f;
  depthIntegral = 0.0f;
  previousDepthValid = false;
}

void stopAllThrusters() {
  armed = false;
  individualThrusterMode = false;
  stabilizationEnabled = false;
  depthHoldEnabled = false;
  autoModeEnabled = false;
  testSequenceActive = false;
  testSequenceStep = 0;
  resetPidState();
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    axisInput[i] = 0.0f;
    individualThrusterInput[i] = 0.0f;
    motorOutputApplied[i] = 0.0f;
    if (thrusters[i].attached()) {
      thrusters[i].writeMicroseconds(settings.pulseNeutral[i]);
    }
  }
}

void applyTestSequenceStep(uint8_t step) {
  memset(axisInput, 0, sizeof(axisInput));
  axisInput[TEST_SEQUENCE_AXES[step]] =
      testSequenceStrength * TEST_SEQUENCE_DIRECTIONS[step];
}

void updateTestSequence(uint32_t now) {
  if (!testSequenceActive ||
      now - testSequenceStepStartedAt < TEST_SEQUENCE_STEP_MS) {
    return;
  }

  ++testSequenceStep;
  if (testSequenceStep >= TEST_SEQUENCE_STEP_COUNT) {
    Serial.println("Urutan uji selesai; semua thruster dinetralkan.");
    stopAllThrusters();
    return;
  }

  testSequenceStepStartedAt = now;
  applyTestSequenceStep(testSequenceStep);
}

bool axisHasMixer(size_t axis) {
  for (size_t motor = 0; motor < THRUSTER_COUNT; ++motor) {
    if (fabsf(settings.mix[motor][axis]) > 0.001f) {
      return true;
    }
  }
  return false;
}

bool attachThrusters() {
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    const int channel = thrusters[i].attach(
        settings.pins[i], settings.pulseMin[i], settings.pulseMax[i],
        settings.pulseNeutral[i]);
    if (channel < 0) {
      Serial.printf("ERROR: gagal attach thruster %u pada GP%u\n",
                    static_cast<unsigned>(i + 1),
                    static_cast<unsigned>(settings.pins[i]));
      return false;
    }
    thrusters[i].writeMicroseconds(settings.pulseNeutral[i]);
  }
  return true;
}

void updateThrusters() {
  if (!armed || !outputsReady) {
    stopAllThrusters();
    return;
  }

  const uint32_t now = millis();
  const uint32_t elapsedMs = lastOutputAt == 0
      ? 20
      : min(static_cast<uint32_t>(now - lastOutputAt), 50u);
  lastOutputAt = now;

  float commandedAxes[AXIS_COUNT];
  memcpy(commandedAxes, axisInput, sizeof(commandedAxes));
  if ((stabilizationEnabled || depthHoldEnabled) && !individualThrusterMode) {
    ROVSensors::State sensor{};
    ROVSensors::snapshot(sensor);
    const bool attitudeReady = sensor.imuReady &&
        isfinite(sensor.attitudePitch) && isfinite(sensor.attitudeRoll) &&
        isfinite(sensor.gyroDps[0]) && isfinite(sensor.gyroDps[1]);
    const bool pressureReady = sensor.pressureReady &&
        isfinite(sensor.pressureMbar) && sensor.pressureMbar > 0.0f &&
        depthReferenceSet;
    if (stabilizationEnabled && !attitudeReady) {
      stabilizationEnabled = false;
      depthHoldEnabled = false;
      autoModeEnabled = false;
      axisInput[4] = 0.0f;
      axisInput[5] = 0.0f;
      commandedAxes[4] = 0.0f;
      commandedAxes[5] = 0.0f;
      commandedAxes[2] = 0.0f;
      stabilizationFaulted = true;
      resetPidState();
      Serial.println("WARNING: IMU tidak tersedia; stabilisasi pitch/roll dimatikan.");
    }
    if (depthHoldEnabled && !pressureReady) {
      depthHoldEnabled = false;
      autoModeEnabled = false;
      commandedAxes[2] = 0.0f;
      depthIntegral = 0.0f;
      previousDepthValid = false;
      Serial.println("WARNING: sensor tekanan tidak tersedia; depth-hold dimatikan.");
    }
    if (stabilizationEnabled || depthHoldEnabled) {
      const float elapsedSeconds = elapsedMs / 1000.0f;
      if (stabilizationEnabled) {
        const float maxTargetAngle = settings.stabilizationMaxAngleDegrees;
        const float targetRate = settings.stabilizationTargetRateDps;
        stabilizationTargetPitch = constrain(
            stabilizationTargetPitch + axisInput[4] * targetRate * elapsedSeconds,
            -maxTargetAngle, maxTargetAngle);
        stabilizationTargetRoll = constrain(
            stabilizationTargetRoll + axisInput[5] * targetRate * elapsedSeconds,
            -maxTargetAngle, maxTargetAngle);

        const float maxOutput =
            settings.stabilizationMaxOutputPercent / 100.0f;
        const float pitchError = stabilizationTargetPitch - sensor.attitudePitch;
        const float rollError = stabilizationTargetRoll - sensor.attitudeRoll;
        if (axisHasMixer(4)) {
          stabilizationPitchIntegral = constrain(
              stabilizationPitchIntegral +
                  settings.stabilizationKi * pitchError * elapsedSeconds,
              -maxOutput, maxOutput);
          commandedAxes[4] = constrain(
              settings.stabilizationKp * pitchError +
                  stabilizationPitchIntegral -
                  settings.stabilizationKd * sensor.gyroDps[1],
              -maxOutput, maxOutput);
        }
        if (axisHasMixer(5)) {
          stabilizationRollIntegral = constrain(
              stabilizationRollIntegral +
                  settings.stabilizationKi * rollError * elapsedSeconds,
              -maxOutput, maxOutput);
          commandedAxes[5] = constrain(
              settings.stabilizationKp * rollError +
                  stabilizationRollIntegral -
                  settings.stabilizationKd * sensor.gyroDps[0],
              -maxOutput, maxOutput);
        }
      }

      if (depthHoldEnabled) {
        const float depthMeters = (sensor.pressureMbar -
            depthReferencePressureMbar) * 100.0f / (1025.0f * 9.80665f);
        const float depthCm = depthMeters * 100.0f;
        const float targetDepthCm = settings.depthTargetCm;
        const float depthErrorCm = targetDepthCm - depthCm;
        float depthRateCmPerSecond = 0.0f;
        if (previousDepthValid && elapsedSeconds > 0.0f) {
          depthRateCmPerSecond =
              (depthCm - previousDepthCm) / elapsedSeconds;
        }
        previousDepthCm = depthCm;
        previousDepthValid = true;

        const float maxOutput = settings.depthMaxOutputPercent / 100.0f;
        if (fabsf(depthErrorCm) <= settings.depthDeadbandCm) {
          depthIntegral = 0.0f;
          commandedAxes[2] = 0.0f;
        } else {
          depthIntegral = constrain(
              depthIntegral + settings.depthKi * depthErrorCm * elapsedSeconds,
              -maxOutput, maxOutput);
          const float pidOutput = constrain(
              settings.depthKp * depthErrorCm + depthIntegral -
                  settings.depthKd * depthRateCmPerSecond,
              -maxOutput, maxOutput);
          commandedAxes[2] = -pidOutput;
        }
      }
    }
  }

  float motorOutput[THRUSTER_COUNT]{};
  float largestMagnitude = 1.0f;
  for (size_t motor = 0; motor < THRUSTER_COUNT; ++motor) {
    if (individualThrusterMode) {
      motorOutput[motor] = individualThrusterInput[motor];
    } else {
      for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
        motorOutput[motor] += commandedAxes[axis] * settings.mix[motor][axis];
      }
    }
    const float magnitude = fabsf(motorOutput[motor]);
    if (magnitude > largestMagnitude) {
      largestMagnitude = magnitude;
    }
  }

  const float powerLimit = settings.maxPowerPercent / 100.0f;
  const float maxStep = (settings.rampPercentPerSecond / 100.0f) *
      (elapsedMs / 1000.0f);
  for (size_t motor = 0; motor < THRUSTER_COUNT; ++motor) {
    const float target = (motorOutput[motor] / largestMagnitude) * powerLimit;
    if (target > motorOutputApplied[motor]) {
      motorOutputApplied[motor] =
          min(target, motorOutputApplied[motor] + maxStep);
    } else {
      motorOutputApplied[motor] =
          max(target, motorOutputApplied[motor] - maxStep);
    }

    float output = motorOutputApplied[motor];
    if (settings.reverse[motor]) {
      output = -output;
    }

    int pulse = settings.pulseNeutral[motor];
    if (output >= 0.0f) {
      pulse += static_cast<int>(output *
          (settings.pulseMax[motor] - settings.pulseNeutral[motor]));
    } else {
      pulse += static_cast<int>(output *
          (settings.pulseNeutral[motor] - settings.pulseMin[motor]));
    }
    thrusters[motor].writeMicroseconds(pulse);
  }
}

void sendConfiguration() {
  String json = "{\"pins\":[";
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (i) json += ',';
    json += String(settings.pins[i]);
  }
  json += "],\"min\":[";
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (i) json += ',';
    json += String(settings.pulseMin[i]);
  }
  json += "],\"neutral\":[";
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (i) json += ',';
    json += String(settings.pulseNeutral[i]);
  }
  json += "],\"max\":[";
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (i) json += ',';
    json += String(settings.pulseMax[i]);
  }
  json += "],\"reverse\":[";
  for (size_t i = 0; i < THRUSTER_COUNT; ++i) {
    if (i) json += ',';
    json += String(settings.reverse[i]);
  }
  json += "],\"mix\":[";
  for (size_t motor = 0; motor < THRUSTER_COUNT; ++motor) {
    if (motor) json += ',';
    json += '[';
    for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
      if (axis) json += ',';
      json += String(settings.mix[motor][axis], 2);
    }
    json += ']';
  }
  json += "],\"power\":" + String(settings.maxPowerPercent);
  json += ",\"ramp\":" + String(settings.rampPercentPerSecond);
  json += ",\"failsafe\":" + String(settings.failsafeMs);
  json += ",\"stabilizationKp\":" + String(settings.stabilizationKp, 4);
  json += ",\"stabilizationKi\":" + String(settings.stabilizationKi, 4);
  json += ",\"stabilizationKd\":" + String(settings.stabilizationKd, 4);
  json += ",\"stabilizationMaxOutput\":" +
      String(settings.stabilizationMaxOutputPercent);
  json += ",\"stabilizationMaxAngle\":" +
      String(settings.stabilizationMaxAngleDegrees);
  json += ",\"stabilizationTargetRate\":" +
      String(settings.stabilizationTargetRateDps);
  json += ",\"depthKp\":" + String(settings.depthKp, 4);
  json += ",\"depthKi\":" + String(settings.depthKi, 4);
  json += ",\"depthKd\":" + String(settings.depthKd, 4);
  json += ",\"depthMaxOutput\":" + String(settings.depthMaxOutputPercent);
  json += ",\"depthDeadband\":" + String(settings.depthDeadbandCm);
  json += ",\"depthTarget\":" + String(settings.depthTargetCm);
  json += ",\"ssid\":";
  appendJsonString(json, settings.wifiSsid);
  json += '}';
  server.send(200, "application/json", json);
}

void handleSetupPage() {
  if (!setupMode) {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Redirecting");
    return;
  }
  server.send_P(200, "text/html; charset=utf-8", ROVWebPages::SETUP_PAGE);
}

void handleSetupSave() {
  if (!setupMode) {
    sendError(403, "Wi-Fi setup is available only in setup mode.");
    return;
  }
  const String ssid = server.arg("ssid");
  const String wifiPassword = server.arg("wifiPass");
  const String webPassword = server.arg("webPass");
  if (!wifiPassword.isEmpty()) {
    sendError(400, "Hotspot ini dikonfigurasi tanpa password Wi-Fi.");
    return;
  }
  if (ssid.isEmpty() || ssid.length() > 32 ||
      webPassword.length() < MIN_WEB_PASSWORD_LENGTH ||
      webPassword.length() > 32) {
    sendError(400, "Nama hotspot atau password tidak valid.");
    return;
  }
  if (!copyField(ssid, settings.wifiSsid, sizeof(settings.wifiSsid), false) ||
      !copyField(webPassword, settings.webPassword, sizeof(settings.webPassword), false) ||
      !persistSettings()) {
    sendError(500, "Gagal menyimpan pengaturan. Periksa Serial Monitor.");
    return;
  }
  server.send(200, "text/plain; charset=utf-8",
              "Pengaturan tersimpan. Pico akan reboot. Sambungkan ke hotspot Pico lalu buka http://192.168.4.1.");
  delay(500);
  rp2040.reboot();
}

void handleHome() {
  if (setupMode) {
    server.sendHeader("Location", "/setup");
    server.send(302, "text/plain", "Redirecting");
    return;
  }
  if (!requireAuthentication()) return;
  server.send_P(200, "text/html; charset=utf-8", ROVWebPages::CONTROL_PAGE);
}

void handleControl() {
  if (!requireAuthentication()) return;
  if (setupMode || !outputsReady) {
    sendError(503, "Thruster belum siap.");
    return;
  }

  long requestedArm = 0;
  if (!readIntegerArg("armed", 0, 1, requestedArm)) {
    sendError(400, "Status ARM tidak valid.");
    return;
  }
  if (testSequenceActive && requestedArm) {
    lastControlAt = millis();
    server.send(200, "application/json",
                "{\"armed\":true,\"sequence\":true}");
    return;
  }
  float requestedAxes[AXIS_COUNT]{};
  for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
    String name = "a" + String(axis);
    long value = 0;
    if (!readIntegerArg(name, -100, 100, value)) {
      sendError(400, "Input sumbu tidak valid.");
      return;
    }
    requestedAxes[axis] = value / 100.0f;
  }
  if (stabilizationFaulted) {
    requestedAxes[4] = 0.0f;
    requestedAxes[5] = 0.0f;
  }

  if (!requestedArm) {
    stopAllThrusters();
  } else {
    armed = true;
    if (individualThrusterMode) {
      memset(motorOutputApplied, 0, sizeof(motorOutputApplied));
    }
    individualThrusterMode = false;
    memset(individualThrusterInput, 0, sizeof(individualThrusterInput));
    for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
      axisInput[axis] = requestedAxes[axis];
    }
  }
  lastControlAt = millis();
  updateThrusters();
  server.send(200, "application/json", armed ? "{\"armed\":true}" : "{\"armed\":false}");
}

void handleIndividualThrusterControl() {
  if (!requireAuthentication()) return;
  if (setupMode || !outputsReady) {
    sendError(503, "Thruster belum siap.");
    return;
  }
  if (!armed) {
    sendError(409, "ARM thruster sebelum kontrol langsung.");
    return;
  }
  if (testSequenceActive) {
    sendError(409, "Kontrol langsung tidak tersedia saat urutan uji berjalan.");
    return;
  }

  long motor = 0;
  long value = 0;
  if (!readIntegerArg("motor", 0, THRUSTER_COUNT - 1, motor) ||
      !readIntegerArg("value", -100, 100, value)) {
    sendError(400, "Thruster atau nilai kontrol tidak valid.");
    return;
  }

  memset(axisInput, 0, sizeof(axisInput));
  stabilizationEnabled = false;
  depthHoldEnabled = false;
  autoModeEnabled = false;
  resetPidState();
  if (!individualThrusterMode) {
    memset(motorOutputApplied, 0, sizeof(motorOutputApplied));
  }
  memset(individualThrusterInput, 0, sizeof(individualThrusterInput));
  individualThrusterMode = true;
  individualThrusterInput[static_cast<size_t>(motor)] = value / 100.0f;
  lastControlAt = millis();
  updateThrusters();
  server.send(200, "application/json", "{\"individual\":true}");
}

void handleTestSequenceStart() {
  if (!requireAuthentication()) return;
  if (setupMode || !outputsReady) {
    sendError(503, "Thruster belum siap.");
    return;
  }
  if (!armed) {
    sendError(409, "ARM thruster sebelum memulai urutan uji.");
    return;
  }
  if (testSequenceActive) {
    sendError(409, "Urutan uji sedang berjalan.");
    return;
  }

  long strength = 0;
  if (!readIntegerArg("strength", 10, 100, strength)) {
    sendError(400, "Kekuatan urutan harus antara 10 dan 100 persen.");
    return;
  }

  testSequenceStrength = strength / 100.0f;
  testSequenceStep = 0;
  testSequenceStepStartedAt = millis();
  testSequenceActive = true;
  individualThrusterMode = false;
  stabilizationEnabled = false;
  depthHoldEnabled = false;
  autoModeEnabled = false;
  resetPidState();
  memset(individualThrusterInput, 0, sizeof(individualThrusterInput));
  memset(motorOutputApplied, 0, sizeof(motorOutputApplied));
  applyTestSequenceStep(testSequenceStep);
  lastControlAt = testSequenceStepStartedAt;
  updateThrusters();
  Serial.println("Urutan uji dimulai manual; enam tahap masing-masing 10 detik.");
  server.send(200, "application/json",
              "{\"active\":true,\"step\":1,\"steps\":6}");
}

void handleTestSequenceHeartbeat() {
  if (!requireAuthentication()) return;
  if (testSequenceActive && armed) {
    lastControlAt = millis();
  }
  String response = "{\"active\":";
  response += testSequenceActive ? "true" : "false";
  response += ",\"step\":";
  response += String(testSequenceActive ? testSequenceStep + 1 : 0);
  response += ",\"armed\":";
  response += armed ? "true}" : "false}";
  server.send(200, "application/json", response);
}

void handleStatus() {
  if (!requireAuthentication()) return;
  const IPAddress ip = WiFi.softAPIP();
  String json = "{\"ip\":\"" + ip.toString() + "\",\"armed\":";
  json += armed ? "true}" : "false}";
  server.send(200, "application/json", json);
}

void handleDepthZero() {
  if (!requireAuthentication()) return;
  if (armed) {
    sendError(409, "DISARM sebelum mengatur nol kedalaman.");
    return;
  }

  ROVSensors::State sensor{};
  ROVSensors::snapshot(sensor);
  if (!sensor.pressureReady || !isfinite(sensor.pressureMbar) ||
      sensor.pressureMbar <= 0.0f) {
    sendError(503, "Sensor tekanan belum siap.");
    return;
  }

  depthReferencePressureMbar = sensor.pressureMbar;
  depthReferenceSet = true;
  server.send(200, "application/json", "{\"calibrated\":true}");
}

void handleTelemetry() {
  if (!requireAuthentication()) return;

  ROVSensors::State sensor{};
  ROVSensors::snapshot(sensor);
  String json = "{\"imu\":{\"online\":";
  json += sensor.imuReady ? "true" : "false";
  json += ",\"pitch\":" + String(sensor.attitudePitch, 2);
  json += ",\"roll\":" + String(sensor.attitudeRoll, 2);
  json += ",\"accel\":[" + String(sensor.accelG[0], 3) + ',' +
      String(sensor.accelG[1], 3) + ',' + String(sensor.accelG[2], 3) + ']';
  json += ",\"gyro\":[" + String(sensor.gyroDps[0], 2) + ',' +
      String(sensor.gyroDps[1], 2) + ',' + String(sensor.gyroDps[2], 2) + ']';
  json += "},\"compass\":{\"online\":";
  json += sensor.compassReady ? "true" : "false";
  json += ",\"type\":";
  appendJsonString(json, sensor.compassType);
  json += ",\"heading\":" + String(sensor.compassHeading, 2);
  json += ",\"raw\":[" + String(sensor.magnetometer[0], 1) + ',' +
      String(sensor.magnetometer[1], 1) + ',' +
      String(sensor.magnetometer[2], 1) + ']';
  json += "},\"pressure\":{\"online\":";
  json += sensor.pressureReady ? "true" : "false";
  json += ",\"pressureMbar\":" + String(sensor.pressureMbar, 1);
  json += ",\"temperatureC\":" + String(sensor.temperatureC, 2);
  json += ",\"depthCalibrated\":";
  json += depthReferenceSet ? "true" : "false";
  const float depthMeters = depthReferenceSet && sensor.pressureReady
      ? (sensor.pressureMbar - depthReferencePressureMbar) * 100.0f /
            (1025.0f * 9.80665f)
      : 0.0f;
  json += ",\"depthM\":" + String(depthMeters, 2) + '}';
  json += ",\"power\":{\"vsysOnline\":";
  json += sensor.vsysReady ? "true" : "false";
  json += ",\"vsysV\":" + String(sensor.vsysVoltage, 2) + '}';
  json += ",\"i2cReady\":";
  json += sensor.i2cReady ? "true" : "false";
  json += ",\"armed\":";
  json += armed ? "true" : "false";
  json += ",\"sequence\":{\"active\":";
  json += testSequenceActive ? "true" : "false";
  json += ",\"step\":";
  json += String(testSequenceActive ? testSequenceStep + 1 : 0);
  json += '}';
  json += ",\"stabilization\":{\"enabled\":";
  json += stabilizationEnabled ? "true" : "false";
  json += ",\"faulted\":";
  json += stabilizationFaulted ? "true" : "false";
  json += ",\"heaveMixer\":";
  json += axisHasMixer(2) ? "true" : "false";
  json += ",\"pitchMixer\":";
  json += axisHasMixer(4) ? "true" : "false";
  json += ",\"rollMixer\":";
  json += axisHasMixer(5) ? "true" : "false";
  json += '}';
  json += ",\"auto\":{\"enabled\":";
  json += autoModeEnabled ? "true" : "false";
  json += ",\"depthHold\":";
  json += depthHoldEnabled ? "true" : "false";
  json += ",\"targetDepthCm\":" + String(settings.depthTargetCm) + '}';
  json += ",\"thrustersReady\":";
  json += outputsReady ? "true" : "false";
  json += ",\"uptime\":" + String(millis() / 1000) + '}';
  server.send(200, "application/json", json);
}

void handleStabilization() {
  if (!requireAuthentication()) return;
  if (setupMode || !outputsReady) {
    sendError(503, "Thruster belum siap.");
    return;
  }
  if (testSequenceActive) {
    sendError(409, "Stabilisasi tidak dapat diubah saat urutan uji berjalan.");
    return;
  }

  long requestedEnabled = 0;
  if (!readIntegerArg("enabled", 0, 1, requestedEnabled)) {
    sendError(400, "Status stabilisasi tidak valid.");
    return;
  }
  if (requestedEnabled) {
    if (!armed) {
      sendError(409, "ARM thruster sebelum mengaktifkan stabilisasi.");
      return;
    }
    if (individualThrusterMode) {
      sendError(409, "Keluar dari kontrol langsung sebelum mengaktifkan stabilisasi.");
      return;
    }

    ROVSensors::State sensor{};
    ROVSensors::snapshot(sensor);
    if (!sensor.imuReady || !isfinite(sensor.attitudePitch) ||
        !isfinite(sensor.attitudeRoll) || !isfinite(sensor.gyroDps[0]) ||
        !isfinite(sensor.gyroDps[1])) {
      sendError(503, "Sensor MPU-6050 belum siap.");
      return;
    }
    if (!axisHasMixer(4) && !axisHasMixer(5)) {
      sendError(409, "Mixer pitch dan roll belum dikonfigurasi.");
      return;
    }

    stabilizationTargetPitch = 0.0f;
    stabilizationTargetRoll = 0.0f;
    resetPidState();
    stabilizationFaulted = false;
    stabilizationEnabled = true;
  } else {
    stabilizationEnabled = false;
    depthHoldEnabled = false;
    autoModeEnabled = false;
    resetPidState();
    stabilizationFaulted = false;
  }

  lastControlAt = millis();
  updateThrusters();
  String response = "{\"enabled\":";
  response += stabilizationEnabled ? "true" : "false";
  response += ",\"faulted\":";
  response += stabilizationFaulted ? "true}" : "false}";
  server.send(200, "application/json", response);
}

void handleAutoMode() {
  if (!requireAuthentication()) return;
  if (setupMode || !outputsReady) {
    sendError(503, "Thruster belum siap.");
    return;
  }
  if (testSequenceActive || individualThrusterMode) {
    sendError(409, "Keluar dari kontrol langsung atau urutan uji sebelum mode Auto.");
    return;
  }

  long requestedEnabled = 0;
  if (!readIntegerArg("enabled", 0, 1, requestedEnabled)) {
    sendError(400, "Status mode Auto tidak valid.");
    return;
  }
  if (requestedEnabled) {
    if (!armed) {
      sendError(409, "ARM thruster sebelum mengaktifkan mode Auto.");
      return;
    }

    ROVSensors::State sensor{};
    ROVSensors::snapshot(sensor);
    if (!depthReferenceSet || !sensor.pressureReady ||
        !isfinite(sensor.pressureMbar) || sensor.pressureMbar <= 0.0f) {
      sendError(503, "Nolkan sensor kedalaman di permukaan sebelum mode Auto.");
      return;
    }
    if (!sensor.imuReady || !isfinite(sensor.attitudePitch) ||
        !isfinite(sensor.attitudeRoll) || !isfinite(sensor.gyroDps[0]) ||
        !isfinite(sensor.gyroDps[1])) {
      sendError(503, "Sensor MPU-6050 belum siap.");
      return;
    }
    if (!axisHasMixer(2) || (!axisHasMixer(4) && !axisHasMixer(5))) {
      sendError(409, "Mixer heave dan minimal satu sumbu pitch/roll harus dikonfigurasi.");
      return;
    }

    axisInput[2] = 0.0f;
    axisInput[4] = 0.0f;
    axisInput[5] = 0.0f;
    stabilizationTargetPitch = 0.0f;
    stabilizationTargetRoll = 0.0f;
    stabilizationFaulted = false;
    stabilizationEnabled = true;
    depthHoldEnabled = true;
    autoModeEnabled = true;
    resetPidState();
  } else {
    stabilizationEnabled = false;
    depthHoldEnabled = false;
    autoModeEnabled = false;
    stabilizationFaulted = false;
    resetPidState();
  }

  lastControlAt = millis();
  updateThrusters();
  String response = "{\"enabled\":";
  response += autoModeEnabled ? "true}" : "false}";
  server.send(200, "application/json", response);
}

void handleMotorSettings() {
  if (!requireAuthentication()) return;
  if (armed) {
    sendError(409, "DISARM sebelum mengubah parameter thruster.");
    return;
  }

  long motor = 0;
  long pin = 0;
  long pulseMin = 0;
  long pulseNeutral = 0;
  long pulseMax = 0;
  long reverse = 0;
  if (!readIntegerArg("motor", 0, THRUSTER_COUNT - 1, motor) ||
      !readIntegerArg("pin", 2, 28, pin) ||
      !readIntegerArg("min", 900, 1500, pulseMin) ||
      !readIntegerArg("neutral", 1400, 1600, pulseNeutral) ||
      !readIntegerArg("max", 1500, 2100, pulseMax) ||
      !readIntegerArg("reverse", 0, 1, reverse)) {
    sendError(400, "Parameter thruster tidak valid.");
    return;
  }

  Settings candidate = settings;
  const size_t index = static_cast<size_t>(motor);
  candidate.pins[index] = static_cast<uint8_t>(pin);
  candidate.pulseMin[index] = static_cast<uint16_t>(pulseMin);
  candidate.pulseNeutral[index] = static_cast<uint16_t>(pulseNeutral);
  candidate.pulseMax[index] = static_cast<uint16_t>(pulseMax);
  candidate.reverse[index] = static_cast<uint8_t>(reverse);
  for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
    String name = "mix" + String(axis);
    if (!readFloatArg(name, candidate.mix[index][axis])) {
      sendError(400, "Nilai mixer harus antara -1 dan 1.");
      return;
    }
  }
  bool motorSettingsValid =
      validPin(candidate.pins[index]) &&
      candidate.pulseMin[index] < candidate.pulseNeutral[index] &&
      candidate.pulseNeutral[index] < candidate.pulseMax[index];
  for (size_t axis = 0; axis < AXIS_COUNT; ++axis) {
    motorSettingsValid = motorSettingsValid &&
        isfinite(candidate.mix[index][axis]) &&
        candidate.mix[index][axis] >= -1.0f &&
        candidate.mix[index][axis] <= 1.0f;
  }
  if (!motorSettingsValid || !validateSettings(candidate)) {
    sendError(400, "Konfigurasi thruster tidak valid atau GPIO sudah digunakan.");
    return;
  }

  settings = candidate;
  server.send(200, "text/plain", "OK");
}

void handleSystemSettings() {
  if (!requireAuthentication()) return;
  if (armed) {
    sendError(409, "DISARM sebelum mengubah pengaturan sistem.");
    return;
  }
  long power = 0;
  long ramp = 0;
  long failsafe = 0;
  long stabilizationMaxOutput = 0;
  long stabilizationMaxAngle = 0;
  long stabilizationTargetRate = 0;
  long depthMaxOutput = 0;
  long depthDeadband = 0;
  long depthTarget = 0;
  float stabilizationKp = 0.0f;
  float stabilizationKi = 0.0f;
  float stabilizationKd = 0.0f;
  float depthKp = 0.0f;
  float depthKi = 0.0f;
  float depthKd = 0.0f;
  if (!readIntegerArg("power", 1, 100, power) ||
      !readIntegerArg("ramp", 5, 200, ramp) ||
      !readIntegerArg("failsafe", MIN_FAILSAFE_MS, MAX_FAILSAFE_MS, failsafe) ||
      !readBoundedFloatArg("stabilizationKp", 0.0f, 1.0f, stabilizationKp) ||
      !readBoundedFloatArg("stabilizationKi", 0.0f, 0.1f, stabilizationKi) ||
      !readBoundedFloatArg("stabilizationKd", 0.0f, 1.0f, stabilizationKd) ||
      !readIntegerArg("stabilizationMaxOutput", 5, 100,
                      stabilizationMaxOutput) ||
      !readIntegerArg("stabilizationMaxAngle", 5, 45,
                      stabilizationMaxAngle) ||
      !readIntegerArg("stabilizationTargetRate", 1, 60,
                      stabilizationTargetRate) ||
      !readBoundedFloatArg("depthKp", 0.0f, 2.0f, depthKp) ||
      !readBoundedFloatArg("depthKi", 0.0f, 0.5f, depthKi) ||
      !readBoundedFloatArg("depthKd", 0.0f, 1.0f, depthKd) ||
      !readIntegerArg("depthMaxOutput", 5, 100, depthMaxOutput) ||
      !readIntegerArg("depthDeadband", 0, 25, depthDeadband) ||
      !readIntegerArg("depthTarget", 0, 500, depthTarget)) {
    sendError(400, "Parameter daya, PID, batas output, target, atau failsafe tidak valid.");
    return;
  }

  Settings candidate = settings;
  candidate.maxPowerPercent = static_cast<uint8_t>(power);
  candidate.rampPercentPerSecond = static_cast<uint8_t>(ramp);
  candidate.failsafeMs = static_cast<uint16_t>(failsafe);
  candidate.stabilizationKp = stabilizationKp;
  candidate.stabilizationKi = stabilizationKi;
  candidate.stabilizationKd = stabilizationKd;
  candidate.stabilizationMaxOutputPercent =
      static_cast<uint8_t>(stabilizationMaxOutput);
  candidate.stabilizationMaxAngleDegrees =
      static_cast<uint8_t>(stabilizationMaxAngle);
  candidate.stabilizationTargetRateDps =
      static_cast<uint8_t>(stabilizationTargetRate);
  candidate.depthKp = depthKp;
  candidate.depthKi = depthKi;
  candidate.depthKd = depthKd;
  candidate.depthMaxOutputPercent = static_cast<uint8_t>(depthMaxOutput);
  candidate.depthDeadbandCm = static_cast<uint8_t>(depthDeadband);
  candidate.depthTargetCm = static_cast<uint16_t>(depthTarget);
  const String ssid = server.arg("ssid");
  const String wifiPassword = server.arg("wifiPass");
  const String webPassword = server.arg("webPass");
  if (ssid.length() > 32 ||
      (!webPassword.isEmpty() &&
       (webPassword.length() < MIN_WEB_PASSWORD_LENGTH || webPassword.length() > 32))) {
    sendError(400, "Nama hotspot atau password tidak valid.");
    return;
  }
  if (!ssid.isEmpty() &&
      !copyField(ssid, candidate.wifiSsid, sizeof(candidate.wifiSsid), false)) {
    sendError(400, "SSID tidak valid.");
    return;
  }
  if (!wifiPassword.isEmpty()) {
    sendError(400, "Hotspot ini dikonfigurasi tanpa password Wi-Fi.");
    return;
  }
  memset(candidate.wifiPassword, 0, sizeof(candidate.wifiPassword));
  if (!webPassword.isEmpty() &&
      !copyField(webPassword, candidate.webPassword, sizeof(candidate.webPassword), false)) {
    sendError(400, "Password web tidak valid.");
    return;
  }
  if (!validateSettings(candidate)) {
    sendError(400, "Pengaturan sistem tidak valid.");
    return;
  }
  settings = candidate;
  server.send(200, "text/plain", "OK");
}

void handleSaveSettings() {
  if (!requireAuthentication()) return;
  if (armed) {
    sendError(409, "DISARM sebelum menyimpan pengaturan.");
    return;
  }
  if (!validateSettings(settings) || !persistSettings()) {
    sendError(500, "Pengaturan tidak valid atau gagal ditulis ke flash.");
    return;
  }
  memcpy(activeWebPassword, settings.webPassword, sizeof(activeWebPassword));
  server.send(200, "text/plain", "Saved");
  delay(500);
  rp2040.reboot();
}

bool startAccessPoint() {
  WiFi.mode(WIFI_AP);

  if (setupMode) {
    setupApName = DEFAULT_HOTSPOT_NAME;
    setupApPassword = "";
  } else {
    setupApName = settings.wifiSsid;
    setupApPassword = settings.wifiPassword;
  }

  if (!WiFi.softAPConfig(ACCESS_POINT_IP, ACCESS_POINT_IP,
                         ACCESS_POINT_SUBNET)) {
    Serial.println("ERROR: gagal mengatur IP hotspot 192.168.4.1.");
    return false;
  }

  if (!WiFi.softAP(setupApName, setupApPassword)) {
    Serial.println("ERROR: gagal membuat hotspot. Periksa board dan catu daya.");
    return false;
  }
  accessPointReady = true;
  if (setupMode) {
    Serial.printf("Hotspot setup: SSID %s, password %s, alamat http://%s\n",
                  setupApName.c_str(), setupApPassword.c_str(),
                  WiFi.softAPIP().toString().c_str());
  } else {
    Serial.printf("Hotspot kontrol aktif: SSID %s, alamat http://%s\n",
                  setupApName.c_str(), WiFi.softAPIP().toString().c_str());
  }
  lastSetupMessageAt = millis();
  return true;
}

void startWebServer() {
  server.on("/", HTTP_GET, handleHome);
  server.on("/setup", HTTP_GET, handleSetupPage);
  server.on("/setup", HTTP_POST, handleSetupSave);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/thruster/control", HTTP_POST, handleIndividualThrusterControl);
  server.on("/api/sequence/start", HTTP_POST, handleTestSequenceStart);
  server.on("/api/sequence/heartbeat", HTTP_POST, handleTestSequenceHeartbeat);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/telemetry", HTTP_GET, handleTelemetry);
  server.on("/api/depth/zero", HTTP_POST, handleDepthZero);
  server.on("/api/stabilization", HTTP_POST, handleStabilization);
  server.on("/api/auto", HTTP_POST, handleAutoMode);
  server.on("/api/config", HTTP_GET, []() {
    if (!requireAuthentication()) return;
    sendConfiguration();
  });
  server.on("/api/motor", HTTP_POST, handleMotorSettings);
  server.on("/api/system", HTTP_POST, handleSystemSettings);
  server.on("/api/save", HTTP_POST, handleSaveSettings);
  server.onNotFound([]() { sendError(404, "Not found"); });
  server.begin();
  serverStarted = true;
  Serial.println("Web server aktif pada port 80.");
}

}  // namespace

void setup1() {
  while (!sensorsInitialized.load(std::memory_order_acquire)) {
    sleep_ms(1);
  }
}

void loop1() {
  ROVSensors::update();
  sleep_ms(5);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  const bool settingsLoaded = loadSettings();
  setupMode = !settingsLoaded;
  if (settingsLoaded) {
    memcpy(activeWebPassword, settings.webPassword, sizeof(activeWebPassword));
    Serial.println("Konfigurasi tersimpan dimuat.");
  } else {
    Serial.println("Belum ada konfigurasi valid; gunakan AP setup.");
  }

  ROVSensors::begin();
  sensorsInitialized.store(true, std::memory_order_release);
  outputsReady = attachThrusters();
  if (!outputsReady) {
    stopAllThrusters();
    Serial.println("ERROR: output thruster gagal diinisialisasi; kontrol dinonaktifkan.");
  }

  if (startAccessPoint()) {
    startWebServer();
  } else {
    Serial.println("ERROR: web server tidak dimulai karena hotspot tidak siap.");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void printSerialStatus() {
  Serial.printf("ROV Pico W | Hotspot: %s | IP: http://%s | Web: %s | Thruster: %s\n",
                setupApName.c_str(), WiFi.softAPIP().toString().c_str(),
                accessPointReady ? "siap" : "tidak tersedia",
                outputsReady ? "siap, DISARM" : "ERROR");
  if (setupMode) {
    Serial.println("Setup pertama | Hotspot terbuka tanpa password Wi-Fi.");
  }
  Serial.println("Serial Monitor: 115200 baud. Kontrol thruster hanya lewat halaman web.");
  lastStatusMessageAt = millis();
}

void loop() {
  if (serverStarted) {
    server.handleClient();
  }

  const uint32_t now = millis();
  const bool serialConnected = static_cast<bool>(Serial);
  if (serialConnected && !serialWasConnected) {
    printSerialStatus();
  }
  serialWasConnected = serialConnected;

  if (armed && now - lastControlAt > settings.failsafeMs) {
    Serial.println("Failsafe: heartbeat web terputus; semua thruster ke netral.");
    stopAllThrusters();
    updateThrusters();
  } else {
    updateTestSequence(now);
    if (now - lastOutputAt >= 20) {
      updateThrusters();
    }
  }
  if (setupMode && now - lastSetupMessageAt >= 10000) {
    lastSetupMessageAt = now;
    Serial.printf("Setup hotspot: SSID %s, password %s, buka http://%s\n",
                  setupApName.c_str(), setupApPassword.c_str(),
                  WiFi.softAPIP().toString().c_str());
  }
  if (!setupMode && serialConnected && now - lastStatusMessageAt >= 10000) {
    Serial.printf("Hotspot: %s | IP: http://%s | Thruster: %s\n",
                  setupApName.c_str(), WiFi.softAPIP().toString().c_str(),
                  armed ? "ARMED" : "DISARM");
    lastStatusMessageAt = now;
  }
  delay(1);
}
