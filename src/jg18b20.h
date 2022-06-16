#ifndef jg18b20_h
#define jg18b20_h

#include <OneWire.h>


void  jg18b20Search(OneWire ow);
//float jg18b20Read(OneWire ds, int x); //wird z.Z. nicht genutzt
void  jg18b20ConvertState_1(OneWire ds);
void  jg18b20ConvertState_2(OneWire ds);

float jg18b20GetValue(int i);

#endif
