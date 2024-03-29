#include "web.h"
#include "history.h"
// #include <AsyncElegantOTA.h>
// #include "ttgo_iodef.h"
// #include "var_dev.h"
// #include "io.h"

static const char *hostName = "heizung";

// AsyncEventSource events("/logevents");

// z.Z. nicht genutzt
// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// History String für SetDygraph
// z.Z. noch nicht eingebunden !! (Stand 2019-03-16)
// valid String "2019/07/12 12:34\n"
/*
String SetDygraphHistory()
{
  String sRet = "";
  for (int i = 0; i < 1440; i++)
  {
    if (history_valDayPerMin[i].t_kessel < 99.0)
    {
      int hour = i / 60;
      int min  = i % 60;
      char buffer[18];
      sprintf(buffer, "%4d/%02d/%02d %02d:%02d", year_old, month_old, day_old, hour, min);
      //sprintf(buffer, "%02d:%02d %02d.%02d.%4d", timeClient.getHours(), timeClient.getMinutes(), day(), month(), year());
      String s = buffer +'\n';
      Serial.print(s);
      sRet = sRet +s;
    }
  }
  return sRet;
}
*/

static String readString(File s)
{
  String ret;
  int c = s.read();
  while (c >= 0)
  {
    ret += (char)c;
    c = s.read();
  }
  return ret;
}

// UINT63 in String wandeln (geht nicht mit standard "String()")
String uint64ToString(uint64_t input)
{
  String result = "";
  uint8_t base = 10;

  do
  {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}



String httpGETRequest(String url)
{
  String payload = "";
  
  HTTPClient http; 
  http.setTimeout(500);
  http.begin(url); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) 
  {
            // HTTP header has been send and Server response header has been handled
            // file found at server
            if(httpCode == HTTP_CODE_OK) 
            {
                payload = http.getString();
            }
  } else 
  {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return payload;
}

// macht probleme
// evt mal dies ausprobieren
// https://github.com/boblemaire/asyncHTTPrequest
/*
static HTTPClient http;
static WiFiClient wc;
String httpGETRequest_old(const char *serverName)
{
  // Your Domain name with URL path or IP address with path
    http.begin(wc, serverName);
    http.setTimeout(500);
 
  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "";

  if (httpResponseCode > 0)
  {
    // Serial.print("HTTP Response code: ");
    // Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("*** HTTP-GET: Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources

  http.end();
  wc.stop();
 
  return payload;
}
*/


// Replaces placeholder %XXXX% with value
String setHtmlVar(const String &var)
{
  Serial.print("func:setHtmlVar: ");
  Serial.println(var);
  /*
  if (var == "HIST")
  {
    String s = "Date,ON_Kessel,ON_WWVentil,Kessel,WW,Heiz,AussenEND";
    //String s= "";
    int ix = history_get_actual_index();
    for (int i = 0; i <= ix; i++)
    {
      Serial.print(history_event2string(i));

      // Arbeisende: geht so nicht !!!

      //String hv = sx::copy(history_event2string, 44);
      // nur damit was ausgegeben wird:
      String hv = "hallo";
      s = s + hv;
      s = s + "END";
    }

    return s;
  }
  */

  if (var == "HEIZUNG")
  {
    if (Device_allg.bNetzON == true)
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    // Serial.println(ledState);
    return ledState;
  }

  if (var == "VENTIL")
  {
    if (Device_WWSpeicher.active > 0)
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    // Serial.println(ledState);
    return ledState;
  }

  if (var == "TEMPK")
  {
    return String(Device_BrennerKessel.temp_akt);
  }

  if (var == "TEMPWW")
  {
    return String(Device_WWSpeicher.temp_akt);
  }

  if (var == "TEMPHE")
  {
    return String(Device_HeizRueckl.temp_akt);
  }

  if (var == "TEMPAU")
  {
    return String(Device_allg.temp_aussen);
  }

  if (var == "HISTORY")
  {
    Serial.println("%HISTORY%");
    return uint64ToString(Device_allg.sec_brenner_on_sum * Config_val.mliter_oel_pro_h / 3600);
  }

  if (var == "WWSOLL")
  {
    return String(Config_val.temp_ww_soll_ein);
  }

  if (var == "WINTER")
  {
    return String(Config_val.temp_winter_tag);
  }

  if (var == "CONFIG")
  {
    if (!SPIFFS.exists("/config.txt"))
    {
      return String(F("Fehler: Datei 'config.txt' nicht vorhanden!"));
    }
    // config.txt einlesen
    fs::File configfile = SPIFFS.open("/config.txt", "r");
    String sConfig;
    if (configfile)
    {
      sConfig = readString(configfile);
      configfile.close();
    }
    else
    { // Keine Datei config.txt
      sConfig = "";
    }
    return sConfig;
  }

  return String();
}

/*
static bool newMQTT=false;
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
 newMQTT = true;
 Serial.print("**** Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
   Serial.print((char)payload[i]);
 }
 Serial.println();
}


void mqtt_client_init()
{
   mqttClient.setServer("192.168.2.22", 1883);
   mqttClient.setCallback(mqttCallback);
   if (mqttClient.connect("heizung"))
   {
   // connection succeeded
   Serial.println("***Connected now subscribing***");
   mqttClient.subscribe("tele/wetterstat/SENSOR");
   if (mqttClient.connected())
   {
      Serial.println("***Connected now ***");
      mqttClient.
   }

   }
   else
   {
   // connection failed
   // mqttClient.state() will provide more information
   // on why it failed.
   Serial.println("****Connection failed ****");
   }

}
*/

void server_init()
{
  Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(hostName))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  else
  {
    Serial.println("mDNS responder started");
  }

  // Route for root /index web page
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   Serial.println("Request /index.html");
   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar); });

  // **Configuration**
  // Route for config web page
  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   Serial.println("Request /config.html");
   request->send(SPIFFS, "/config.html", String(), false, setHtmlVar); });

  // Set **HISTORY** values
  // Route for config web page
  server.on("/setvalues.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   Serial.println("Request /setvalues.html");
   request->send(SPIFFS, "/setvalues.html", String(), false, setHtmlVar); });

  // only Testpage
  // Route for root / web page
  server.on("/index1.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   Serial.println("Request /index1.html");
   request->send(SPIFFS, "/index1.html", String(), false, setHtmlVar); });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  // bringt nichts:
  // server.serveStatic("/", SPIFFS, "/").setTemplateProcessor(setHtmlVar);

  // Route to set GPIO to HIGH
  server.on("/onheiz", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   SetRelaisNetz(_EIN);
   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar); });

  // Route to set GPIO to LOW
  server.on("/offheiz", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SetRelaisNetz(_AUS);
    request->send(SPIFFS, "/index.html", String(), false, setHtmlVar); });

  // Route to set GPIO to HIGH
  server.on("/onventil", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SetRelaisVentil(_EIN);
   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar); });

  // Route to set GPIO to LOW
  server.on("/offventil", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SetRelaisVentil(_AUS);
    request->send(SPIFFS, "/index.html", String(), false, setHtmlVar); });

  server.on("/fetch", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    
    Serial.println("Request /fetch");
    String netz    = "ON";
    String brenner = "ON";
    String ventil  = "ON";
   
    if (Device_allg.bNetzON == false)
    {
       netz = "OFF";
    }
     if (Device_WWSpeicher.active  == false)
    {
      ventil = "OFF";
    }
    
    if (Device_BrennerKessel.active == false)
    {
      brenner = "OFF";
    }
    
    //loop_active =true;
    String s = String(Device_BrennerKessel.temp_akt)  + "," +
               String(Device_WWSpeicher.temp_akt)     + "," +
               String(Device_HeizRueckl.temp_akt)     + "," +
               String(Device_allg.temp_aussen)        + "," +
               netz                                   + "," +
               ventil                                 + "," +
               brenner                                + "," +
               String(get_Heizoel_mLiter_Verbrauch()) + "," +
               String(Device_allg.onStatus);

    request->send(200, "text/plain", s);
    Serial.println("server.on /fetch: "+ s); });

  // Route for root /index1 web page
  server.on("/index3.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
   Serial.println("Request /index3.html");
   request->send(SPIFFS, "/index3.html", String(), false, setHtmlVar); });

  // Route  /config.html web page
  server.on("/config.html", HTTP_POST, [](AsyncWebServerRequest *request)
            {
   Serial.println("Argument: " + request->argName(0));
   Serial.print("Value: ");
   uint8_t i = 0;
   String s  = request->arg(i);
   Serial.println(s);
   if (request->argName(0) == "saveconfig")
   {
       var_devSaveConfig(s);
       history_save();
   }
   else
   if (request->argName(0) == "reboot")
   {
       history_save();
       ESP.restart();
   }

   request->send(SPIFFS, "/config.html", String(), false, setHtmlVar); });

  // Route  /config.html web page
  server.on("/setvalues.html", HTTP_POST, [](AsyncWebServerRequest *request)
            {
  
   Serial.println("Argument: " + request->argName(0));
   Serial.print("Value: ");
   uint8_t i = 0;
   String s  = request->arg(i);
   Serial.println(s);
   if (request->argName(0) == "oelverbrauch") // in history oelverbrauch neu setzten
   {
      set_Heizoel_mLiter_Verbrauch(s.toDouble());
   }

   request->send(SPIFFS, "/setvalues.html", String(), false, setHtmlVar); });

  server.onNotFound([](AsyncWebServerRequest *request)
                    {
     Serial.printf("NOT_FOUND: ");
     String message = "File Not Detected\n\n";
       message += "URI: ";
       message += request->url().c_str();
       message += "\nMethod: ";
       message += (request->method() == HTTP_GET)?"GET":"POST";
       message += "\nArguments: ";
       message += request->args();
       message += "\n";
       for (uint8_t i=0; i<request->args(); i++){
         message += " NAME:"+request->argName(i) + "\n VALUE:" + request->arg(i) + "\n";
       }
       request->send(404, "text/plain", message);
       Serial.println(message); });

  // AsyncElegantOTA.begin(&server);    // Start ElegantOTA

  AsyncWebLog.begin(&server);
  AsyncWebOTA.begin(&server);
  server.begin();
}
