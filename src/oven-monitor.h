#ifndef __OVEN_MONITOR_H_
#define __OVEN_MONITOR_H_

#include "pins.h"
#include <Arduino.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifndef NAME
#define NAME "NEW-oven-monitor"
#endif

#ifndef TOPIC
#define TOPIC "program-me/NEW-oven-monitor"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "something"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "stuff"
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER "xxx"
#endif

#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif


#endif
