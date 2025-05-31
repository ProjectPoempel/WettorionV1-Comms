#include "PacketHandler.h"
#include "NetManager.h"

bool PacketHandler::HandlePacket(Packet& packet)
{
    PacketType type = (PacketType)PKT_READu8(packet);
    LOG_DEBUG("packethandler", "Packet received! (type: %d)", (int)type);

    switch (type)
    {
    case PacketType::PKT_PING:
    {
        Packet pkt;
        PKT_WRITEu8(pkt, PacketType::PKT_PING);
        NetManager::SendPacket(pkt);
        break;
    }
    case PacketType::PKT_RECONNECT:
    {
        LOG_INFO("packethandler", "Server sent reconnect request! Reconnecting...");
        NetManager::ScheduleReconnect();
        break;
    }
    case PacketType::PKT_SHUTDOWN:
    {
        LOG_WARN("packethandler", "Server sent shutdown packet! Attempting reconnect in %d seconds...", NET_RECONNECT_WAIT);
        delay(NET_RECONNECT_WAIT * 1000);
        NetManager::ScheduleReconnect();
        break;
    }
    default:
        LOG_WARN("packethandler", "Received unknown packet type: %d", (int)type);
        return false;
    }

    return true;
}
  