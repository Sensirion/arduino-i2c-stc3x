/*
 * Copyright (c) 2024, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For this example you need to install the SHT4x library
 * 'Sensirion I2C SHT4X'.
 * Please find all relevant instructions here:
 * https://github.com/Sensirion/arduino-i2c-sht4x
 */

#include <Arduino.h>
#include <SensirionI2cSht4x.h>
#include <SensirionI2cStc3x.h>
#include <Wire.h>

// macro definitions
// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

// Measurement configuration
// The measurement delay in ms. Default: 1Hz.
// Don't set this below 140ms (~7Hz)
#define MEASUREMENT_DELAY_MS 1000
// measure  for 20 seconds to stabilize signal
#define STABILIZATION_DURATION_MS 20000
// Measure STC31-C CO2 in air in range 0% - 40%
#define BINARY_GAS 0x13
#define ENABLE_WEAK_FILTER 1
#define ENABLE_STRONG_FILTER 0
// The reference raw CO2 vol% concentration.
// default to 0.0 vol%
static float frcReferenceValue = 0.0;

// Calculate actual measurement duration considering
// the measurement duration of the sensor.
// Gas Models >= 0x10 are low-cross-sensitivity
#if BINARY_GAS >= 0x10
#define ACTUAL_MEASUREMENT_DELAY_MS \
    (MEASUREMENT_DELAY_MS - STC31_MEASUREMENT_DELAY_HIGH)
#else
#define ACTUAL_MEASUREMENT_DELAY_MS \
    (MEASUREMENT_DELAY_MS - STC31_MEASUREMENT_DELAY_LOW)
#endif

// Ensure actual measurement duration is positiv
#if ACTUAL_MEASUREMENT_DELAY_MS < 0
#define ACTUAL_MEASUREMENT_DELAY_MS 1
#endif

SensirionI2cStc3x stc3x_sensor;
SensirionI2cSht4x sht4x_sensor;

static float stc3xCo2Concentration = 0.0;
static float stc3xTemperature = 0.0;
static float sht4xTemperature = 0.0;
static float sht4xRelativeHumidity = 0.0;

static char errorMessage[64];
static int16_t error = NO_ERROR;

#define STC3X_STATE_SIZE 30
static uint8_t stc3xState[STC3X_STATE_SIZE];

void PrintUint64(uint64_t& value) {
    Serial.print("0x");
    Serial.print((uint32_t)(value >> 32), HEX);
    Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
}

/**
 * @brief Converts a gas concentration in vol% to the raw
 *        gas concentration
 *
 * @param gasConcentration gas concentration in vol%
 * @return uint16_t the raw gas concentration
 */
uint16_t signalRawGasConcentration(float gasConcentration) {
    return (uint16_t)gasConcentration * 327.68 + 16384.0;
}

/**
 * Perform soft reset for all sensor connected to I2C Bus.
 */
void i2cSoftReset() {
    Wire.beginTransmission(0x00);
    size_t writtenBytes = Wire.write(0x06);
    uint8_t i2c_error = Wire.endTransmission();
}

/**
 * @brief measure SHT4x and STC3x sensor and print the measured
 *        values to the serial console.
 */
void measureAndPrint() {
    //
    // Read humidity and temperature from external SHT4x sensor and use
    // it for compensation.
    error = sht4x_sensor.measureHighPrecision(sht4xTemperature,
                                              sht4xRelativeHumidity);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureHighPrecision(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    error = stc3x_sensor.setRelativeHumidity(sht4xRelativeHumidity);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setRelativeHumidity(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    error = stc3x_sensor.setTemperature(sht4xTemperature);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setTemperature(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // If you have a pressure sensor or constant pressure
    // in your environment you should also set the absolute pressure
    // to achieve the best sensor performance.
    /*
    stc3x_sensor.setPressure(absoluePressure);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setPressure(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    */

    // Measure gas concentration
    error = stc3x_sensor.measureGasConcentration(stc3xCo2Concentration,
                                                 stc3xTemperature);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureGasConcentration(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    Serial.print("CO2_concentration:");
    Serial.print(stc3xCo2Concentration);
    Serial.print(",");
    Serial.print("Temperature_STC3x:");
    Serial.print(stc3xTemperature);
    Serial.print(",");
    Serial.print("Temperature_SHT4x:");
    Serial.print(sht4xTemperature);
    Serial.print(",");
    Serial.print("Temperature_Delta:");
    Serial.print(stc3xTemperature - sht4xTemperature);
    Serial.println();
}

/**
 * @brief run concentration measurement for FRC. If low cross-sensitivity
 *        measurement is selected, additional measurements are taken
 *        for stabilization of the gas concentration
 */
void measureAndStabilizeReadings() {
    measureAndPrint();
    int measurement_loops =
        (int)(STABILIZATION_DURATION_MS / MEASUREMENT_DELAY_MS);
    // Measure for 20 seconds with 1Hz to stabilize the signal.
    // The measurement depends on
    for (int ix = 0; ix < measurement_loops; ix++) {

        delay(ACTUAL_MEASUREMENT_DELAY_MS);
        measureAndPrint();
    }
}

void configureFilter() {
#if ENABLE_WEAK_FILTER
    error = stc3x_sensor.enableWeakFilter();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute enableWeakFilter(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
#endif
#if ENABLE_STRONG_FILTER
    // Enable stronger smoothing filter.
    error = stc3x_sensor.enableStrongFilter();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute enableStrongFilter(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
#endif
}

/**
 * @brief This method executes the first loop of the
 *        post-soldering forced recalibration sequence.
 */
void firstFrcLoop() {
    Serial.println("========== 1st FRC loop ==========");
    // Step 1:
    // Enable automatic self-calibration (ASC)
    // If ASC is not used in the application, it must not be enabled.
    error = stc3x_sensor.enableAutomaticSelfCalibration();
    if (error != NO_ERROR) {
        Serial.print(
            "Error trying to execute enableAutomaticSelfCalibration(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // Step 2:
    // Set binary gas.
    error = stc3x_sensor.setBinaryGas(BINARY_GAS);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setBinaryGas(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // Step 3 (optional):
    // Enable weak filter. Setting the filter can be omitted for the
    // low-noise mode (binary gas: 0x0 - 0x3) but is necessary for the
    //  low-cross-sensitivity mode (binary gas: 0x10 - 0x13).
    configureFilter();

    // Step 4:
    // Measure and stabilize readings by running measurements at
    // 1Hz for 20 seconds.
    measureAndStabilizeReadings();

    // Step 5: Apply FRC
    uint16_t frcReferenceValueRaw =
        signalRawGasConcentration(frcReferenceValue);
    error = stc3x_sensor.forcedRecalibration(frcReferenceValueRaw);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute forcedRecalibration(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("Set forced recalibration to ");
    Serial.print(frcReferenceValue);
    Serial.println(" vol%.");

    // Step 6:
    // Prepare and read state and store it in memory
    // of the Arduino.
    error = stc3x_sensor.getSensorState(stc3xState, STC3X_STATE_SIZE);

    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getSensorState(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // Step 7:
    // Soft reset. Ends 1st loop.
    i2cSoftReset();

    // make sure sensor is ready after soft reset.
    delay(14);

    Serial.println("========== END 1st FRC loop ==========");
}

void secondFrcLoop() {
    Serial.println("========== START 2nd FRC loop ==========");
    // Step 1: Write and apply state as retrieved by FRC loop 1
    stc3x_sensor.setSensorState(stc3xState, STC3X_STATE_SIZE);

    // Step 2 (optional):
    // Enable weak or strong filter. Setting the filter can be omitted
    // for the low-noise mode (binary gas: 0x0 - 0x3) but is necessary for the
    // low-cross-sensitivity mode (binray gas: 0x10 - 0x13).
    configureFilter();

    // Step 3: Measure STC3x sensor to stabilize the signal
    measureAndStabilizeReadings();

    // Take an additional measurement and verify that the FRC
    // was successful by checking if the measured signal is
    // within specification of the expected CO2 concentration
    // in vol%.
    measureAndPrint();
    // achievable accuracy needs to be determined for the application
    // we allow +- 1vol%.
    float margin = 1.0;

    // Test if measured CO2 concentration is within expected range.
    if (stc3xCo2Concentration >= (frcReferenceValue - margin) &&
        stc3xCo2Concentration <= (frcReferenceValue + margin)) {
        Serial.println("STC3x forced recalibration successful.");
    } else {
        Serial.println("STC3x measurement not in range. Please repeat FRC.");
    }
    Serial.println("========== END 2nd FRC loop ==========");
}

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    Wire.begin();

    // Initialize STC31-C and SHT4x sensors
    stc3x_sensor.begin(Wire, STC31_C_I2C_ADDR_29);
    sht4x_sensor.begin(Wire, SHT40_I2C_ADDR_44);

    // Make sure STC31-C sensor is in the proper state
    // to initiate the FRC procedure.
    i2cSoftReset();

    // make sure sensor is ready after soft reset.
    delay(14);

    // run first loop of the forced recalibration
    firstFrcLoop();

    // run second loop of the forced recalibraion
    secondFrcLoop();
}

void loop() {

    //
    // Measure with 1Hz
    //
    delay(ACTUAL_MEASUREMENT_DELAY_MS);

    // Measures the SHT4x and STC31-C sensor
    // and prints the measured values to the
    // serial console.
    measureAndPrint();
}
