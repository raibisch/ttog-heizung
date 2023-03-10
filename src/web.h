#ifndef TTGO_WEB_H
#define TTGO_WEB_H

#include "io.h"
#include "var_dev.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ESP32Ping.h>

#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

//z.Z. nicht genutzt
//#include <PubSubClient.h>
#include "SPIFFS.h"

// Eigene Libs
#include "AsyncWebLog.h"
#include "AsyncWebOTA.h"

static AsyncWebServer server(80);

// SSE Create an Event Source on /events

extern AsyncEventSource events;
// zZ. nicht genutzt
//#define MQTT_SERVER "192.168.2.22"
//extern void mqtt_client_init();

void server_init();
extern String httpGETRequest(String url);

//const char* PARAM_MESSAGE = "MESSAGE";
static String ledState;

#endif
