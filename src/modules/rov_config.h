#pragma once

#include <stddef.h>
#include <stdint.h>

namespace ROVConfig {

constexpr uint32_t CONFIG_MAGIC = 0x524F5657;
constexpr uint8_t CONFIG_VERSION = 4;
constexpr char DEFAULT_HOTSPOT_NAME[] = "SAUVAC";
constexpr char DEFAULT_WEB_USERNAME[] = "menang";
constexpr size_t MIN_WEB_PASSWORD_LENGTH = 6;
constexpr size_t THRUSTER_COUNT = 6;
constexpr size_t AXIS_COUNT = 6;

constexpr uint8_t DEFAULT_MAX_POWER_PERCENT = 15;
constexpr uint8_t DEFAULT_RAMP_PERCENT_PER_SECOND = 40;
constexpr uint16_t DEFAULT_FAILSAFE_MS = 600;
constexpr uint16_t MIN_FAILSAFE_MS = 250;
constexpr uint16_t MAX_FAILSAFE_MS = 2000;
constexpr float DEFAULT_STABILIZATION_KP = 0.02f;
constexpr float DEFAULT_STABILIZATION_KI = 0.0f;
constexpr float DEFAULT_STABILIZATION_KD = 0.008f;
constexpr uint8_t DEFAULT_STABILIZATION_MAX_OUTPUT_PERCENT = 25;
constexpr uint8_t DEFAULT_STABILIZATION_MAX_ANGLE_DEGREES = 35;
constexpr uint8_t DEFAULT_STABILIZATION_TARGET_RATE_DPS = 20;
constexpr float DEFAULT_DEPTH_KP = 0.04f;
constexpr float DEFAULT_DEPTH_KI = 0.0f;
constexpr float DEFAULT_DEPTH_KD = 0.015f;
constexpr uint8_t DEFAULT_DEPTH_MAX_OUTPUT_PERCENT = 40;
constexpr uint8_t DEFAULT_DEPTH_DEADBAND_CM = 2;
constexpr uint16_t DEFAULT_DEPTH_TARGET_CM = 0;
constexpr uint16_t DEFAULT_PULSE_MIN_US = 1000;
constexpr uint16_t DEFAULT_PULSE_NEUTRAL_US = 1500;
constexpr uint16_t DEFAULT_PULSE_MAX_US = 2000;

constexpr uint8_t I2C_SDA_PIN = 26;
constexpr uint8_t I2C_SCL_PIN = 27;
constexpr uint32_t I2C_CLOCK_HZ = 100000;
constexpr uint16_t SENSOR_UPDATE_INTERVAL_MS = 50;
constexpr uint8_t MPU6050_ADDRESS = 0x68;
constexpr uint8_t MPU6050_ALT_ADDRESS = 0x69;
constexpr uint8_t HMC5883L_ADDRESS = 0x1E;
constexpr uint8_t QMC5883L_ADDRESS = 0x0D;
constexpr uint8_t MS5803_ADDRESS_HIGH = 0x76;
constexpr uint8_t MS5803_ADDRESS_LOW = 0x77;
constexpr uint8_t MS5803_PRESSURE_4096_COMMAND = 0x48;
constexpr uint8_t MS5803_TEMPERATURE_4096_COMMAND = 0x58;
constexpr uint8_t MS5803_CONVERSION_TIME_MS = 10;
constexpr uint8_t VSYS_ADC_PIN = 29;
constexpr uint8_t VSYS_ADC_SAMPLE_COUNT = 8;
constexpr uint16_t VSYS_SAMPLE_INTERVAL_MS = 250;
constexpr float VSYS_ADC_REFERENCE_VOLTS = 3.3f;
constexpr float VSYS_DIVIDER_MULTIPLIER = 3.0f;

constexpr uint8_t DEFAULT_THRUSTER_PINS[THRUSTER_COUNT] = {2, 3, 4, 5, 6, 7};
constexpr float DEFAULT_MIX[THRUSTER_COUNT][AXIS_COUNT] = {
    {1.0f,  1.0f, 0.0f, -1.0f,  0.0f, 0.0f},
    {1.0f, -1.0f, 0.0f,  1.0f,  0.0f, 0.0f},
    {1.0f, -1.0f, 0.0f, -1.0f,  0.0f, 0.0f},
    {1.0f,  1.0f, 0.0f,  1.0f,  0.0f, 0.0f},
    {0.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f},
    {0.0f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f},
};

inline bool isThrusterPinAllowed(uint8_t pin) {
  return ((pin >= 2 && pin <= 22) || (pin >= 26 && pin <= 28)) &&
         pin != I2C_SDA_PIN && pin != I2C_SCL_PIN;
}

}  // namespace ROVConfig
