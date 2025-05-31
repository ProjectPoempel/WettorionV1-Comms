#pragma once

#include "Logger.h"
#include "Config.h"
#include "Packet.h"

class PacketHandler
{
public:
    static bool HandlePacket(Packet& packet);
};