// GxEPD_Example : test example for e-Paper displays from Waveshare and from Dalian Good Display Inc.
//
// Created by Jean-Marc Zingg based on demo code from Good Display,
// available on http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// The e-paper displays are available from:
//
// https://www.aliexpress.com/store/product/Wholesale-1-54inch-E-Ink-display-module-with-embedded-controller-200x200-Communicate-via-SPI-interface-Supports/216233_32824535312.html
//
// http://www.buy-lcd.com/index.php?route=product/product&path=2897_8363&product_id=35120
// or https://www.aliexpress.com/store/product/E001-1-54-inch-partial-refresh-Small-size-dot-matrix-e-paper-display/600281_32815089163.html
//

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Dispay ePaper for Arduino : https://forum.arduino.cc/index.php?topic=436411.0

// mapping suggestion from Waveshare SPI e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

// mapping suggestion for Arduino MEGA
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 53, CLK -> 52, DIN -> 51

// include library, include base class, make path known

// by JG *************************************************************
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
// bitmap generator
// http://javl.github.io/image2cpp/
// *******************************************************************
#include <Arduino.h>
#include <TaskScheduler.h>

#include <NTPClient.h>
#include <Timezone.h>
#include <TimeLib.h>
#include <Button.h>
#include <OneWire.h>

#include "ttgo_iodef.h"
#include "io.h"
#include "epdisplay.h"
#include "web.h"

#include "jg18b20.h"
#include "history.h"

#include "credentials.cred"
#include "AsyncWebLog.h"

WiFiUDP ntpUDP;
//Pointer To The Time Change Rule, Use to Get The TZ Abbrev
TimeChangeRule *tcr;
time_t  time_now ;

unsigned long time_1 = 0;
int i = 0;


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire ow(IN_ONEWIRE);


NTPClient timeClient(ntpUDP, "192.168.2.1");
//NTPClient timeClient(ntpUDP, "ntp1.t-online.de");
//TimeZone Settings Details https://github.com/JChristensen/Timezone
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time (Frankfurt, Paris)
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Time (Frankfurt, Paris)
Timezone CE(CEST, CET);


Button button1 = Button(BUTTON1);
Button button2 = Button(BUTTON2);
Button button3 = Button(BUTTON3);


void display_Icon16(const uint16_t x, const uint16_t y, const uint8_t bitmap[]);
void display_Clear32(const uint16_t x, const uint16_t y);
void display_Text9pt(const uint16_t x, const uint16_t y, String txt);

// Callback methods prototypes
void t1_DisplayCallback();
void t2_InputCallback();
void t3_VentilCallback();
void t4_NetzOnCallback();
void t5_PumpeNachlaufCallback();
void t6_ds18d20S1Callback();
void t7_ds18d20S2Callback();
void t8_WebRequest();


Task tDisplay      (1000, TASK_FOREVER, &t1_DisplayCallback);
Task tInput        (500,  TASK_FOREVER, &t2_InputCallback);
Task tVentil       (500, TASK_FOREVER, &t3_VentilCallback);
Task tPumpe        (500, TASK_FOREVER, &t4_NetzOnCallback);
Task tPumpeNachlauf(500, TASK_FOREVER, &t5_PumpeNachlaufCallback);

Task tDs18d20State_1(2000, TASK_FOREVER, &t6_ds18d20S1Callback);
Task tDs18d20State_2(1, TASK_ONCE, &t7_ds18d20S2Callback);
Task tWebRequest   (2000, TASK_FOREVER, &t8_WebRequest);

Scheduler taskManager;

void io_init()
{
  pinMode(OUT_RELAIS1, OUTPUT);
  pinMode(OUT_RELAIS2, OUTPUT);
  pinMode(IN_BRENNER, INPUT_PULLUP);
  pinMode(IN_ERROR,   INPUT_PULLUP);
#ifdef HELTEC_WIFI_KIT32
  pinMode(LED_BUILTIN, OUTPUT);
#endif

  digitalWrite(OUT_RELAIS1, 1);
  digitalWrite(OUT_RELAIS2, 1);

  button1.begin();
  button2.begin();
  button3.begin();
}


// *** Time Lib einbinden *** /*
time_t ntpSyncProvider()
{
  return time_now;
}

// Teste ob die Fritzbox noch erreichbar ist 
const IPAddress remote_ip(192, 168, 2, 1);
bool isFritzBoxPing()
{
  if(Ping.ping(remote_ip)) 
  {
    Serial.println("**> Ping zur FritzBox OK!");
    return true;
  } 
  else 
  {
    Serial.println("****> KEIN PING ZUR FRITZBOX MOEGLICH ***");
    return false;
  }
}

unsigned min_tmp =99;
void DisplayTime()
{
  if (min_tmp != timeClient.getMinutes())
  {
   
    min_tmp = timeClient.getMinutes();
    char buffer[18];
    sprintf(buffer, "%02d:%02d %02d.%02d.%4d", timeClient.getHours(), timeClient.getMinutes(), day(), month(), year());
    display_Text9pt(0,0,buffer);
    Serial.println(buffer);
 }
}

void UpdateTime()
{
  timeClient.update();
  time_now = CE.toLocal(timeClient.getEpochTime(), &tcr);
  setSyncProvider(&ntpSyncProvider);

  if (CE.locIsDST(time_now) )
  {
     //Serial.println("Sommerzeit");
     timeClient.setTimeOffset(7200);
  }
  else
  {
     timeClient.setTimeOffset(3600);
  }

  // Save History um Mitternacht
  if (timeClient.getHours()==0 && timeClient.getMinutes()==0 && timeClient.getSeconds()< 2)
  {
    history_save();
  }
}


void menu_display()
{
  int x_menu = 60;
  int y_menu  = 112;
  String s = Menu[Menu_akt].button1;
  display_Text9pt(x_menu +  00, y_menu, "[man]"/* Menu[Menu_akt].button1*/);
  //display_Text9pt(x_menu + 53, y_menu, Menu[Menu_akt].button2);
  //display_Text9pt(x_menu + 91, y_menu, Menu[Menu_akt].button3);
  Menu_akt = Menu[Menu_akt].next_menuix;

  // Ueberschriften für Temp
  display_Text9pt(1,20,"Kessel");
  display_Text9pt(75,20,"W.W.");
  display_Text9pt(75+75,20,"Heiz.");
  display_Text9pt(75+75+75,20,"Aussen");
}

// Task DISPLAY
//**********************************************************
void t1_DisplayCallback(void)
{
  //Serial.println("--> DisplayCallback");
  Serial.print("Heizoel sec.: ");
  unsigned long m = Device_allg.sec_brenner_on_sum;
  Serial.println(m);

  UpdateTime();
  DisplayTime();

  display_Temp(1, Device_BrennerKessel.temp_akt);
  display_Temp(76, Device_WWSpeicher.temp_akt);
  display_Temp(151, Device_HeizRueckl.temp_akt);
  display_Temp(226, Device_allg.temp_aussen);
}

// Task Input
// *************************************************************
void t2_InputCallback(void)
{
  //Serial.println("--> InputCallback");

  // ------- BUTTON 1-3 -------------
  /* z.Z nicht genutz
  if(button1.pressed())
  {
    Device_allg.bManuell = true;
    display_Text9pt(1,112," B1 ");
    if (Device_Pumpe.active == false)
    {
      SetRelaisNetz(true);
    }
  }
  else
  if (button2.pressed())
  {
    display_Text9pt(1,112," B2 ");
    Device_allg.bManuell = true;
    if (Device_WWSpeicher.active == false)
    {
      SetRelaisVentil(true);
    }
  }
  else
  if (button3.pressed())
  {
    Device_allg.bManuell = false;
    display_Text9pt(1,112," B3 ");
    //SetRelaisNetz(false);
    SetRelaisVentil(false);
  }
  */
  // ----------- IO Brenner aktiv -------------------
  // Achtung neg. logic 1=AUS 0=aktiv
  if (digitalRead(IN_BRENNER))
  { // 1
    // Brenner AUS
    if (Device_BrennerKessel.active == true)
    {
      display_Clear32(2,70);
      Device_allg.sec_brenner_on=0;
      Device_allg.sec_brenner_old_sum = Device_allg.sec_brenner_on_sum;
    }
    Device_BrennerKessel.active = false;
  }
  else    
  {  // 0  
     // Brenner EIN
    if (Device_BrennerKessel.active == false)
    {   
     
        Device_allg.millis_start_brenner = millis();
        display_Icon32(2,70, kessel_32px);
    }
    Device_BrennerKessel.active = true;
    Device_allg.sec_brenner_on = (millis() - Device_allg.millis_start_brenner) / (long int)1000;
    Device_allg.sec_brenner_on_sum = Device_allg.sec_brenner_on + Device_allg.sec_brenner_old_sum;
  }

  // ----------- IO Stoerung -------------------
  // Achtung 0 = Störung !!!
  /*
    evt Stoerung durch Parameter
    wie z.B. Kessel < 40Grad bestimmen
    und dann event (email?) auslösen

    
  if (digitalRead(IN_ERROR))
  {
    //display_Text9pt(0,14,"      ");
    if (Device_allg.bError == true)
    {
      display_Text9pt(0,14,"      ");
    }
    Device_allg.bError = false;

  }
  else
  {
      //display_Text9pt(0,14,"      ");
    if (Device_allg.bError == false)
    {

    }
    Device_allg.bError = false;
  }
  */

}

// Task Ventil (Heinzung/Warmwasser) Steuerung
// hier kann noch etwas mehr "Intelligenz" eingebaut werden
// *******************************************************************
// ACHTUNG: wird hier NIE ausgeschaltet, da dies im Nachlauf geschieht
// *******************************************************************

// IDEE: WW-Ventil im Winter nicht zu lange eingeschaltet lassen, da dann die Heizkörper nicht versorgt werden
// ... mal sehen ob das im Winter funktioniert
uint16_t ventil_winter_on_toggle =0;
uint16_t ventil_winter_off_toggle = 0;

void t3_VentilCallback()
{
   if (Config_val.ventil_manuell == 1)
   {
     AsyncWebLog.println("Ventil-MANUELL");
     return;
   }

   if ( Device_allg.bNetzON==true)
   {
     // Sommerbetrieb
     if (Device_Wetterstation.temp_akt > Config_val.temp_winter_tag)
     {
         AsyncWebLog.println("SOMMERBETRIEB: Ventil-EIN");
         SetRelaisVentil(true);
         return;
     }
     else
     { // Winterbetrieb

      // Wasser warm genug
      if (Device_WWSpeicher.temp_akt >= Config_val.temp_ww_soll_aus)
      {
         ventil_winter_on_toggle=0;
         AsyncWebLog.println("WINTERBETRIEB:Ventil-AUS (WW warm genug)");
         SetRelaisVentil(false);
         return;
      }

      // Wasser zu kalt
      if (Device_WWSpeicher.temp_akt < Config_val.temp_ww_soll_ein)
      {
      
        AsyncWebLog.println("WINTERBETRIEB: WW zu kalt");
        if (Device_WWSpeicher.active)
        {
           String s = "WINTERBETRIEB: WW-ON countdown:" + String(Config_val.time_minute_ww_winter*120 - ventil_winter_on_toggle);
           AsyncWebLog.println(s);
           ventil_winter_on_toggle++;
           
           if (ventil_winter_on_toggle > Config_val.time_minute_ww_winter*120)
           {
            ventil_winter_on_toggle = 0;
             AsyncWebLog.println("WINTERBETRIEB: SET Ventil-AUS (Toggel-Betrieb)");
            SetRelaisVentil(false);
           }
        }
        else
        {
          String s = "WINTERBETRIEB: WW-OFF-countdown:" + String(Config_val.time_minute_ww_winter*120 - ventil_winter_off_toggle);
          AsyncWebLog.println(s);
          ventil_winter_off_toggle++;
          if (ventil_winter_off_toggle > Config_val.time_minute_ww_winter*120)
          {
            ventil_winter_off_toggle = 0;
            AsyncWebLog.println("WINTERBETRIEB: SET Ventil-EIN (Toggle-Betrieb)");
            SetRelaisVentil(true);
          }
        }
      }
     }
   }
   else
   {
     SetRelaisVentil(false); // Immer AUS wenn Netz OFF
   }
   
}

// ALT:Task Pumpe Steuerung
// neu: JETZT NETZ Steuerung !!
// auch hier kann noch etwas mehr "Intelligenz" eingebaut werden
static bool bNachlauf_active = false;
static uint8_t off_hour, off_minu;
// ***********************************************
void t4_NetzOnCallback()
{
  //Serial.println("--> NetzOnCallback");
  //String s= "NetzOnCallback iTest:" + String(iTest++)+;
  //events.send(s.c_str(), "logprint", millis());
  //AsyncWebLog.println("NetzOnCallback iTest:" + String(iTest++));
  if (Config_val.netz_manuell==1)
  {
    AsyncWebLog.println("STATUS: MANUELL");
    return;
  }

  if (Device_BrennerKessel.active)
  {
    bNachlauf_active = false;
     tPumpeNachlauf.disable();
    //Serial.println("NETZ-EIN: Brenner active");
    SetRelaisNetz(_EIN);
    Device_allg.onStatus = ONSTATUS_BRENNERAKTIV;
    AsyncWebLog.println("STATUS: BRENNERAKTIV");
    return;
  }
  
  // Test Winter-Nacht EIN
  if(Device_Wetterstation.temp_akt < Config_val.temp_winter_nacht)
  {
    Device_allg.onStatus = ONSTATUS_WINTER;
    AsyncWebLog.println("ON WINTER NACHT. oder sehr kalt:"+ String(Device_Wetterstation.temp_akt));
    SetRelaisNetz(_EIN);
    return;
  }

  uint16_t minday_akt = timeClient.getHours()*60 + timeClient.getMinutes();

  // Vorgabe Einschalten: Wochentag-ON
  uint16_t minday_calc_on  = Config_val.hour_heizung_werktag_on*60 + Config_val.minute_heizung_werktag_on;
  // Vorgabe Auschalten: Winter-OFF
  uint16_t minday_calc_off = Config_val.hour_heizung_winter_off*60 + Config_val.minute_heizung_winter_off;
  off_hour = Config_val.hour_heizung_winter_off; off_minu = Config_val.minute_heizung_winter_off;

  // Test Wochenende (setzt hier nur Einschaltzeit am Morgen)
  if ((timeClient.getDay() == 7) || timeClient.getDay() == 0)
  {
    AsyncWebLog.println("ON Wochenende ");
    minday_calc_on = Config_val.hour_heizung_wochenende_on*60 + Config_val.minute_heizung_wochenende_on;
    Device_allg.onStatus = ONSTATUS_WEEKEND;
  }
 
  // Test Winter-Tag EIN
  if ( 
       (Device_Wetterstation.temp_akt < Config_val.temp_winter_tag) &&
       ((minday_akt > minday_calc_on) && (minday_akt < minday_calc_off))
     )
  {
    Device_allg.onStatus = ONSTATUS_WINTER;
    AsyncWebLog.println("ON WINTER Tag, Temp-Aussen:"+ String(Device_Wetterstation.temp_akt));
    SetRelaisNetz(_EIN);
    return;
  }
  
  // Test Sommer-TAG EIN
  minday_calc_off = Config_val.hour_heizung_sommer_off*60 + Config_val.minute_heizung_sommer_off;
  off_hour = Config_val.hour_heizung_sommer_off; off_minu = Config_val.minute_heizung_sommer_off;
  if ((minday_akt > minday_calc_on) && (minday_akt < minday_calc_off))
  {
     Device_allg.onStatus = ONSTATUS_SUMMER;
    
     // Wasser warm genug
    if (Device_WWSpeicher.temp_akt >= Config_val.temp_ww_soll_aus)
    {
         ventil_winter_on_toggle=0;
         AsyncWebLog.println("STATUS: AUS SOMMER(WW warm genug)");
         //SetRelaisVentil(false);
         // ... kein Return.. laueft dann in den Nachlauf
    }
    else
    {
      AsyncWebLog.println("STATUS:EIN Sommer Zeitschalt.");
     SetRelaisNetz(_EIN);
     return;
    }
  }

  // WW im Sommer nach Einschaltzeit bei Tag zu kalt, bis abends nachheizen
  minday_calc_off = Config_val.hour_heizung_winter_off*60 + Config_val.minute_heizung_winter_off;
  if ((minday_akt < minday_calc_off)&& (Device_WWSpeicher.temp_akt < Config_val.temp_ww_soll_ein) )
  {
    ///Serial.printf("NETZ-EIN: WW-akt:%f < WW-soll:%d\n", Device_WWSpeicher.temp_akt, Config_val.temp_ww_soll_ein);
    SetRelaisNetz(_EIN);
    Device_allg.onStatus = ONSTATUS_WWLOW;
    AsyncWebLog.println("STATUS:EIN WW-LOW");
    return;
  }

  // Nachlauf... kommt nur dahin wen vorherige Afragen nicht erfuellt !!
  if ((bNachlauf_active == false))
  {
    bNachlauf_active = true;
    tPumpeNachlauf.enableDelayed(Device_allg.nachlauf);
  }
  else if (Device_allg.bNetzON==true)
  {
     Device_allg.onStatus = ONSTATUS_NACHLAUF;
    Serial.println("--> Nachlauf");
    AsyncWebLog.println("STATUS: EIN Nachlauf");
    return;
  }
  else
  {
    Device_allg.onStatus = ONSTATUS_OFF;
    String s = "AUS  Off-Time:"+ String(off_hour) +":"+ String(off_minu);
    AsyncWebLog.println(s);
  }
}

void t5_PumpeNachlaufCallback()
{
 
  Serial.println("--> *** PumpeNachlaufCallback ***");
  /*
  if (Config_val.device_manuell==1) 
  { 
     tPumpeNachlauf.disable();
     return; 
  }
  */

  if  (Device_BrennerKessel.active == false)
  {
    bNachlauf_active = false;
    SetRelaisNetz(_AUS);
    tPumpeNachlauf.disable();
    Device_allg.onStatus = ONSTATUS_OFF;
  }
  
}

void t6_ds18d20S1Callback()
{  
  //Serial.println("--> ds1820_S1 Callback");
  jg18b20ConvertState_1(ow);

  tDs18d20State_2.restartDelayed(850);
  tDs18d20State_2.enable();
}

float tmpf;         
static float old_val[4];
void t7_ds18d20S2Callback()
{
   
   //Serial.println("--> ds1820_S2 Callback");
   jg18b20ConvertState_2(ow);
   tmpf = jg18b20GetValue(Config_val.sensor_ix_kessel);
   if (tmpf > 0) 
   { 
     Device_BrennerKessel.temp_akt = tmpf; 
     old_val[Config_val.sensor_ix_kessel] = tmpf;
   }

   tmpf = jg18b20GetValue(Config_val.sensor_ix_ww);
   if (tmpf > 0)
   { 
     Device_WWSpeicher.temp_akt    = tmpf; 
     old_val[Config_val.sensor_ix_ww] = tmpf;
   }

   tmpf = jg18b20GetValue(Config_val.sensor_ix_ruecklauf);
   if (tmpf > 0) 
   { 
     Device_HeizRueckl.temp_akt    = tmpf; 
      old_val[Config_val.sensor_ix_ruecklauf] = tmpf;
   }
   
   tmpf = jg18b20GetValue(Config_val.sensor_ix_aussen);
   if (Device_Wetterstation.temp_akt != 0)
   {
        Device_allg.temp_aussen = Device_Wetterstation.temp_akt;
   }
   else
   if  (tmpf != -0.1)
   {
     Device_allg.temp_aussen   = tmpf; 
    old_val[Config_val.sensor_ix_aussen] = tmpf;
   }

}


void t8_WebRequest()
{
   //Serial.println("--> t8_WebRequest Callback");

  
  if (!isFritzBoxPing())
  {
    delay(500);
    history_save();
    WiFi.reconnect();
  }
  
  // Wetterstation abfragen
  // direkt von Wetterstation (Tastmota)
  /*
  String s = httpGETRequest("http://192.168.2.71/cm?cmnd=status%2010");
  if (s.length() < 10)
  {
    return;
  }
  int ix1 = s.indexOf("Temperature\":");
  int ix2 = s.indexOf(",",ix1+5);
  if ((ix1 > 0) && (ix2 > ix1))
  {
    String sTemp = s.substring(ix1+13,ix2);
    Serial.printf("**Temperatur von Wetterstation:%s\n", sTemp.c_str());
    Device_Wetterstation.temp_akt= sTemp.toFloat();
  }
  oder :
  von Domiticz */
  String s = httpGETRequest("http://192.168.2.22/json.htm?type=devices&rid=45");
  if (s.length() < 10)
  {
    return;
  }
  int ix1 = s.indexOf("Data\"");
  int ix2 = s.indexOf("\"",ix1+5);
  int ix3 = s.indexOf("\"",ix2+2);
  if ((ix1 > 0) && (ix3 > ix2))
  {
    
    String sTemp = s.substring(ix2+1,ix3-1);
    Serial.printf("**Temperatur von Wetterstation:%s\n", sTemp.c_str());
   
    Device_Wetterstation.temp_akt= sTemp.toFloat();
  }
   
  
}
//**********************************************************
void menu_init()
{
  Menu_akt = 1;

  Menu[1].button1 = "[man]";
  Menu[1].button2 = "[+]";
  Menu[1].button3 = "[-]";
  Menu[1].next_menuix = 0;
}

void wifi_init()
{
  WiFi.begin(ssid, password);
  delay(1000);
  //display_Text9pt(187,0,".CONNECT");
  int i = 0;
  WiFi.setHostname("Heizungtest");

  int wifiClientConnectionTimeoutSeconds = 5;
  int timeoutCounter = wifiClientConnectionTimeoutSeconds * (1000 / 150);
  while (WiFi.status() != WL_CONNECTED && timeoutCounter > 0)
  {
#ifdef HELTEC_WIFI_KIT32
     digitalWrite(LED_BUILTIN, 1);
     delay(150);
     digitalWrite(LED_BUILTIN,0);
     delay(150);
#else
    delay(300);
    display_Text9pt(187,0,".CONNECT");
    delay ( 50 );
    display_Text9pt(187,0,"          ");
#endif
    if (timeoutCounter == (wifiClientConnectionTimeoutSeconds * 2 - 3))
    {
      WiFi.reconnect();
    }
    timeoutCounter--;
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED)
  {
     String scon = "\r\nSSID:" + WiFi.SSID() + "  IP:" + WiFi.localIP().toString();
     Serial.println(scon);
     display_Text9pt(187,0, ssid);
  }
  else
  {
     Serial.print("*** ERROR: no Wifi connection !");
     delay(100);
#ifdef HELTEC_WIFI_KIT32
     digitalWrite(LED_BUILTIN, 1);
     delay(800);
#endif   
    ESP.restart();
  }
 
}



//****** S E T U P ***********************************************
void setup()
{

  io_init();
  Serial.begin(115200);

  wifi_init();
  


  if (!SPIFFS.begin())
  {
   // Serious problem
   Serial.println("SPIFFS Mount failed");
  } else
  {
   Serial.println("SPIFFS Mount succesfull");
   var_devLoadConfig();
  }
 
  // erst jetzt: da Daten aus Config_val.sensor[] verwendet werden
  jg18b20Search(ow);
  display_Init();
  Serial.println("setup done");


  timeClient.begin();
 
  display_Icon16(280, 0, wifi_16px);
  server_init();
  //mqtt_client_init();

  menu_init();

 
  menu_display();

  //showIcon32(70,95, kessel_32px);
  //showIcon32(110,95, kessel_32px);
  //showIcon32(150,95, kessel_32px);
  history_init();

  taskManager.init();
  taskManager.addTask(tDisplay);
  taskManager.addTask(tInput);
  taskManager.addTask(tPumpe);
  taskManager.addTask(tVentil);
  taskManager.addTask(tPumpeNachlauf);
  taskManager.addTask(tDs18d20State_1);
  taskManager.addTask(tDs18d20State_2);
  taskManager.addTask(tWebRequest);

  tDisplay.enable();
  tInput.enable();
  tPumpe.enable();
  tVentil.enable();
  tDs18d20State_1.enable();
  tWebRequest.enable();
}


void loop()
{
  //if (loop_active == false)
  //{
    taskManager.execute();
  //}
}
