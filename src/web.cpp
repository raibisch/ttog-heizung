#include "web.h"
#include "history.h"
//#include "ttgo_iodef.h"
//#include "var_dev.h"
//#include "io.h"

static const char * hostName = "heizung";

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
  while (c >= 0) {
    ret += (char)c;
    c = s.read();
  }
  return ret;
}

// UINT63 in String wandeln (geht nicht mit standart "String()")
String uint64ToString(uint64_t input) 
{
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;}



// Replaces placeholder with value
String setHtmlVar(const String& var)
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

  if(var == "HEIZUNG")
  {
    if(Device_Heizung.active > 0)
	  {
      ledState = "ON";
    }
    else
	  {
      ledState = "OFF";
    }
    //Serial.println(ledState);
    return ledState;
  }
  
  if(var == "VENTIL")
  {
    if(Device_WWSpeicher.active > 0)
	  {
      ledState = "ON";
    }
    else
	  {
      ledState = "OFF";
    }
    //Serial.println(ledState);
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
      return String(Device_Heizung.temp_akt);
  }

  if (var == "TEMPAU")
  {
      return String(Device_allg.temp_aussen);
  }

   if (var == "HISTORY")
  {
      return uint64ToString(Device_allg.sec_brenner_on_sum);
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


void server_init()
{
  Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
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

  //Route for root /index1 web page
  server.on("/index1.html",          HTTP_GET, [](AsyncWebServerRequest *request)
  {
   Serial.println("Request /index1.html");
   request->send(SPIFFS, "/index1.html", String(), false, setHtmlVar);
  });

  // Configuration
  //Route for config web page
  server.on("/config.html",          HTTP_GET, [](AsyncWebServerRequest *request)
  {
   Serial.println("Request /config.html");
   request->send(SPIFFS, "/config.html", String(), false, setHtmlVar);
  });


  //Route for root / web page
  server.on("/",          HTTP_GET, [](AsyncWebServerRequest *request)
  {
   Serial.println("Request /index.html");
   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar);
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  //server.serveStatic("/", SPIFFS, "/").setTemplateProcessor(setHtmlVar);


  // Route to set GPIO to HIGH
  server.on("/onheiz",        HTTP_GET, [](AsyncWebServerRequest *request)
  {
   Config_val.device_manuell=0;
   SetRelaisNetz(_EIN);

   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar);
  });

  // Route to set GPIO to LOW
  server.on("/offheiz",       HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Config_val.device_manuell=1;
     //SetRelaisNetz(_AUS);
    request->send(SPIFFS, "/index.html", String(), false, setHtmlVar);
  });

// Route to set GPIO to HIGH
  server.on("/onventil",        HTTP_GET, [](AsyncWebServerRequest *request)
  {
    SetRelaisVentil(_EIN);
  
   request->send(SPIFFS, "/index.html", String(), false, setHtmlVar);
  });

  // Route to set GPIO to LOW
  server.on("/offventil",       HTTP_GET, [](AsyncWebServerRequest *request)
  {
    SetRelaisVentil(_AUS);
    //digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, setHtmlVar);
  });


  server.on("/fetch", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    
    Serial.println("Request /fetch");
    String netz    = "ON";
    String brenner = "ON";
    String ventil  = "ON";
   
    if (Device_Heizung.active == false)
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
    
    loop_active =true;
    String s = String(Device_BrennerKessel.temp_akt) + "," +
               String(Device_WWSpeicher.temp_akt)    + "," +
               String(Device_Heizung.temp_akt)    + "," +
               String(Device_allg.temp_aussen)    + "," +
               netz + "," +
               ventil +"," +
               brenner + "," +
               String(get_Heizoel_l_Verbrauch());

    request->send(200, "text/plain", s);
    Serial.println("server.on /fetch: "+ s);
    loop_active = false;
  });


  //Route for root /index1 web page
  server.on("/index3.html",          HTTP_GET, [](AsyncWebServerRequest *request)
  {
   Serial.println("Request /index3.html");
   request->send(SPIFFS, "/index3.html", String(), false, setHtmlVar);
  });

  //Route for root /config.html web page
  server.on("/config.html",          HTTP_POST, [](AsyncWebServerRequest *request)
  {
   Serial.println("Argument: " + request->argName(0));
   Serial.println("Value: ");
   uint8_t i = 0;
   String s  = request->arg(i);
   Serial.println(s);
   if (request->argName(0) == "saveconfig")
   {
       var_devSaveConfig(s);
       history_save();
   }
   if (request->argName(0) == "reboot")
   {
       history_save();
       ESP.restart();
   }
   //Serial.println("Request /index3.html");
   request->send(SPIFFS, "/config.html", String(), false, setHtmlVar);
  });
  
 
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
       Serial.println(message);  Serial.println("config.txt save OK!");
    });

    server.begin();
}
