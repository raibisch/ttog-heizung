#ifndef TTGO_WEB_H
#define TTGO_WEB_H

#include "io.h"
#include "var_dev.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ESP32Ping.h>

#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"


//#include "var_dev.h"
//#include "ttgo_iodef.h"

static AsyncWebServer server(80);

void server_init();
//AsyncWebSocket ws("/test");


//const char* PARAM_MESSAGE = "MESSAGE";
static String ledState;

#endif
