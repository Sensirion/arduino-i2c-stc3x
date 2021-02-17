/*
 * I2C-Generator: 0.2.0
 * Yaml Version: 0.1.1
 * Template Version: 0.6.0
 */
/*
 * Copyright (c) 2021, Sensirion AG
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
#include <SensirionI2CStc3x.h>
#include <Wire.h>

SensirionI2CStc3x stc3x;

// TODO: DRIVER_GENERATOR Add missing commands and make printout more pretty

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    Wire.begin();

    uint16_t error;
    char errorMessage[256];

    stc3x.begin(Wire);

    uint32_t productNumber;
    uint8_t serialNumber[8];
    uint8_t serialNumberSize = 8;

    error = stc3x.readProductIdentifier(productNumber, serialNumber,
                                        serialNumberSize);

    if (error) {
        Serial.print("Error trying to execute readProductIdentifier(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("ProductNumber:");
        Serial.print(productNumber);
        Serial.print("\t");
        Serial.print("SerialNumber:");
        for (size_t i = 0; i < serialNumberSize; i++) {
            Serial.print(serialNumber[i]);
            Serial.print(", ");
        }
        Serial.println();
    }

    uint16_t selfTestOutput;

    error = stc3x.selfTest(selfTestOutput);

    if (error) {
        Serial.print("Error trying to execute selfTest(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SelfTestOutput:");
        Serial.println(selfTestOutput);
    }

    // Start Measurement
}

void loop() {
    uint16_t error;
    char errorMessage[256];

    // TODO: DRIVER_GENERATOR Adjust measurement delay
    delay(1000);
    // TODO: DRIVER_GENERATOR Add scale and offset to printed measurement values
    // Read Measurement

    uint16_t gasTicks;
    uint16_t temperatureTicks;

    error = stc3x.measureGasConcentration(gasTicks, temperatureTicks);

    if (error) {
        Serial.print("Error trying to execute measureGasConcentration(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("GasTicks:");
        Serial.print(gasTicks);
        Serial.print("\t");
        Serial.print("TemperatureTicks:");
        Serial.println(temperatureTicks);
    }
}
