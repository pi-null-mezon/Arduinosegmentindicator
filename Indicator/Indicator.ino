#include <avr/io.h>
#include <avr/interrupt.h>

//Sensor---------------------------------------------------
#define MIN_MEASURE_DELAY 1000 // it is not a milliseconds, just a cycles number

//Indicator------------------------------------------------
#define DOT_FLAG        1 // indicate a dot with symbol
#define MINUS_FLAG      2 // indicate G segment only
#define BLANKZERO_FLAG  3 // do not indicate zero symbol
#define ANIMATION       4 // animation
#define SYMBOL_T_FLAG   5 // indicate t symbol
#define SYMBOL_E_FLAG   6 // indicate E symbol
#define SEGMENT_A A0
#define SEGMENT_B 10
#define SEGMENT_C 6
#define SEGMENT_D 4
#define SEGMENT_E 3
#define SEGMENT_F 15
#define SEGMENT_G 7
#define SEGMENT_H 5
#define CATHODE_FIRST A2  
#define CATHODE_SECOND 14 
#define CATHODE_THIRD 16   
#define CATHODE_FOURTH 2
#define CATHODE_FIFTH 8 
#define CATHODE_SIX 9  

const byte v_segments[]   = {SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G, SEGMENT_H};
const byte v_digits[]     = {CATHODE_FIRST, CATHODE_SECOND, CATHODE_THIRD, CATHODE_FOURTH, CATHODE_FIFTH, CATHODE_SIX}; 
const byte v_symbolCode[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // a logic here is like in string H G F E D C B A, so A is a first bit, B is a second bit and so on

byte v_symbols[6];
byte v_modifiers[6];
byte  symbol_counter = 0;

unsigned int  animationticks = 0;
byte          animationpos = 0;

void setup() {
  // There are two Serial1 ports on the Leonardo, Serial1 and Serial11.
  // The Serial1 instance is via the onboard USB-to-Serial1 conversion process.
  // Serial11 is via the RX and TX pins
  Serial1.begin(9600); 
  initIndicator();
  for(byte i = 0; i < 6; ++i) {
    v_symbols[i] = 8;
    v_modifiers[i] = DOT_FLAG; 
  }
  // Timer1 initialization
  cli(); // отключить глобальные прерывания
  TCCR1A = 0;
  TCCR1B = 0; 
  OCR1A = 25000; // compare to value 2E16-1 is max, here 2500 is compromise value. Too short value causes unsaturated lights, too long causes visible blinking. 
  TCCR1B |= (1 << WGM12); // enable comparision regime
  TCCR1B |= (1 << CS10); // set frequency divider to 1
  TIMSK1 |= (1 << OCIE1A);  // enable compare interruption
  sei();    
}

ISR(TIMER1_COMPA_vect)
{   
  digitalWrite(v_digits[symbol_counter], HIGH);
  symbol_counter = (symbol_counter + 1) % 6;     
  setSegmentsForValue(v_symbols[symbol_counter], v_modifiers[symbol_counter]);
  digitalWrite(v_digits[symbol_counter], LOW); 
}

//-------------------------------------------------------------------------
void loop() {
  if(Serial1.available()) {
    int incomingbyte = Serial1.read();
    switch(incomingbyte) {
      case 'T': {
        float value = Serial1.parseFloat();
        if(value > 0) {
          v_symbols[0] = (long)(value / 1E2);
          if(v_symbols[0] == 0)
            v_modifiers[0] = SYMBOL_T_FLAG;
          else
            v_modifiers[0] = 0;
          v_symbols[1] = (long)(value / 1E1) % 10;
          if(v_symbols[1] == 0)
            v_modifiers[1] = BLANKZERO_FLAG;
          else
            v_modifiers[1] = 0;
          v_symbols[2] = (long)(value) % 10;
          v_modifiers[2] = 0;
        } else {
          v_modifiers[0] = MINUS_FLAG;
          v_symbols[1] = (long)(-value / 1E1) % 10;
          v_symbols[2] = (long)(-value) % 10;  
        }
      } break;
      case 'B': {
        float value = Serial1.parseFloat();
        if(value > 0 && value < 1000) {
          v_symbols[3] = (long)(value / 1E2);
          v_modifiers[3] = 0;
          v_symbols[4] = (long)(value / 1E1) % 10;
          v_modifiers[4] = 0;
          v_symbols[5] = (long)(value) % 10;
          v_modifiers[5] = 0;
        } else if(value >= 1000) {
          v_symbols[3] = (long)(value / 1E3);
          v_modifiers[3] = DOT_FLAG;
          v_symbols[4] = (long)(value / 1E2) % 10;
          v_modifiers[4] = 0;
          v_symbols[5] = (long)(value / 1E1) % 10;
          v_modifiers[5] = SYMBOL_E_FLAG;  
        } else {
          v_modifiers[3] = MINUS_FLAG;
          v_symbols[4] = (long)(-value / 1E1) % 10;
          v_symbols[5] = (long)(-value) % 10;  
        }
      } break;
      case 'D': {
        for(byte i = 0; i < 6; ++i) {
          v_symbols[i] = 0;
          v_modifiers[i] = BLANKZERO_FLAG; 
        }  
      } break;
      case 'A': {
        for(byte i = 0; i < 6; ++i)
          v_modifiers[i] = ANIMATION;          
      } break;
    }
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
  pinMode(CATHODE_FIFTH, OUTPUT);
    digitalWrite(CATHODE_FIFTH, HIGH);
  pinMode(CATHODE_SIX, OUTPUT);
    digitalWrite(CATHODE_SIX, HIGH);   
}

//-------------------------------------------------------------------------

void setSegmentsForValue(byte value, byte flags) // use v_symbolCode vector for valueCode
{  
  byte valueCode = v_symbolCode[value];
  // Animation counters ------------------
  animationticks++;
  if((animationticks % 40) == 0)
    animationpos = (animationpos + 1) % 6;  
  //--------------------------------------
  switch(flags) {  
    case ANIMATION:               
      valueCode = 0x01 << animationpos;
      break;
      
    case DOT_FLAG:
      valueCode |= 0x80;
      break;
      
    case MINUS_FLAG:
      valueCode = 0x40;
      break;

    case SYMBOL_T_FLAG:
      valueCode = B01111000;
      break;

    case SYMBOL_E_FLAG:
      valueCode = B01111001;
      break;
  }
  if ((value == 0)  && (flags == BLANKZERO_FLAG)) {
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], LOW); 
  } else { 
    for(byte i = 0; i < 8 ; i++)
      digitalWrite(v_segments[i], (valueCode >> i) & 0x01);  
  } 
}
