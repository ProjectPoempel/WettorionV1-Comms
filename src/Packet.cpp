#include "Packet.h"

void WriteStringToPacket(Packet* packet, const STRING& value)
{
    if (packet->offset >= PKT_BUF_SIZE) return;
    if (packet->offset + value.length() >= PKT_BUF_SIZE) return;
    memcpy(&packet->buf[packet->offset], value.c_str(), value.length());
    packet->offset += value.length();
}

STRING ReadStringFromPacket(Packet* packet, size_t size, const STRING& defaultValue)
{
    if (packet->offset >= PKT_BUF_SIZE) return defaultValue;
    if (packet->offset + size >= PKT_BUF_SIZE) return defaultValue;

    char* buf = new char[size + 1];
    memcpy(buf, &packet->buf[packet->offset], size);
    buf[size] = 0;

    STRING copy = buf;
    delete[] buf;
    packet->offset += size;

    return copy;
}

uint16_t ComputePacketChecksum(const Packet& packet)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    for (int i = 0; i < packet.offset; i++) {
        uint8_t byte = packet.buf[i];
        sum1 = (sum1 + byte) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}