#ifndef FILEVARSTORE_H
#define FILEVARSTORE_H

#include <Arduino.h>
#include <SPIFFS.h>


class FileVarStore
{
  public:
     // ADC-Parameter
   static  int varADC_i_Gain;        // Hardware-Einstellung
   static  int varADC_i_ChopDelay;   // Hardware-Einstellung  

   // DAC-Parameter
   static int varDAC_i_Divisor;      // Software-Teiler
   static int varDAC_i_Factor;       // Software-Faktor
   
  //Kalibrierungs-Parameter
   static int varVALUE_i_Tara;
   static int varVALUE_i_KaliZero;
   static int varVALUE_i_KaliSet;
   // Anzeige Faktor
   static int varVALUE_i_Factor;
   static int varVALUE_i_Divisor;
   static int varVALUE_i_Decimal;

   
  // Device-Parameter
   String varDEVICE_s_Name  = "he268S2";
   String varDEVICE_s_Unit  = "xx";

  // Wifi-Parameter
   String varWIFI_s_Mode    = "STA"; // STA=client verbindet sich mit Router,  AP=Access-Point wird erzeugt
   String varWIFI_s_Password= "###ud361309$$$";
   String varWIFI_s_SSID    = "SMC4";

   String varWIFI_s_IpAdr   = "00.00.00.00";
   

    FileVarStore();
    FileVarStore (String filename);
    ~FileVarStore ();
  
    bool isLoaded();
    bool Load();
    bool Save(String s);

private:
    String _filename;
    String _sBuf;
    String GetVarString(String name);
    int GetVarInt(String name);
    int GetVarInt(String name, int defaultvalue);
    float GetVarFloat(String name);

    bool SetVar(String sKey, int iVal); // noch nicht implementiert
    bool _isLoaded = false;
};

#endif