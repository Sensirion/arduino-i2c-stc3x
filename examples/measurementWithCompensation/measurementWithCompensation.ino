/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 *
 * Generator:     sensirion-driver-generator 0.38.1
 * Product:       stc3x
 * Model-Version: 1.0.0
 */
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

SensirionI2cStc3x stc3x_sensor;
SensirionI2cSht4x sht4x_sensor;

static char errorMessage[64];
static int16_t error;

void PrintUint64(uint64_t& value) {
    Serial.print("0x");
    Serial.print((uint32_t)(value >> 32), HEX);
    Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
}

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    Wire.begin();
    stc3x_sensor.begin(Wire, STC31_C_I2C_ADDR_29);
    sht4x_sensor.begin(Wire, SHT40_I2C_ADDR_44);

    uint32_t stc3xProductId = 0;
    uint64_t stc3xSerialNumber = 0;
    uint32_t sht4xSerialNumber = 0;

    // make sure sensor is ready after power up.
    delay(14);

    // Output SHT4x serial number
    error = sht4x_sensor.serialNumber(sht4xSerialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute serialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("SHT4x Seiral Number = ");
    Serial.print(sht4xSerialNumber);
    Serial.println();

    //
    // Output the product identifier and serial number
    //
    error = stc3x_sensor.getProductId(stc3xProductId, stc3xSerialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getProductId(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("STC3x Product id = ");
    Serial.print(stc3xProductId);
    Serial.println();
    Serial.print("STC3x Serial Number = ");
    PrintUint64(stc3xSerialNumber);
    Serial.println();

    //
    // Measure STC31-C CO2 in air in range 0% - 40%
    // or STC31 CO2 in air in range 0% - 25%
    //
    error = stc3x_sensor.setBinaryGas(19);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setBinaryGas(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
}

void loop() {

    float stc3xCo2Concentration = 0.0;
    float stc3xTemperature = 0.0;
    float sht4xTemperature = 0.0;
    float sht4xRelativeHumidity = 0.0;

    //
    // Slow down the sampling to 1Hz
    //
    delay(1000);

    //
    // Read humidity and temperature from external SHT4x sensor and use
    // it for compensation.
    error = sht4x_sensor.measureHighPrecision(sht4xTemperature,
                                              sht4xRelativeHumidity);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureLowestPrecision(): ");
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

    // Measure gas concentration
    error = stc3x_sensor.measureGasConcentration(stc3xCo2Concentration,
                                                 stc3xTemperature);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureGasConcentration(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    //
    // Print CO2 concentration in Vol% and temperature in degree celsius.
    //
    Serial.print("CO2_concentration:");
    Serial.print(stc3xCo2Concentration);
    Serial.print(",");
    Serial.print("Temperature:");
    Serial.print(stc3xTemperature);
    Serial.println();
}
