#pragma once

#include <cstdint>
#include <utility>

typedef uint8_t SettingKey;

enum class SettingType
{
    U8,
    U16,
    U32,
    FLOAT
};

struct Setting
{
    SettingType type;
    uint8_t u8 = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    float f = 0.0f;
};

class Settings
{
public:
    static bool SetSetting(SettingKey key, const Setting& setting, bool overwrite = true);
    static std::pair<bool, Setting> GetSetting(SettingKey key);
};