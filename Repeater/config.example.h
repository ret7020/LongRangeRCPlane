#ifndef CONFIG_H

#define CONFIG_H

// NRF Settings
#define NRF_CE D1
#define NRF_CSN D8
#define NRF_PACKET_ITEMS 4
#define NRF_CHANNEL 5
#define NRF_SPEED RF24_1MBPS
#define NRF_POWER RF24_PA_LOW
#define NRF_ADDRESS 0x1234567890LL

// Lora Settings
#define LORA_RX D3
#define LORA_TX D2
#define LORA_CONTROL D0

// WiFi Settings + WEB debug interface
//#define WEB_DEBUG true
#define WIFI_SSID "NETWORK"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME "HOSTNAME"
#define WEB_HTTP_PORT 80

#define UART_DEBUG
#endif
