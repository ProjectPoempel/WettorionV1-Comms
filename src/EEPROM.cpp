#include "EEPROM.h"
#include "Logger.h"

#include <Wire.h>

EEPROM::EEPROM(uint8_t address) : i2cAddress(address)
{
}

bool EEPROM::WriteByte(uint16_t address, uint8_t data)
{
    delay(5);

    Wire.beginTransmission(i2cAddress);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    Wire.write(data);
    int result = Wire.endTransmission();
    LOG_DEBUG("EEPROM", "WriteByte(): result of Wire.endTransmission(): %d", result);
    if (result != 0) return false;
}

bool EEPROM::Write(uint16_t address, uint8_t* data, uint8_t length)
{
    delay(5);

    Wire.beginTransmission(i2cAddress);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    Wire.write(data, length);
    int result = Wire.endTransmission();
    LOG_DEBUG("EEPROM", "Write(): result of Wire.endTransmission(): %d", result);
    if (result != 0) return false;
}

bool EEPROM::Read(uint16_t address, uint8_t* data, uint8_t length)
{
    delay(5);

    Wire.beginTransmission(i2cAddress);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    int result = Wire.endTransmission();
    LOG_DEBUG("EEPROM", "Read(): result of Wire.endTransmission(): %d", result);
    if (result != 0) return false;

    int received = Wire.requestFrom(i2cAddress, length);
    LOG_DEBUG("EEPROM", "Read(): result of Wire.requestFrom(): %d", received);

    uint16_t i = 0;
    while (Wire.available() > 0 && i < length)
    {
        data[i] = Wire.read();
        LOG_DEBUG("EEPROM", "Read(): byte #%d: %d", i, data[i]);
        i++;
    }

    return true;
}

uint8_t EEPROM::ReadByte(uint16_t address)
{
    delay(5);

    Wire.beginTransmission(i2cAddress);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
    int result = Wire.endTransmission();
    LOG_DEBUG("EEPROM", "ReadByte(): result of Wire.endTransmission(): %d", result);
    if (result != 0) return false;

    Wire.requestFrom(i2cAddress, 1);

    return Wire.read();
}
