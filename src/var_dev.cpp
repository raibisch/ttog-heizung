
#include <Arduino.h>
#include <Timezone.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "var_dev.h"

// bool loop_active = false;

struct_menu Menu[5];
struct_config Config_val;
struct_allg Device_allg;
struct_device Device_WWSpeicher, Device_HeizRueckl, Device_BrennerKessel, Device_Pumpe, Device_Wetterstation;

int Menu_akt;
int Menu_ix = 1;

// neu: 23.5.2022 default, min, max
int key2Int(String sBuf, String sKey, int idefault = -1, int imin = 0, int imax = 9999)
{
  int poskey1 = sKey.indexOf('.');
  sKey = sKey.substring(poskey1 + 1, sKey.length() + 1);
  int val = 0;
  int posStart = sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.println("key:'" + sKey + "' not found!!");
    return idefault;
  }
  // Serial.printf("PosStart1:%d", posStart);
  posStart = sBuf.indexOf('=', posStart);
  // Serial.printf("PosStart2:%d", posStart);
  int posEnd = sBuf.indexOf('\r', posStart);
  String sVal = sBuf.substring(posStart + 1, posEnd);
  sVal.trim();

  Serial.print(sKey + ": ");

  val = sVal.toInt();
  if (val < imin)
  {
    val = idefault;
    Serial.printf(" val:%d < min:%d\n", val, imin);
  }
  if (val > imax)
  {
    val = idefault;
    Serial.printf(" val:%d > max:%d\n", val, imax);
  }
  Serial.println(val);
  return val;
}

// neu: 29.1.23
String key2String(String sBuf, String sKey, int minLength)
{
  int poskey1 = sKey.indexOf('.');
  sKey = sKey.substring(poskey1 + 1, sKey.length() + 1);
  int val = 0;
  int posStart = sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.println("key:'" + sKey + "' not found!!");
    return "";
  }
  // Serial.printf("PosStart1:%d", posStart);
  posStart = sBuf.indexOf('=', posStart);
  // Serial.printf("PosStart2:%d", posStart);
  int posEnd = sBuf.indexOf('\r', posStart);
  String sVal = sBuf.substring(posStart + 1, posEnd);
  sVal.trim();
  Serial.print(sKey + ": ");
  return sVal;
}

// Wandelt Source-Code Variablen-Namen in String um :-))
#define GET_VARNAME(Variable) (#Variable)

// ... alles andere war mir zu kompliziert
// Gueltige Beispieleinträge
//-------------------------------
bool var_devLoadConfig()
{
  File configFile = SPIFFS.open("/config.txt");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  Serial.printf("size of config.txt:%d \n", size);
  if (size > 1024)
  {
    Serial.println("Config file size is too large");
    return false;
  }

  String sBuf;
  sBuf = configFile.readString();
  sBuf.replace(" ", "");
  sBuf.replace(";", "");

  /*
  uint8_t netz_manuell               = 0;
    uint8_t ventil_manuell              = 0;

    uint8_t hour_heizung_werktag_on    = 6;
    uint8_t minute_heizung_werktag_on    = 0;

    uint8_t hour_heizung_wochenende_on = 8;
    uint8_t minute_heizung_wochenende_on = 0;

    uint8_t hour_heizung_winter_off    = 23;
    uint8_t minute_heizung_winter_off    = 50;

    uint8_t hour_heizung_sommer_off    = 12;
    uint8_t minuteheizung_sommer_off    = 0;

    uint8_t temp_winter_tag            = 10;
    uint8_t temp_winter_nacht          = -3;

    uint8_t temp_ww_soll_ein           = 41;
    uint8_t temp_ww_soll_aus           = 52;

    uint8_t time_minute_nachlauf   = 5;
    uint8_t time_minute_ww_winter  = 10;

  */

  Config_val.netz_manuell = key2Int(sBuf, GET_VARNAME(Config_val.netz_manuell), 0, 0, 1);
  Config_val.ventil_manuell = key2Int(sBuf, GET_VARNAME(Config_val.ventil_manuell), 0, 0, 1);

  Config_val.hour_heizung_werktag_on = key2Int(sBuf, GET_VARNAME(Config_val.hour_heizung_werktag_on), 6, 4, 12);
  Config_val.minute_heizung_werktag_on = key2Int(sBuf, GET_VARNAME(Config_val.minute_heizung_werktag_on), 0, 0, 59);

  Config_val.hour_heizung_wochenende_on = key2Int(sBuf, GET_VARNAME(Config_val.hour_heizung_wochenende_on), 8, 4, 12);
  Config_val.minute_heizung_wochenende_on = key2Int(sBuf, GET_VARNAME(Config_val.minute_heizung_wochenende_on), 0, 0, 59);

  Config_val.hour_heizung_winter_off = key2Int(sBuf, GET_VARNAME(Config_val.hour_heizung_winter_off), 23, 12, 23);
  Config_val.minute_heizung_winter_off = key2Int(sBuf, GET_VARNAME(Config_val.minute_heizung_winter_off), 0, 0, 59);

  Config_val.hour_heizung_sommer_off = key2Int(sBuf, GET_VARNAME(Config_val.hour_heizung_sommer_off), 14, 12, 23);
  Config_val.minute_heizung_sommer_off = key2Int(sBuf, GET_VARNAME(Config_val.minute_heizung_sommer_off), 0, 0, 59);

  Config_val.temp_winter_tag = key2Int(sBuf, GET_VARNAME(Config_val.temp_winter_tag), 10, 0, 30);
  Config_val.temp_winter_nacht = key2Int(sBuf, GET_VARNAME(Config_val.temp_winter_nacht), -3, -10, 30);

  Config_val.temp_ww_soll_ein = key2Int(sBuf, GET_VARNAME(Config_val.temp_ww_soll_ein), 41, 20, 70);
  Config_val.temp_ww_soll_aus = key2Int(sBuf, GET_VARNAME(Config_val.temp_ww_soll_aus), 42, 20, 70);

  Config_val.mliter_oel_pro_h = key2Int(sBuf, GET_VARNAME(Config_val.mliter_oel_pro_h));

  Config_val.sensor_ix_kessel = key2Int(sBuf, GET_VARNAME(Config_val.sensor_ix_kessel), 0, 0, 3);
  Config_val.sensor_ix_ww = key2Int(sBuf, GET_VARNAME(Config_val.sensor_ix_ww), 1, 0, 3);
  Config_val.sensor_ix_ruecklauf = key2Int(sBuf, GET_VARNAME(Config_val.sensor_ix_ruecklauf), 2, 0, 3);
  Config_val.sensor_ix_aussen = key2Int(sBuf, GET_VARNAME(Config_val.sensor_ix_aussen), 4, 0, 4);
  // z.Z. nicht genutzt
  // Config_val.sensor_ix_vorlauf=          key2Int(sBuf, GET_VARNAME(Config_val.sensor_ix_vorlauf)  ,2,0,3);

  Config_val.time_minute_ww_winter = key2Int(sBuf, GET_VARNAME(Config_val.time_minute_ww_winter), 1, 1, 20);

  // 29.1.23 NEU: fragt Zusstand ab... und schaltet statt Relais den externen SHELLY-Switch
  Config_val.netz_http_switch          = key2String(sBuf, GET_VARNAME(Config_val.netz_http_switch),5);

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
  if (!configFile)
  {
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
