#pragma once

// Wi-Fi credentials
#define WIFI_SSID     "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// MQTT broker — set this to your Raspberry Pi's local IP
#define MQTT_BROKER   "192.168.1.100"
#define MQTT_TOPIC    "home/sensors"

// NTP — UTC offset in seconds (e.g. -10800 for BRT/GMT-3)
#define NTP_UTC_OFFSET_SEC -10800