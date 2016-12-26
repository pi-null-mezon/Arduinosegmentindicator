#include <DHT.h>
#include <DHT_U.h>
/*
 Location note:
 В этом скетче распиновка соответсвует плате индикации на базе Arduino micro,
 которая будет установлена в Злынке у Т.М. 
*/

//Sensor---------------------------------------------------
#define DHTPIN 13              // what digital pin we're connected to
#define DHTTYPE DHT22 // AM2302 is the same but from the other manufacturer
#define MIN_MEASURE_DELAY 1500 // it is not a milliseconds, just a cycles number
DHT dht(DHTPIN, DHTTYPE);

//Indicator------------------------------------------------
#define DOT_FLAG        0x01 // indicate a dot with symbol
#define MINUS_FLAG      0x02 // indicate G segment only
#define BLANKZERO_FLAG  0x04 // do not indicate zero symbol
#define SEGMENT_A   4
#define SEGMENT_B   8  
#define SEGMENT_C   12 
#define SEGMENT_D   11
#define SEGMENT_E   10  
#define SEGMENT_F   5
#define SEGMENT_G   A5
#define SEGMENT_H   A0
#define CATHODE_FIRST  9
#define CATHODE_SECOND 7
#define CATHODE_THIRD  6 
#define CATHODE_FOURTH 3

const byte v_segments[]   = {SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G, SEGMENT_H};
const byte v_digits[]     = {CATHODE_FIRST, CATHODE_SECOND, CATHODE_THIRD, CATHODE_FOURTH}; 
const byte v_symbolCode[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // a logic here is like in string H G F E D C B A, so A is a first bit, B is a second bit and so on

#define SYMBOL_H_FLAG 0x08
#define SYMBOL_T_FLAG 0x10

//Miscellaneous--------------------------------------------------
#define MEASUREMENTS_REGIME 0x01
#define ANALOGREAD_REGIME 0x02
#define PARSESERIAL_REGIME 0x03
#define ANALOG_PIN A3
#define BUTTON_PIN 2
uint16_t  measureDelay = MIN_MEASURE_DELAY; 
byte      v_symbols[4];
float     measure;
float     v_measure[2];
byte      m_sensors;
int       analogvalue;
byte      regime = MEASUREMENTS_REGIME; 
byte      valuetypeFlag;
//-------------------------------------------------------------------------

void setup() {
  initIndicator();
  dht.begin();
}

//-------------------------------------------------------------------------
void loop() {
  
  switch(regime) {    
    case MEASUREMENTS_REGIME:
    
       for(byte k = 0 ; k < 2; k++) {
         if(k == 0)
            valuetypeFlag = SYMBOL_H_FLAG;
         else
            valuetypeFlag = SYMBOL_T_FLAG;
            
          measure = v_measure[k];        
          v_symbols[0] = (byte)(measure*10) % 10;
          v_symbols[1] = (byte)measure % 10;
          v_symbols[2] = (byte)(measure/10) % 10;         
          for(byte i = 0; i < 4; i++)
            for(uint16_t j = 0; j < (measureDelay >> 3); j++) {
                setSymbol(0, v_symbols[0], 0);
                setSymbol(1, i > 0 ? v_symbols[1]:0, ((i > 0) && (v_symbols[1] == 0)) ? 0 : BLANKZERO_FLAG);
                setSymbol(2, i > 1 ? v_symbols[2]:0, ((i > 1) && (v_symbols[2] == 0)) ? 0 : BLANKZERO_FLAG);
                setSymbol(3, 0, (i > 2)? valuetypeFlag : BLANKZERO_FLAG);
            }
          for(uint16_t j = 0; j < measureDelay; j++) {
              setSymbol(0, v_symbols[0], 0);
              setSymbol(1, v_symbols[1], DOT_FLAG);
              setSymbol(2, v_symbols[2], 0);
              setSymbol(3, v_symbols[3], valuetypeFlag);
          } 
          for(byte i = 0; i < 4; i++)
            for(uint16_t j = 0; j < (measureDelay >> 3); j++) {
                setSymbol(0, i > 2 ? 0:v_symbols[0], ((i < 3) && (v_symbols[0] == 0)) ? 0 : BLANKZERO_FLAG);
                setSymbol(1, i > 1 ? 0:v_symbols[1], ((i < 2) && (v_symbols[1] == 0)) ? 0 : BLANKZERO_FLAG);
                setSymbol(2, i > 0 ? 0:v_symbols[2], ((i < 1) && (v_symbols[2] == 0)) ? 0 : BLANKZERO_FLAG);
                setSymbol(3, 0, BLANKZERO_FLAG);
            }
      }
      readDHT22();     
      analogvalue = analogRead(ANALOG_PIN); 
      measureDelay = MIN_MEASURE_DELAY + (analogvalue << 4);     
      break;
  }
    
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

//-------------------------------------------------------------------------

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
  } else if((flags & SYMBOL_H_FLAG) == SYMBOL_H_FLAG) {
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (0x76 >> i) & 0x01);
  } else if((flags & SYMBOL_T_FLAG) == SYMBOL_T_FLAG) {
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (0x78 >> i) & 0x01);
  } else if ((value == 0)  && ((flags & BLANKZERO_FLAG) == BLANKZERO_FLAG)) {
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], LOW); 
  } else { 
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (valueCode >> i) & 0x01);  
  }
 
  // turn specified digit on (followed optimization digitalWrite(v_digits[digit], LOW), becomes to unsaturated lights, so leave this peace of code as is) 
  for(byte i = 0; i < 4; i++)
    digitalWrite(v_digits[i], i == digit ? LOW : HIGH);
}

//-------------------------------------------------------------------------

void buttonPressed()
{
  delayMicroseconds(1000);
  if(digitalRead(BUTTON_PIN) == LOW) {
    delayMicroseconds(1000);
    if(digitalRead(BUTTON_PIN) == LOW) {
      measureDelay = 0;
      switch(regime)  {
        case MEASUREMENTS_REGIME:
          regime = ANALOGREAD_REGIME;
          break;
        case ANALOGREAD_REGIME:
          regime = PARSESERIAL_REGIME;
          break;
        case PARSESERIAL_REGIME:
          regime = MEASUREMENTS_REGIME; 
          break;
      }
    }
  }
}
//----------------------------------------------------------------------

void readDHT22()
{
  noInterrupts();
  v_measure[0] = dht.readHumidity();
  v_measure[1] = dht.readTemperature();
  interrupts(); 
}

//----------------------------------------------------------------------
