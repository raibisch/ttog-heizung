#ifndef AsyncWebLog_h
#define AsyncWebLog_h

#include "Arduino.h"
#include "stdlib_noniso.h"
#include <functional>

#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// by JG:  log only oneway server-->client
//typedef std::function<void(uint8_t *data, size_t len)> RecvMsgHandler;

// Uncomment to enable webserial debug mode
// #define WEBSERIAL_DEBUG 1
// only println for Strings
#define MINI_WEBLOG

// by JG: point to Webpage in SPIFF
#define LOG_URL "/log.html"

class AsyncWebLogClass
{

public:
    void begin(AsyncWebServer *server, const char* url = LOG_URL);

    // by JG:  log only oneway server-->client
    //void msgCallback(RecvMsgHandler _recv);

    // Print for all types
#ifndef MINI_WEBLOG
    void print(String m = "");

    void print(const char *m);

    void print(char *m);

    void print(int m);

    void print(uint8_t m);

    void print(uint16_t m);

    void print(uint32_t m);

    void print(double m);

    void print(float m);


    // Print with New Line


    void println(const char *m);

    void println(char *m);

    void println(int m);

    void println(uint8_t m);

    void println(uint16_t m);

    void println(uint32_t m);

    void println(float m);

    void println(double m);

#endif 
    // only for Strings
    void println(String m = "");


private:
    AsyncWebServer *_server;
    AsyncEventSource *_events; // use Server Send Events (SSE)
    //AsyncWebSocket *_ws;     // maybe alternative websocket

    // by JG:  only for websocket, SSE is only oneway
    //RecvMsgHandler _RecvFunc = NULL;
    
    #if defined(WEBSERIAL_DEBUG)
        void DEBUG_WEB_SERIAL(const char* message);
    #endif
};

extern AsyncWebLogClass AsyncWebLog;
#endif