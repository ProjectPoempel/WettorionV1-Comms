#include "NetManager.h"
#include "Logger.h"
#include "PacketHandler.h"

#include <Arduino.h>
#include <WiFi.h>
#include <mutex>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define INFO(fmt, ...) LOG_INFO("netmanager", fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOG_WARN("netmanager", fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOG_ERROR("netmanager", fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) LOG_FATAL("netmanager", fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) LOG_DEBUG("netmanager", fmt, ##__VA_ARGS__)

typedef uint8_t NetResponse;

static WiFiClient client;
static std::mutex packetMutex;
static TaskHandle_t readTask = NULL;
static volatile bool reconnectScheduled = false;

static bool DoHandshake()
{
    DEBUG("Receiving handshake packet...");

    Optional<Packet> optPacket = NetManager::ReceivePacket();
    if (!optPacket)
    {
        ERROR("Failed to receive handshake packet!");
        return false;
    }

    Packet& packet = optPacket.getValue();
    String message = ReadStringFromPacket(&packet, 13);
    if (message != "Hello, world!")
    {
        ERROR("Received wrong handshake message: %s (expected: 'Hello, world!')");
        return false;
    }

    DEBUG("Sending handshake response...");

    Packet respPacket;
    WriteStringToPacket(&respPacket, "hi");
    NetManager::SendPacket(respPacket);

    return true;
}

static bool DoConnect()
{
    int curTry = 0;
    while (curTry < NET_CONNECT_TRIES)
    {
        INFO("Connection try #%d", curTry + 1);

        if (client.connect(NET_HOST, NET_PORT))
        {
            return DoHandshake();
        }

        curTry++;
    }

    return false;
}

static void SendResponse(NetResponse resp)
{
    client.write(resp);
}

static NetResponse ReceiveResponse()
{
    while (client.available() < 1) delay(1);
    return client.read();
}

static void RunReadTask(void* ignored)
{
    for (;;)
    {
        if (client.available() > 0)
        {
            Optional<Packet> optPkt = NetManager::ReceivePacket();
            if (!optPkt) continue;

            if (!PacketHandler::HandlePacket(optPkt.getValue()))
            {
                WARN("Failed to handle packet!");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

bool NetManager::ConnectWiFi()
{
    Serial.print("[netmanager/INFO]: Connecting WiFi...");

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    unsigned long ms = 0;
    while (millis() - start < WIFI_CONNECT_TIMEOUT * 1000 && WiFi.status() != WL_CONNECTED)
    {
        delay(1);

#ifdef LOG_ENABLED
        if (ms % 500 == 0)
        {
            Serial.print(".");
        }

        ms++;
#endif
    }

    unsigned long end = millis();

#ifdef LOG_ENABLED
        Serial.println();
#endif


    if (WiFi.status() != WL_CONNECTED)
    {
        ERROR("Failed to connect WiFi! (Status: %d)", WiFi.status());
        return false;
    }

    INFO("WiFi connected! (in %dms)", end - start);

    return true;
}

bool NetManager::InitClient()
{
    INFO("Connecting to backend... (%s:%d)", NET_HOST, NET_PORT);

    if (!DoConnect())
    {
        ERROR("Failed to connect to backend! (%d tries)", NET_CONNECT_TRIES);
        return false;
    }

    xTaskCreate(
        &RunReadTask,
        "NetReadTask",
        8192,
        NULL,
        1,
        &readTask
    );

    INFO("Connected to backend!");

    return true;
}

void NetManager::Disconnect()
{
    if (readTask)
    {
        vTaskDelete(readTask);
        readTask = NULL;
    }
    if (!client.connected()) return;
    client.stop();
}

void NetManager::Reconnect() 
{
    Disconnect();
    
    bool success;
    do
    {
        success = InitClient();
        if (!success)
        {
            ERROR("Failed to connect to backend! Retrying in %d seconds...", NET_RECONNECT_WAIT);
            delay(NET_RECONNECT_WAIT * 1000);
        }
    }
    while (!success);
}

bool NetManager::SendPacket(const Packet& packet)
{
    std::lock_guard<std::mutex> lock(packetMutex);
    while (client.available() > 1)
    {
        DEBUG("discarding: %#02x", client.read());
    }

    if (packet.offset == 0)
    {
        WARN("Tried to send a packet with no data.");
        return false;
    }

    uint16_t checksum = htons(ComputePacketChecksum(packet));
    uint16_t packetDataSize = htons(packet.offset);

    uint8_t buf[PKT_TOTAL_SIZE];
    buf[0] = PKT_START_BYTE;
    memcpy(&buf[1], (uint8_t*)&checksum, sizeof(checksum));
    memcpy(&buf[3], (uint8_t*)&packetDataSize, sizeof(packetDataSize));
    memcpy(&buf[5], packet.buf, packet.offset);

    for (uint16_t i = 0; i < 1000; i++)
    {
        if (client.write(buf, PKT_HEADER_SIZE + packet.offset) < 1)
        {
            if (!client.connected())
            {
                WARN("SendPacket(): Connection broke.");
            }
            else
            {
                WARN("Failed to send packet. (no bytes sent)");
            }
            
            return false;
        }

        if (ReceiveResponse() == PKT_RESP_SUCCESS) return true;
    }

    WARN("Failed to send packet. (timeout)");

    return false;
}

static Optional<Packet> _ReceivePacket(uint8_t _tries)
{
    _tries++;
    if (_tries >= 32) return {};

    while (client.available() < PKT_HEADER_SIZE)
    {
        delay(1);
    }
    
    uint8_t tries = 0;
    do
    {
        uint8_t startByte = 0;
        if (client.read(&startByte, sizeof(startByte)) < 1)
        {
            continue;
        }

        if (startByte == PKT_START_BYTE) break;
    }
    while (tries++ < 255);

    if (tries >= 254)
    {
        SendResponse(PKT_RESP_FAIL);
        return _ReceivePacket(_tries++);
    }

    uint16_t checksum;
    if (client.read((uint8_t*)&checksum, sizeof(checksum)) < 2)
    {
        SendResponse(PKT_RESP_FAIL);
        return _ReceivePacket(_tries++);
    }
    checksum = ntohs(checksum);

    uint16_t packetDataSize;
    if (client.read((uint8_t*)&packetDataSize, sizeof(packetDataSize)) < 2)
    {
        SendResponse(PKT_RESP_FAIL);
        return _ReceivePacket(_tries++);
    }
    packetDataSize = ntohs(packetDataSize);

    while (client.available() < packetDataSize)
    {
        delay(1);
    }

    uint8_t* buf = new uint8_t[packetDataSize];
    for (int i = 0; i < packetDataSize; i++)
    {
        while (client.available() < 1) delay(1);
        buf[i] = client.read();
    }

    SendResponse(PKT_RESP_SUCCESS);

    Packet packet;
    memcpy(packet.buf, buf, packetDataSize);
    delete[] buf;

    return packet;
}

Optional<Packet> NetManager::ReceivePacket()
{
    return _ReceivePacket(0);
}

void NetManager::ScheduleReconnect()
{
    reconnectScheduled = true;
}

void NetManager::Update()
{
    if (reconnectScheduled)
    {
        reconnectScheduled = false;
        Reconnect();
    }

    if (!reconnectScheduled)
    {
        if (!client.connected())
        {
            ERROR("Lost connection to backend! Reconnecting in %d seconds...", NET_RECONNECT_WAIT);
            Reconnect();
        }
    }
}
