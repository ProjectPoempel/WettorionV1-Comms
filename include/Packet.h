#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#define STRING String
#else
#include <cstdint>
#include <cstring>
#include <string>
#define STRING std::string
#endif

#define PKT_BUF_SIZE 1024
#define PKT_HEADER_SIZE 5
#define PKT_TOTAL_SIZE PKT_HEADER_SIZE + PKT_BUF_SIZE
#define PKT_START_BYTE 0x42

#define PKT_RESP_SUCCESS 0xFF
#define PKT_RESP_FAIL 0x69

#define PKT_FLIP(p) p.offset = 0

#define PKT_WRITEu8(p, v) WriteToPacket(&p, (uint8_t)v)
#define PKT_WRITEu16(p, v) WriteToPacket(&p, (uint16_t)v)
#define PKT_WRITEu32(p, v) WriteToPacket(&p, (uint32_t)v)
#define PKT_WRITEf(p, v) WriteToPacket(&p, (float)v)

#define PKT_READu8(p) (uint8_t)ReadFromPacket(&p, 0)
#define PKT_READu16(p) (uint16_t)ReadFromPacket(&p, 0)
#define PKT_READu32(p) (uint32_t)ReadFromPacket(&p, 0)
#define PKT_READf(p) (float)ReadFromPacket(&p, 0.0f)

enum PacketType : uint8_t
{
    PKT_NONE,
    PKT_PING,
    PKT_RECONNECT,
    PKT_SHUTDOWN,
    PKT_ACTION
};

enum ActionType : uint8_t
{
    ACT_DISCONNECT
};

struct Packet
{
    uint8_t buf[PKT_BUF_SIZE];
    uint16_t offset = 0;
};

template<typename T>
void WriteToPacket(Packet* packet, T value)
{
    if (packet->offset >= PKT_BUF_SIZE) return;
    if (packet->offset + sizeof(value) >= PKT_BUF_SIZE) return;
    memcpy(&packet->buf[packet->offset], (uint8_t*)&value, sizeof(value));
    packet->offset += sizeof(value);
}

template<typename T>
T ReadFromPacket(Packet* packet, T defaultValue)
{
    if (packet->offset >= PKT_BUF_SIZE) return defaultValue;
    if (packet->offset + sizeof(T) >= PKT_BUF_SIZE) return defaultValue;
    T value;
    memcpy((uint8_t*)&value, &packet->buf[packet->offset], sizeof(T));
    return value;
}

void WriteStringToPacket(Packet* packet, const STRING& value);
STRING ReadStringFromPacket(Packet* packet, size_t size, const STRING& defaultValue = "");
uint16_t ComputePacketChecksum(const Packet& packet);