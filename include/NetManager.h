#pragma once

#include "Packet.h"
#include "Optional.h"

class NetManager
{
public:
    static bool ConnectWiFi();
    static bool InitClient();
    static void Disconnect();
    static void Reconnect();
    static bool SendPacket(const Packet& packet);
    static Optional<Packet> ReceivePacket();
    static void ScheduleReconnect();
    static void Update();
};