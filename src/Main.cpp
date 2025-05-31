#include "Logger.h"
#include "Config.h"
#include "Settings.h"
#include "EEPROM.h"
#include "NetManager.h"
#include "MessageCallbacks.h"

#include <Arduino.h>
#include <WTMsg.h>
#include <Wire.h>

EEPROM datastore(0x50);

struct MSGER
{
    WTId id;
    uint8_t rx;
    uint8_t tx;
};

void setup()
{
    Logger::Init();
#ifdef LOG_ENABLED
    Serial.println("------------------------------------");
#endif
    LOG_INFO("main", "Hallo! Wettorion v1 - Comms-Module");

    LOG_INFO("main", "Setting up the messengers...");
    MSGER msgers[] = MESSENGERS;
    WTReceivedCallback msgerCallbacks[] = MESSENGER_CALLBACKS;
    for (int i = 0; i < sizeof(msgers) / sizeof(MSGER); i++)
    {
        MSGER& msger = msgers[i];
        WTGlobal::CreateConnectionTo(msger.id, msger.rx, msger.tx);
        WTGlobal::SetOnReceive(msger.id, msgerCallbacks[i]);
        LOG_INFO("main", "Registered messenger: %#02x", msger.id);
    }

    Wire.begin();
    
    LOG_INFO("main", "Setting up the network...");
    if (!NetManager::ConnectWiFi())
    {
        LOG_FATAL("main", "Failed to set up the network!");
        return;
    }

    if (!NetManager::InitClient())
    {
        LOG_FATAL("main", "Failed to set up the network!");
        return;
    }
}

void loop()
{
    NetManager::Update();
}
