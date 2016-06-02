#include <OneWire.h>

#define ONE_WIRE_PIN A0
#define MEASURE_DELAY 1000
#define MEASURE_RESOLUTION 0.0625

OneWire ds(ONE_WIRE_PIN);
byte addr[8];
bool f_firstrun = true;

void setup()
{
  Serial.begin(9600);
}

void loop()
{ 
  if ( !ds.search(addr)) {
    Serial.print("\nNo more addresses.\n");    
    ds.reset_search();
    f_firstrun = false;
    return;
  }
 
  Serial.print("adr: ");
  for(byte i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
  if(f_firstrun == true) {
    configureDS18B20(addr);
  }  
  Serial.print("\nT: ");
  Serial.print(measureTemperature(addr));
  Serial.print(" Celsius degrees\n");
}

float measureTemperature(byte *addres)
{
  ds.reset();
  ds.select(addres);
  ds.write(0x44,1); // start conversion, with parasite power on at the end 
   
  delay(MEASURE_DELAY); // maybe 750ms is enough, maybe not 
  
  ds.reset();
  ds.select(addres);    
  ds.write(0xBE); // start read scratchpad
  byte lsb = ds.read();
  byte msb = ds.read();
  byte sign = (msb >> 7) & 0x01;
  msb &= 0x0F;
  float result;
  if(sign == 0x01)
     result = - ((int)(~lsb) | ((int)(~msb) << 8)) * MEASURE_RESOLUTION;    
  else
     result = ((int)lsb | ((int)msb << 8)) * MEASURE_RESOLUTION;
  return result; 
}

void configureDS18B20(byte *addres) // setup particular device to measure temperature with 12-bit resolution
{
  Serial.print("\nBurn 12-bit meas. config.");
  
  ds.reset();
  ds.select(addres);
  ds.write(0x4E, 1);
  ds.write(0x01, 1); // alarm top temperature
  ds.write(0x01, 1); // alarm low temperature
  ds.write(0x7F, 1); // configuration register for 12-bit resolution measurements
  
  ds.reset();
  ds.select(addres);
  ds.write(0x48, 1); // write configuration in to DS18B20 EEPROM
  delay(15);  
}

