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
void t4_PumpeCallback();
void t5_PumpeNachlaufCallback();
void t6_ds18d20S1Callback();
void t7_ds18d20S2Callback();
void t8_PingOrReset();


Task tDisplay      (1000, TASK_FOREVER, &t1_DisplayCallback);
Task tInput        (500,  TASK_FOREVER, &t2_InputCallback);
Task tVentil       (500, TASK_FOREVER, &t3_VentilCallback);
Task tPumpe        (500, TASK_FOREVER, &t4_PumpeCallback);
Task tPumpeNachlauf(1, TASK_FOREVER, &t5_PumpeNachlaufCallback );

Task tDs18d20State_1(4000, TASK_FOREVER, &t6_ds18d20S1Callback);
Task tDs18d20State_2(1, TASK_ONCE, &t7_ds18d20S2Callback);
//Task tPingOrReset   (60000, TASK_FOREVER, &t8_PingOrReset);

Scheduler taskManager;

void io_init()
{
  pinMode(OUT_RELAIS1, OUTPUT);
  pinMode(OUT_RELAIS2, OUTPUT);
  pinMode(IN_BRENNER, INPUT_PULLUP);
  pinMode(IN_ERROR,   INPUT_PULLUP);

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
  if (timeClient.getHours()==0 && timeClient.getMinutes()==0 && timeClient.getSeconds()< 10)
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
  display_Temp(151, Device_Heizung.temp_akt);
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
  // Achtung 0 = aktiv
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

  /*
  if (digitalRead(26))
  {
    display_Text9pt(0,14,"1");
  }
  else
  {
    display_Text9pt(0,14,"0");
  }
  */



  // ----------- IO Stoerung -------------------
  // Achtung 0 = Störung !!!
  /*
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
// ***********************************************
void t3_VentilCallback()
{
   if (Config_val.device_manuell == 1)
   {
     return;
   }
   //Serial.println("--> VentilCallback");
   if (Device_WWSpeicher.temp_akt < Device_WWSpeicher.temp_soll)
   {
     SetRelaisVentil(true);
   }
   else
   {
     SetRelaisVentil(false);
   }

}

// Task Pumpe Steuerung
// auch hier kann noch etwas mehr "Intelligenz" eingebaut werden
static bool bNachlauf_active = false;
// ***********************************************
void t4_PumpeCallback()
{
   Serial.println("--> PumpeCallback");
  if ((Device_BrennerKessel.active))
  {
    Serial.println("NETZ set active");
    SetRelaisNetz(_EIN);
    return;
  }

  // Output Pumpe (=Hauptschalter) steuern
  int on_time=Config_val.hour_heizung_werktag_on;
  if ((timeClient.getDay() == 7) || timeClient.getDay() == 0)
  {
    on_time = Config_val.hour_heizung_wochenende_on;
  }

  if ((timeClient.getHours() > on_time) && (timeClient.getHours() < Config_val.hour_heizung_nacht_off))
  {
     Serial.printf("Zeitschaltung AKTIV On-hour:%d, Off-hour:%d\n", on_time, Config_val.hour_heizung_nacht_off);
     SetRelaisNetz(_EIN);
     return;
  }
   

  if ((bNachlauf_active == false))
  {
    bNachlauf_active = true;
    Serial.println("--> Start Pumpe-Nachlauf");
    //tPumpeNachlauf.restartDelayed(Device_Pumpe.nachlauf);
    tPumpeNachlauf.enableDelayed(Device_allg.nachlauf);
  }

}

void t5_PumpeNachlaufCallback()
{
  bNachlauf_active = false;
  Serial.println("--> PumpeNachlaufCallback");
  if (Config_val.device_manuell==1)
  { 
     tPumpeNachlauf.disable();
     return; 
  }

  if  (Device_BrennerKessel.active == false)
  {
    SetRelaisNetz(_AUS);
    tPumpeNachlauf.disable();
  }
  
}

void t6_ds18d20S1Callback()
{
  //Serial.println("--> ds1820_S1 Callback");
  jg18b20ConvertState_1(ow);

  tDs18d20State_2.restartDelayed(850);
  tDs18d20State_2.enable();


}

// Arbeitsende 1.2.2021 !!! JG
#define IX_BRENNERKESSEL 0
#define IX_WWSPEICHER    1
#define IX_VLHEIZUNG     2
#define IX_AUSSEN        3  
float tmpf;         
void t7_ds18d20S2Callback()
{
   //Serial.println("--> ds1820_S2 Callback");
   jg18b20ConvertState_2(ow);
  
   tmpf = jg18b20GetValue(IX_BRENNERKESSEL);
   if (tmpf > 0) { Device_BrennerKessel.temp_akt = tmpf; }

   tmpf = jg18b20GetValue(IX_WWSPEICHER);
   if (tmpf > 0){ Device_WWSpeicher.temp_akt    = tmpf; }

   tmpf = jg18b20GetValue(IX_VLHEIZUNG);
   if (tmpf > 0) { Device_Heizung.temp_akt    = jg18b20GetValue(IX_VLHEIZUNG); }
   
   tmpf = jg18b20GetValue(IX_AUSSEN);
   if (tmpf > -30) { Device_allg.temp_aussen   = jg18b20GetValue(IX_AUSSEN); }

}


void t8_PingOrReset()
{
  if (!isFritzBoxPing())
  {
    delay(500);
    history_save();
    ESP.restart();
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




//****** S E T U P ***********************************************
void setup()
{

  io_init();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  delay(1000);
  //display_Text9pt(187,0,".CONNECT");
  int i = 0;
  WiFi.setHostname("Heizungtest");
  while ( WiFi.status() != WL_CONNECTED )
  {
    i++;
    display_Text9pt(187,0,".CONNECT");
    delay ( 500 );
    display_Text9pt(187,0,"          ");
    Serial.print(".");
    if (i > 5)
    {
      ESP.restart();
    }
  }
  display_Text9pt(187,0, ssid);

 
  //delay(500);
 

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

 // by JG TEST
 
 display_Icon16(280, 0, wifi_16px);
 server_init();

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
 //taskManager.addTask(tPingOrReset);

 
 tDisplay.enable();
 tInput.enable();
 tPumpe.enable();
 tVentil.enable();
 tDs18d20State_1.enable();
 //tPingOrReset.enable();

}


void loop()
{
  //if (loop_active == false)
  //{
    taskManager.execute();
  //}
}
