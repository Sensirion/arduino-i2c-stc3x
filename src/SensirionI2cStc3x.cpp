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

#include "SensirionI2cStc3x.h"
#include <Arduino.h>

// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

static uint8_t communication_buffer[47] = {0};

SensirionI2cStc3x::SensirionI2cStc3x() {
}

float SensirionI2cStc3x::signalTemperature(uint16_t rawTemperature) {
    float temperature = 0.0;
    temperature = rawTemperature / 200.0;
    return temperature;
}

float SensirionI2cStc3x::signalGasConcentration(uint16_t rawGasConcentration) {
    float gasConcentration = 0.0;
    gasConcentration = (100.0 * (rawGasConcentration - 16384.0)) / 32768.0;
    return gasConcentration;
}

int16_t SensirionI2cStc3x::setRelativeHumidity(float relativeHumidity) {
    uint16_t rhTicks = 0;
    int16_t localError = 0;
    rhTicks = (uint16_t)((relativeHumidity * 65535.0) / 100.0);
    localError = setRelativeHumidityRaw(rhTicks);
    return localError;
}

int16_t SensirionI2cStc3x::setTemperature(float temperature) {
    uint16_t temperatureTicks = 0;
    int16_t localError = 0;
    temperatureTicks = (uint16_t)(temperature * 200.0);
    localError = setTemperatureRaw(temperatureTicks);
    return localError;
}

int16_t SensirionI2cStc3x::measureGasConcentration(float& co2Concentration,
                                                   float& temperature) {
    uint16_t rawGasConcentration = 0;
    uint16_t rawTemperature = 0;
    int16_t localError = 0;
    localError =
        measureGasConcentrationRaw(rawGasConcentration, rawTemperature);
    if (localError != NO_ERROR) {
        return localError;
    }
    co2Concentration =
        SensirionI2cStc3x::signalGasConcentration(rawGasConcentration);
    temperature = SensirionI2cStc3x::signalTemperature(rawTemperature);
    return localError;
}

int16_t SensirionI2cStc3x::getSensorState(uint8_t state[], uint16_t stateSize) {
    int16_t localError = 0;
    localError = prepareReadState();
    if (localError != NO_ERROR) {
        return localError;
    }
    localError = readSensorState(state, stateSize);
    return localError;
}

int16_t SensirionI2cStc3x::setSensorState(const uint8_t sensorState[],
                                          uint16_t sensorStateSize) {
    int16_t localError = 0;
    localError = writeSensorState(sensorState, sensorStateSize);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError = applyState();
    return localError;
}

int16_t SensirionI2cStc3x::getProductId(uint32_t& productId,
                                        uint64_t& serialNumber) {
    uint32_t productNumber = 0;
    uint32_t serialNumberHigh = 0;
    uint32_t serialNumberLow = 0;
    int16_t localError = 0;
    localError = prepareProductIdentifier();
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        readProductIdentifier(productNumber, serialNumberHigh, serialNumberLow);
    if (localError != NO_ERROR) {
        return localError;
    }
    productId = productNumber;
    serialNumber = ((uint64_t)(serialNumberHigh)*4294967296) + serialNumberLow;
    return localError;
}

int16_t SensirionI2cStc3x::setBinaryGas(uint16_t binaryGas) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3615, buffer_ptr, 5);
    localError |= txFrame.addUInt16(binaryGas);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    if (binaryGas < 0x10) {
        _measurement_delay = STC31_MEASUREMENT_DELAY_LOW;
    } else {
        _measurement_delay = STC31_MEASUREMENT_DELAY_HIGH;
    }
    return localError;
}

int16_t
SensirionI2cStc3x::setRelativeHumidityRaw(uint16_t relativeHumidityTicks) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3624, buffer_ptr, 5);
    localError |= txFrame.addUInt16(relativeHumidityTicks);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::setTemperatureRaw(uint16_t temperatureTicks) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x361e, buffer_ptr, 5);
    localError |= txFrame.addUInt16(temperatureTicks);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::setPressure(uint16_t absoluePressure) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x362f, buffer_ptr, 5);
    localError |= txFrame.addUInt16(absoluePressure);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t
SensirionI2cStc3x::measureGasConcentrationRaw(uint16_t& gasTicks,
                                              uint16_t& temperatureTicks) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3639, buffer_ptr, 6);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(_measurement_delay);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 6);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 6,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(gasTicks);
    localError |= rxFrame.getUInt16(temperatureTicks);
    return localError;
}

int16_t
SensirionI2cStc3x::forcedRecalibration(uint16_t referenceConcentration) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3661, buffer_ptr, 5);
    localError |= txFrame.addUInt16(referenceConcentration);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(_measurement_delay);
    return localError;
}

int16_t SensirionI2cStc3x::enableAutomaticSelfCalibration() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3fef, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::disableAutomaticSelfCalibration() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3f6e, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::prepareReadState() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3752, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::readSensorState(uint8_t state[],
                                           uint16_t stateSize) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe133, buffer_ptr, 45);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    SensirionI2CRxFrame rxFrame(buffer_ptr, 45);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 45,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getBytes((uint8_t*)state, stateSize);
    return localError;
}

int16_t SensirionI2cStc3x::writeSensorState(const uint8_t state[],
                                            uint16_t stateSize) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe133, buffer_ptr, 47);
    localError |= txFrame.addBytes(state, stateSize);
    if (localError != NO_ERROR) {
        return localError;
    }

    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cStc3x::applyState() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3650, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::selfTest(STC3xTestResultT& selfTestOutput) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x365b, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(22);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(selfTestOutput.value);
    return localError;
}

int16_t SensirionI2cStc3x::prepareProductIdentifier() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x367c, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cStc3x::readProductIdentifier(uint32_t& productNumber,
                                                 uint32_t& serialNumberHigh,
                                                 uint32_t& serialNumberLow) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe102, buffer_ptr, 18);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(10);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 18);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 18,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt32(productNumber);
    localError |= rxFrame.getUInt32(serialNumberHigh);
    localError |= rxFrame.getUInt32(serialNumberLow);
    return localError;
}

int16_t SensirionI2cStc3x::enterSleepMode() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3677, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::exitSleepMode() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x0, buffer_ptr, 2);
    SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    delay(12);
    return localError;
}

int16_t SensirionI2cStc3x::enableWeakFilter() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3fc8, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::disableWeakFilter() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3f49, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::enableStrongFilter() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3fd5, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cStc3x::disableStrongFilter() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3f54, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

void SensirionI2cStc3x::begin(TwoWire& i2cBus, uint8_t i2cAddress) {
    _i2cBus = &i2cBus;
    _i2cAddress = i2cAddress;
}
