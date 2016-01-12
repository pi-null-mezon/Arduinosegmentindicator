#include <OneWire.h>
//Sensor---------------------------------------------------
#define ONE_WIRE_PIN A0
#define MEASURE_DELAY 2500
#define MEASURE_RESOLUTION 0.0625
OneWire ds(ONE_WIRE_PIN);
//Indicator------------------------------------------------
#define DOT_FLAG        0x01 // indicate a dot with symbol
#define MINUS_FLAG      0x02 // indicate G segment only
#define BLANKZERO_FLAG  0x04 // do not indicate zero symbol
#define SEGMENT_A 6
#define SEGMENT_B 8
#define SEGMENT_C 10
#define SEGMENT_D 12
#define SEGMENT_E 13
#define SEGMENT_F 7
#define SEGMENT_G 9
#define SEGMENT_H 11
#define CATHODE_FIRST   5
#define CATHODE_SECOND  4
#define CATHODE_THIRD   3 
#define CATHODE_FOURTH  2
byte v_segments[]   = {SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G, SEGMENT_H};
byte v_digits[]     = {CATHODE_FIRST, CATHODE_SECOND, CATHODE_THIRD, CATHODE_FOURTH}; 
byte v_symbolCode[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // a logic here is like in string H G F E D C B A, so A is a first bit, B is a second bit and so on
//------------------------------------------------------

byte  v_symbols[4];
float measure;
byte  addr[8]; 

//-------------------------------------------------------------------------

void setup() {
  initIndicator();
  initSensor(addr);  
}

//-------------------------------------------------------------------------
void loop() {
  startConversion(addr);
  if(measure < 0.0) {
    measure = - measure;
    v_symbols[0] = (byte)(measure*10) % 10;
    v_symbols[1] = (byte)measure % 10;
    v_symbols[2] = (byte)(measure/10) % 10;
    for(int j = 0; j < MEASURE_DELAY; j++) {
        setSymbol(0, v_symbols[0], 0);
        setSymbol(1, v_symbols[1], DOT_FLAG);
        setSymbol(2, v_symbols[2], BLANKZERO_FLAG);
        setSymbol(3, v_symbols[3], MINUS_FLAG);
    }   
  } else {
    v_symbols[0] = (byte)(measure*100) % 10;
    v_symbols[1] = (byte)(measure*10) % 10;
    v_symbols[2] = (byte)measure % 10;
    v_symbols[3] = (byte)(measure/10) % 10;
    for(int j = 0; j < MEASURE_DELAY; j++) {
        setSymbol(0, v_symbols[0], 0);
        setSymbol(1, v_symbols[1], 0);
        setSymbol(2, v_symbols[2], DOT_FLAG);
        setSymbol(3, v_symbols[3], BLANKZERO_FLAG);
    } 
  }
  for(int j = 0; j < 4; j++)
    setSymbol(j, 0, BLANKZERO_FLAG);  
  measure = readSensor(addr);     
}

//-------------------------------------------------------------------------

void initIndicator()
{
  pinMode(SEGMENT_A, OUTPUT);
    digitalWrite(SEGMENT_A, HIGH);
  pinMode(SEGMENT_B, OUTPUT);
    digitalWrite(SEGMENT_B, HIGH);
  pinMode(SEGMENT_C, OUTPUT);
    digitalWrite(SEGMENT_C, HIGH);
  pinMode(SEGMENT_D, OUTPUT);
    digitalWrite(SEGMENT_D, HIGH);
  pinMode(SEGMENT_E, OUTPUT);
    digitalWrite(SEGMENT_E, HIGH);
  pinMode(SEGMENT_F, OUTPUT);
    digitalWrite(SEGMENT_F, HIGH);
  pinMode(SEGMENT_G, OUTPUT);
    digitalWrite(SEGMENT_G, HIGH);
  pinMode(SEGMENT_H, OUTPUT);
    digitalWrite(SEGMENT_H, HIGH);

  pinMode(CATHODE_FIRST, OUTPUT);
    digitalWrite(CATHODE_FIRST, HIGH);
  pinMode(CATHODE_SECOND, OUTPUT);
    digitalWrite(CATHODE_SECOND, HIGH);
  pinMode(CATHODE_THIRD, OUTPUT);
    digitalWrite(CATHODE_THIRD, HIGH);
  pinMode(CATHODE_FOURTH, OUTPUT);
    digitalWrite(CATHODE_FOURTH, HIGH);   
}

void setSymbol(byte digit, byte value, byte flags) // use v_symbolCode vector for valueCode
{
  // turn all digits off
  for(byte i = 0; i < 4; i++)
    digitalWrite(v_digits[i], HIGH);

  byte valueCode = v_symbolCode[value];  
  if((flags & DOT_FLAG) == DOT_FLAG)
    valueCode |= 0x80;
  if((flags & MINUS_FLAG) == MINUS_FLAG) {  
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (0x40 >> i) & 0x01);  
  } else if ((value == 0)  && ((flags & BLANKZERO_FLAG) == BLANKZERO_FLAG)){
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], LOW);   
  } else {
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (valueCode >> i) & 0x01);  
  }
 
  // turn specified digit on
  for(byte i = 0; i < 4; i++)
    digitalWrite(v_digits[i], i == digit ? LOW : HIGH);
}

//-------------------------------------------------------------------------
uint8_t initSensor(byte *addres)
{
  return ds.search(addres); 
}

void startConversion(byte *addres)
{
  ds.reset();
  ds.select(addres);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end  
}

float readSensor(byte *addres)
{
  ds.reset();
  ds.select(addres);    
  ds.write(0xBE); // start read scratchpad
  byte lsb = ds.read();  
  byte msb = ds.read();
  byte sign = msb & 0xF0;
  if(sign == 0xF0)
    return -((int)((byte)~lsb) | ((int)((byte)~msb) << 8)) * MEASURE_RESOLUTION;
  else
    return ((int)lsb | ((int)msb << 8)) * MEASURE_RESOLUTION;  
}

//----------------------------------------------------------------------
