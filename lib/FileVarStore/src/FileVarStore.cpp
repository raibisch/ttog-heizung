#include "FileVarStore.h"


int FileVarStore::varADC_i_Gain=0b0100;
int FileVarStore::varADC_i_ChopDelay=0b0011;

int FileVarStore::varVALUE_i_Tara=140;
int FileVarStore::varVALUE_i_KaliZero=0;
int FileVarStore::varVALUE_i_KaliSet=0;
int FileVarStore::varVALUE_i_Factor=100;
int FileVarStore::varVALUE_i_Divisor=9241;
int FileVarStore::varVALUE_i_Decimal=0;

int FileVarStore::varDAC_i_Factor=1;
int FileVarStore::varDAC_i_Divisor=124;
//----------------------------------------------------public Functions----------------------------------------------------

//default constructor 
FileVarStore::FileVarStore()
{
  _filename = "/config.txt";
}

// constructor with custom filename
FileVarStore::FileVarStore(String fn)
{
  _filename = "/"+ fn;
}

/*-----------------------------
destructor for class, not needed by Arduino but for complete class. Calls Arduino end function
*/
FileVarStore::~FileVarStore()
{
  
}

// Wandelt Source-Code Variablen-Namen in String um :-))
#define GETVARNAME(Variable) (#Variable)


bool FileVarStore::Load()
{
   File configFile = SPIFFS.open(_filename);
  if (!configFile) 
  {
    Serial.println("Failed to open config file");
    _isLoaded = false;
    return false;
  }

  size_t size = configFile.size();
  Serial.printf("size of config.txt:%d \n", size);
  if (size > 1024) {
    Serial.println("Config file size is too large");
    _isLoaded = false;
    return false;
  }

  _sBuf = configFile.readString();
  _sBuf.replace(" ","");
  varDEVICE_s_Name     = GetVarString(GETVARNAME(varDEVICE_s_Name));
  varWIFI_s_Mode       = GetVarString(GETVARNAME(varWIFI_s_Mode)); //STA or AP
  varDEVICE_s_Unit     = GetVarString(GETVARNAME(varDEVICE_s_Unit));
  varWIFI_s_Password   = GetVarString(GETVARNAME(varWIFI_s_Password));
  varWIFI_s_SSID       = GetVarString(GETVARNAME(varWIFI_s_SSID));
  varADC_i_ChopDelay   = GetVarInt(GETVARNAME(varADC_i_ChopDelay));
  
  varVALUE_i_Tara      = GetVarInt(GETVARNAME(varVALUE_i_Tara),    0);
  varVALUE_i_KaliZero  = GetVarInt(GETVARNAME(varVALUE_i_KaliZero),0);
  varVALUE_i_KaliSet   = GetVarInt(GETVARNAME(varVALUE_i_KaliSet), 0);
  varVALUE_i_Factor    = GetVarInt(GETVARNAME(varVALUE_i_Factor),  1);
  varVALUE_i_Divisor   = GetVarInt(GETVARNAME(varVALUE_i_Divisor), 1);
  varVALUE_i_Decimal   = GetVarInt(GETVARNAME(varVALUE_i_Decimal), 1);

  configFile.close();
  Serial.println("config.txt load OK!");
  _isLoaded = true;
  return true;
}

bool FileVarStore::isLoaded()
{
  return _isLoaded;
}

bool FileVarStore::Save(String s)
{
  File configFile = SPIFFS.open(_filename, "w");
  if (!configFile) 
  {
    Serial.println("ERROR: Failed to open config file for writing");
    return false;
  }
  configFile.print(s);
 
  configFile.close();
  Serial.println("config.txt save OK!");
   // daten neu laden
  Load();
  return true;
}

// private member
String FileVarStore::GetVarString(String sK)
{
  int posStart = _sBuf.indexOf(sK);
  if (posStart < 0)
  {
    Serial.println("key:'"+sK+ "' not found!!");
    return "";
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf(';', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  
  Serial.print(sK+":");
  Serial.print(sVal); 
  Serial.printf(" length:%d\r\n", sVal.length());
  return sVal;
}

int FileVarStore::GetVarInt(String sKey)
{   
  
  uint32_t val = 0;
  int posStart = _sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.println("key:'"+sKey+ "' not found!!");
    return -1;
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf('\n', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  
  Serial.print (sKey+":");
 
  val = sVal.toInt();
  Serial.println(val); 
  return val;
 
}

int FileVarStore::GetVarInt(String sKey, int defaultvalue)
{   
  
  uint32_t val = 0;
  int posStart = _sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.print("key:'"+sKey+ "' not found set defaultvalue: ");
     Serial.println(val); 
    val = defaultvalue;
    return -1;
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf('\n', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  
  Serial.print (sKey+":");
 
  val = sVal.toInt();
  Serial.println(val); 
  return val;
 
}


float FileVarStore::GetVarFloat(String sKey)
{
  float val = 0;
  int posStart = _sBuf.indexOf(sKey);
  if (posStart < 0)
  {
    Serial.println("key:'"+sKey+ "' not found!!");
    return -1;
  }
  //Serial.printf("PosStart1:%d", posStart);
  posStart = _sBuf.indexOf('=', posStart);
  //Serial.printf("PosStart2:%d", posStart);
  int posEnd   = _sBuf.indexOf('\n', posStart);
  String sVal = _sBuf.substring(posStart+1,posEnd); 
  
  Serial.print (sKey+":");
 
  val = sVal.toFloat();
  Serial.println(val); 
  return val;
}

bool FileVarStore::SetVar(String sKey, int iVal)
{
  Serial.println("FileVarStor::SetVar !!!MACHT NOCH NICHTS !!!")
  return true;
}





