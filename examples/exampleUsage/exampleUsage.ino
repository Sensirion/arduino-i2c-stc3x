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
#include <Arduino.h>
#include <SensirionI2cStc3x.h>
#include <Wire.h>

// macro definitions
// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

SensirionI2cStc3x sensor;

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
    sensor.begin(Wire, STC31_C_I2C_ADDR_29);

    uint32_t productId = 0;
    uint64_t serialNumber = 0;
    delay(14);
    //
    // Output the product identifier and serial number
    //
    error = sensor.getProductId(productId, serialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getProductId(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("Product id = ");
    Serial.print(productId);
    Serial.println();
    Serial.print("Serial Number = ");
    PrintUint64(serialNumber);
    Serial.println();
    //
    // Measure STC31-C CO2 in air in range 0% - 40%
    // or STC31 CO2 in air in range 0% - 25%
    //
    error = sensor.setBinaryGas(19);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setBinaryGas(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    //
    // Enable weak smoothing filter for measurement.
    // This will decrease the signal noise but increase response time.
    //
    error = sensor.enableWeakFilter();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute enableWeakFilter(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
}

void loop() {

    float co2Concentration = 0.0;
    float temperature = 0.0;
    //
    // Slow down the sampling to 1Hz
    //
    delay(1000);
    //
    // Set default humidity of 50%. Adjust the value to your environment or
    // use the actual humidity read out from the SHT4x sensor on the STC31-C
    // evaluation kit.
    //
    error = sensor.setRelativeHumidity(50.0);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute setRelativeHumidity(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    error = sensor.measureGasConcentration(co2Concentration, temperature);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureGasConcentration(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    //
    // Print CO2 concentration in Vol% and temperature in degree celsius.
    //
    Serial.print("CO2 concentration = ");
    Serial.print(co2Concentration);
    Serial.println();
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println();
}
