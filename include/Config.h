// Wettorion v1 - Comms-Modul Konfigurations-Datei

// Pins
// PIN_I2C_SDA 21
// PIN_I2C_SCK 22

// Messengers
// This: WT_ID_CONTROLLER_COMMS
// ID, RX, TX
#define MESSENGERS { { WT_ID_CONTROLLER_DATA, 0, 0 } }
#define MESSENGER_CALLBACKS { &OnMessageReceived_Data }

// Logging
#define LOG_ENABLED // Auskommentieren, um Logs zu deaktivieren
#define LOG_DEBUG_ENABLED // Auskommentieren, um Debug-Logging zu deaktivieren

// Network
#define WIFI_CONNECT_TIMEOUT 15 // Sekunden
#define WIFI_SSID "Anasko"
#define WIFI_PASSWORD "LOa7K8f2(!)Osdfgwd165491zgjkjJZLZUJk95646"
#define NET_HOST "192.168.178.48"
#define NET_PORT 8086
#define NET_CONNECT_TRIES 5
#define NET_RECONNECT_WAIT 15 // Sekunden

// Misc
#define SETTINGS_REGISTRY_SIZE 24
#define SETTINGS_REGISTRY_ADDRESS 0x00 // Registry = 64 bytes groß
#define SETTINGS_VALUE_SIZE 64
#define SETTINGS_VALUES_ADDRESS 0x41 // 64 + 1