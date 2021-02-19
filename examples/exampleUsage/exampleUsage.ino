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
#include <inttypes.h>

SensirionI2CStc3x stc3x;

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    Wire.begin();

    uint16_t error;
    char errorMessage[256];

    stc3x.begin(Wire);

    error = stc3x.prepareProductIdentifier();
    if (error) {
        Serial.print("Error trying to execute prepareProductIdentifier(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        uint32_t productNumber;
        uint8_t serialNumberRaw[32];
        uint8_t serialNumberSize = 32;
        error = stc3x.readProductIdentifier(productNumber, serialNumberRaw,
                                            serialNumberSize);
        if (error) {
            Serial.print("Error trying to execute readProductIdentifier(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        } else {
            uint64_t serialNumber = (uint64_t)serialNumberRaw[0] << 56 |
                                    (uint64_t)serialNumberRaw[1] << 48 |
                                    (uint64_t)serialNumberRaw[2] << 40 |
                                    (uint64_t)serialNumberRaw[3] << 32 |
                                    (uint64_t)serialNumberRaw[4] << 24 |
                                    (uint64_t)serialNumberRaw[5] << 16 |
                                    (uint64_t)serialNumberRaw[6] << 8 |
                                    (uint64_t)serialNumberRaw[7];
            Serial.print("ProductNumber: 0x");
            Serial.println(productNumber, HEX);
            char buffer[20];
            sprintf(buffer, "SerialNumber: %llu", serialNumber);
            Serial.println(buffer);
        }
    }

    uint16_t selfTestOutput;
    error = stc3x.selfTest(selfTestOutput);
    if (error) {
        Serial.print("Error trying to execute selfTest(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        char buffer[32];
        sprintf(buffer, "Self Test: 0x%04x (OK = 0x0000)", selfTestOutput);
        Serial.println(buffer);
    }

    error = stc3x.setBinaryGas(0x0001);
    if (error) {
        Serial.print("Error trying to execute selfTest(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.println("Set binary gas to 0x0001");
    }
}

void loop() {
    delay(1000);

    // Measure
    uint16_t gasTicks;
    uint16_t temperatureTicks;

    uint16_t error;
    char errorMessage[256];

    error = stc3x.measureGasConcentration(gasTicks, temperatureTicks);
    if (error) {
        Serial.print("Error trying to execute measureGasConcentration(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("GasConcentration:");
        Serial.print(100 * ((float)gasTicks - 16384.0) / 32768.0);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.println((float)temperatureTicks / 200.0);
    }
}
