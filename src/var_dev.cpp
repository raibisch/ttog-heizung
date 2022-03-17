
#include <Arduino.h>
#include <Timezone.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "var_dev.h"


bool loop_active = false;

struct_menu Menu[5];
struct_config Config_val;
struct_allg Device_allg;
struct_device Device_WWSpeicher, Device_Heizung, Device_BrennerKessel, Device_Pumpe;

int Menu_akt;
int Menu_ix = 1;



int key2int(String sBuf, String sKey)
{
  int poskey1 = sKey.indexOf('.');
  sKey = sKey.substring(poskey1+1, sKey.length()+1);
  uint32_t val = 0;
  int posStart = sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.println("key:'"+sKey+ "' not found!!");
    return -1;
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = sBuf.indexOf('\r', posStart);
  String sVal = sBuf.substring(posStart+1,posEnd); 
  
  Serial.print (sKey+": ");
 
  val = sVal.toInt();
  Serial.println(val); 
  return val;
}

// Wandelt Source-Code Variablen-Namen in String um :-))
#define GET_VARNAME(Variable) (#Variable)

// ... alles andere war mir zu kompliziert
// Gueltige Beispieleinträge
//
//-------------------------------
bool var_devLoadConfig() 
{
  File configFile = SPIFFS.open("/config.txt");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  Serial.printf("size of config.txt:%d \n", size);
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  String sBuf;
  sBuf = configFile.readString();
  sBuf.replace(" ","");
  sBuf.replace(";","");
 
  Config_val.device_manuell=             key2int(sBuf, GET_VARNAME(Config_val.device_manuell));
  Config_val.hour_heizung_nacht_off =    key2int(sBuf, GET_VARNAME(Config_val.hour_heizung_nacht_off));
  Config_val.hour_heizung_werktag_on=    key2int(sBuf, GET_VARNAME(Config_val.hour_heizung_werktag_on));
  Config_val.hour_heizung_wochenende_on= key2int(sBuf, GET_VARNAME(Config_val.hour_heizung_wochenende_on)); 
  Config_val.hour_ww_sommer_off =        key2int(sBuf, GET_VARNAME(Config_val.hour_ww_sommer_off));
  Config_val.temp_winterbetrieb =        key2int(sBuf, GET_VARNAME(Config_val.temp_winterbetrieb));
  Config_val.temp_ww_soll_aus =          key2int(sBuf, GET_VARNAME(Config_val.temp_ww_soll_aus));
  Config_val.temp_ww_soll_ein=           key2int(sBuf, GET_VARNAME(Config_val.temp_ww_soll_ein));
  Config_val.mliter_oel_pro_h =          key2int(sBuf, GET_VARNAME(Config_val.mliter_oel_pro_h));
  Config_val.sensor_ix[0]=               key2int(sBuf, GET_VARNAME(Config_val.sensor_ix[0]));
  Config_val.sensor_ix[1]=               key2int(sBuf, GET_VARNAME(Config_val.sensor_ix[1]));
  Config_val.sensor_ix[2]=               key2int(sBuf, GET_VARNAME(Config_val.sensor_ix[2]));
  Config_val.sensor_ix[3]=               key2int(sBuf, GET_VARNAME(Config_val.sensor_ix[3]));
 
  configFile.close();
  Serial.println("config.txt load OK!");
  return true;
}

//--------------------------------------------
//
//--------------------------------------------
bool var_devSaveConfig(String s) 
{
  
  File configFile = SPIFFS.open("/config.txt", "w");
  if (!configFile) {
    Serial.println("ERROR: Failed to open config file for writing");
    return false;
  }
  configFile.print(s);
 
  configFile.close();
  Serial.println("config.txt save OK!");
   // daten neu laden
  var_devLoadConfig();
  return true;
}
