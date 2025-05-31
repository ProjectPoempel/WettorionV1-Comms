#include "Settings.h"
#include "EEPROM.h"
#include "Config.h"
#include "Logger.h"

#pragma pack(push, 1)

extern EEPROM datastore;

struct RegistryEntry
{
    SettingKey key;
    SettingType type;
};

static RegistryEntry _registry[SETTINGS_REGISTRY_SIZE];
static int _registryLen = -1;

static void LoadRegistry()
{
    _registryLen = datastore.ReadByte(SETTINGS_REGISTRY_ADDRESS);
    datastore.Read(SETTINGS_REGISTRY_ADDRESS + 1, (uint8_t*)_registry, SETTINGS_REGISTRY_SIZE);
}

static void SaveRegistry()
{
    datastore.WriteByte(SETTINGS_REGISTRY_ADDRESS, (uint8_t)_registryLen);
    datastore.Write(SETTINGS_REGISTRY_ADDRESS + 1, (uint8_t*)_registry, SETTINGS_REGISTRY_SIZE);
}

static bool SettingExists(SettingKey key)
{
    for (int i = 0; i < _registryLen; i++)
    {
        if (_registry[i].key == key) return true;
    }

    return false;
}

static std::pair<RegistryEntry, int> GetEntry(SettingKey key)
{
    for (int i = 0; i < _registryLen; i++)
    {
        if (_registry[i].key == key) return { _registry[i], i };
    }
    
    return std::make_pair(RegistryEntry(), 0);
}

bool Settings::SetSetting(SettingKey key, const Setting& setting, bool overwrite)
{
    if (_registryLen == -1) LoadRegistry();

    bool exists = SettingExists(key);
    if (!overwrite && exists) return false;

    auto entry = GetEntry(key);
    if (!exists)
    {
        _registry[_registryLen] = { key, setting.type };
        _registryLen++;
        SaveRegistry();

        RegistryEntry e{};
        e.key = key;
        e.type = setting.type;
        entry = std::make_pair(e, _registryLen - 1);
    }

    uint16_t addr = SETTINGS_VALUES_ADDRESS + entry.second * SETTINGS_VALUE_SIZE;

    switch (setting.type)
    {
    case SettingType::U8:
    {
        if (!datastore.WriteByte(addr, setting.u8)) return false;
        break;
    }
    case SettingType::U16:
    {
        if (!datastore.Write(addr, (uint8_t*)&setting.u16, sizeof(setting.u16))) return false;
        break;
    }
    case SettingType::U32:
    {
        if (!datastore.Write(addr, (uint8_t*)&setting.u32, sizeof(setting.u32))) return false;
        break;
    }
    case SettingType::FLOAT:
    {
        if (!datastore.Write(addr, (uint8_t*)&setting.f, sizeof(setting.f))) return false;
        break;
    }
    }

    return true;
}

std::pair<bool, Setting> Settings::GetSetting(SettingKey key)
{
    if (_registryLen == -1) LoadRegistry();

    bool exists = SettingExists(key);
    if (!exists) return { false, {} };

    auto entry = GetEntry(key);
    uint16_t addr = SETTINGS_VALUES_ADDRESS + entry.second * SETTINGS_VALUE_SIZE;

    Setting setting{};
    setting.type = entry.first.type;

    switch (setting.type)
    {
    case SettingType::U8:
    {
        setting.u8 = datastore.ReadByte(addr);
        break;
    }
    case SettingType::U16:
    {
        if (!datastore.Read(addr, (uint8_t*)&setting.u16, sizeof(setting.u16))) return { false, {} };
        break;
    }
    case SettingType::U32:
    {
        if (!datastore.Read(addr, (uint8_t*)&setting.u32, sizeof(setting.u32))) return { false, {} };
        break;
    }
    case SettingType::FLOAT:
    {
        if (!datastore.Read(addr, (uint8_t*)&setting.f, sizeof(setting.f))) return { false, {} };
        break;
    }
    }

    return { true, setting };
}
