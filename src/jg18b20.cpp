/*
  JG 26.1.2019
  Onewire Implementierung nur für DS18b20
*/


#include <OneWire.h>
#include "jg18b20.h"
#include "var_dev.h"

#define MAX_SENSORS 4


//byte owROM_0[0x28, 0xFF, 0x6D, 0x1A, 0xB1, 0x17, 0x05, 0x3C];
//byte owROM_1[0x28, 0xFF, 0xAF, 0x30, 0xB1, 0x17, 0x04, 0xA8];
struct DS18X20STRUCT {
  uint8_t address[8];
  uint8_t valid = 0;
  float   temperature = -99;
} ds18x20_sensor[MAX_SENSORS];

int aktSensor = 0; // zum durchzaehlen beim State_1/State_2;
byte convert_state = 0;
byte owROM[8];

/* z.Z. nicht benutzt
void showAddress(byte SensorIndex, byte address[8])
  {
  // Im Array address enthaltene Daten kompakt sedezimal ausgeben
  byte i;

  if (OneWire::crc8(address, 7) != address[7])
    { Serial.println("hat keinen gueltigen CRC!"); }
  else
    {
    //alles ist ok, anzeigen
    for (i = 0; i < 8; i++)
      {
      if (address[i] <= 0xF)
        { Serial.print("0"); }
        ds18x20_sensor[SensorIndex].address[i] = owROM[i];
      Serial.print(address[i],HEX);
      }
    Serial.println("");
    }
  }
*/

/* z.Z. nicht benutzt
void lookUpSensors(OneWire ow)
  {
  // Alle DS18x20-Sensoren suchen, Adresse ausgeben
  byte address[8];
  bool found = false;
  bool detect = false;
  byte ix = 0;
 
  Serial.println("--Suche gestartet--");
  while (ow.search(address))
    {
    detect = false;
    switch (address[0])
      {
      case 0x10:
        Serial.print("DS18S20: ");  // oder alter DS1820
        detect = true;
        break;
      case 0x28:
        Serial.print("DS18B20: ");
        detect = true;
        break;
      case 0x22:
        Serial.print("DS1822: ");
        detect = true;
        break;
      default:
        Serial.println("Device ist kein DS18x20-Sensor.");
      } 
    // Adresse anzeigen, wenn ein Temperatursensor gefunden wurde
    if (detect)
      {
      found = true;
      showAddress(ix, address);
      } // ... if detect
    }// ... while
  if (!found)
    { Serial.println("Keine Sensoren gefunden"); }
  Serial.println("--Suche beendet--");
  ow.reset_search();
  }
*/


////////////////////////////////////////////////////////////////////
// Sucht alle Sensoren und trägt ROM-Kennung in ds18x20_sensor[] ein
////////////////////////////////////////////////////////////////////
void jg18b20Search(OneWire ow)
{
  ow.reset_search();
  delay(500);
  Serial.println("18b20 Search START !");
  int ix = 0;
  byte oldRomLastByte = 0x00;
  delay(500);
   for (int i=1; i<=4; i++)
   {
    if (ow.search(owROM))
    {
      if (oldRomLastByte == owROM[7])
      {
        break;
      }
      oldRomLastByte = owROM[7];
      uint8_t is = Config_val.sensor_ix[ix];
       Serial.print("sensor_ix:");
      Serial.println(ix);
     
      Serial.print(" Device:");
      Serial.print(is);
      Serial.print(" ROM=");
     
     
      for (int ii = 0; ii < 8;  ii++)
      {
       
        ds18x20_sensor[is].address[ii] = owROM[ii];
         Serial.print(owROM[ii], HEX);
      }
      ds18x20_sensor[is].valid = 1;
      ix++;
      Serial.println();
      delay(500);
    }
    else
    {
      Serial.println("No more addresses.");
      Serial.println();
      delay(500);
    }
   }
   ow.reset_search();
  Serial.print("Nr of Devices:");
  Serial.println(ix);

}


void jg18b20ConvertState_1(OneWire ds)
{
    //Serial.println("** Start Convert_1 ****");
    //Serial.print("Sensor:");
    //Serial.println(aktSensor);

    ds.reset();
    ds.select(ds18x20_sensor[aktSensor].address);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end
}

void jg18b20ConvertState_2(OneWire ds)
{
  //Serial.println("** Start Convert_2 ****");

  byte data[12];
  float celsius;

  ds.reset();
  ds.select(ds18x20_sensor[aktSensor].address);
  ds.write(0xBE);         // Read Scratchpad

  for (int i = 0; i < 9; i++)
  {     // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }

  int16_t raw = (data[1] << 8) | data[0];

   //byte cfg = (data[4] & 0x60);
   // at lower res, the low bits are undefined, so let's zero them

   //if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
   //else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
   //else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

   //// default is 12 bit resolution, 750 ms conversion time

   celsius = (float)raw / 16.0;
   celsius = round((celsius * 10))/10;


  if (celsius < 100) // kleine Wertueberpruefung
  {
    ds18x20_sensor[aktSensor].temperature= celsius;
  }
  //else
  //{
    //celsius = -99;
  //}
  Serial.printf("Sensor:%d Temp:%02.1f\n",aktSensor+1, celsius);

  aktSensor++;
  
  if (aktSensor >= 4)
  {
    aktSensor = 0;
  }
  
}

float jg18b20GetValue(int i)
{
   return ds18x20_sensor[i].temperature;
}

////////////////////////////////////////////////////////////////
// Sensor-Wert lesen (evt mit Schleife über Anzahl der Sensoren)
// Wert in Grad Celsius
// vorher muessen die ROM-Werte mit Search ermittelt werden
////////////////////////////////////////////////////////////////
float jg18b20Read(OneWire ds, int x)
{
   Serial.println("----Start 18b20Read--------");
   //byte present = 0;
   //byte type_s = 1;
   byte data[12];
   //byte addr[8];
   float celsius;

     if   (ds18x20_sensor[x].valid == 1)
     {
           ds.reset();
           ds.select(ds18x20_sensor[x].address);
           ds.write(0x44, 1);        // start conversion, with parasite power on at the end
           delay(900);

           ds.reset();
           ds.select(ds18x20_sensor[x].address);
           ds.write(0xBE);         // Read Scratchpad

           for (int i = 0; i < 9; i++)
           {           // we need 9 bytes
             data[i] = ds.read();
             //Serial.print(data[i], HEX);
             //Serial.print(" ");
           }

           int16_t raw = (data[1] << 8) | data[0];

            byte cfg = (data[4] & 0x60);
            // at lower res, the low bits are undefined, so let's zero them
            if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
            else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
            else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
            //// default is 12 bit resolution, 750 ms conversion time

           celsius = (float)raw / 16.0;
           celsius = round((celsius * 10))/10;


           if (celsius < 100) // kleine Wertueberpruefung
           {
             ds18x20_sensor[x].temperature= celsius;
           }
           else
           {
             celsius = -99;
           }
           Serial.print("Sensor:");
           Serial.print (x+1);
           Serial.print("  Temperature = ");
           Serial.print(celsius);
           Serial.println(" Celsius, ");
     }
     else
     {
       celsius = -99; // Fehler Wert;
     }

     return celsius;

}
