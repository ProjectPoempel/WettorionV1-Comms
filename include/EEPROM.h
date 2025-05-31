#pragma once

#include <Arduino.h>

class EEPROM
{
public:
    EEPROM(uint8_t address);

    bool WriteByte(uint16_t address, uint8_t data);
    bool Write(uint16_t address, uint8_t* data, uint8_t length);
    bool Read(uint16_t address, uint8_t* data, uint8_t length);
    uint8_t ReadByte(uint16_t address);
private:
    uint8_t i2cAddress;
};