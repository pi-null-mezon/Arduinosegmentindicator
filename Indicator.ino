//Indicator------------------------------------------------
#define DOT_FLAG        0x01 // indicate a dot with symbol
#define MINUS_FLAG      0x02 // indicate G segment only
#define BLANKZERO_FLAG  0x04 // do not indicate zero symbol
#define SEGMENT_A A4
#define SEGMENT_B 9
#define SEGMENT_C 6
#define SEGMENT_D 4
#define SEGMENT_E 3
#define SEGMENT_F 12
#define SEGMENT_G 7
#define SEGMENT_H 5
#define CATHODE_FIRST   8
#define CATHODE_SECOND  10
#define CATHODE_THIRD   11 
#define CATHODE_FOURTH  A5
const byte v_segments[]   = {SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G, SEGMENT_H};
const byte v_digits[]     = {CATHODE_FIRST, CATHODE_SECOND, CATHODE_THIRD, CATHODE_FOURTH}; 
const byte v_symbolCode[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // a logic here is like in string H G F E D C B A, so A is a first bit, B is a second bit and so on

//Miscellaneous--------------------------------------------------
#define MEASUREMENTS_REGIME 0x01
#define ANALOGREAD_REGIME 0x02
#define SERIALREAD_REGIME 0x03
#define ANALOG_PIN A1
#define BUTTON_PIN 2
#define MAX_SENSORS_COUNT 8
uint16_t  fadeCycles = 500; 
byte      v_symbols[4];
int       value;
byte      regime = SERIALREAD_REGIME; 
//-------------------------------------------------------------------------

void setup() {
  // Initialize custom board facilities
  initIndicator();
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  //attachInterrupt(0, buttonPressed, FALLING); // 0 interrupt for Arduino Uno linked with 2 GIO 
  Serial.begin(9600);
}

//-------------------------------------------------------------------------
void loop() {
  indicate();
  switch(regime) {    
    case ANALOGREAD_REGIME:
      value = analogRead(ANALOG_PIN);
      v_symbols[0] = (byte)(value % 10);
      v_symbols[1] = (byte)((value/10) % 10);
      v_symbols[2] = (byte)((value/100) % 10);
      v_symbols[3] = (byte)((value/1000) % 10);     
      break;           
  }  
}

void serialEvent() {   
  for(byte i = 0; i < 4; i++)
    for(uint16_t j = 0; j < fadeCycles; j++) {
        setSymbol(0, 0, BLANKZERO_FLAG);
        setSymbol(1, i > 0 ? 0:v_symbols[1], (i > 0) ? BLANKZERO_FLAG : 0);
        setSymbol(2, i > 1 ? 0:v_symbols[2], (i > 1) ? BLANKZERO_FLAG : 0);
        setSymbol(3, i > 2 ? 0:v_symbols[3], (i > 2) ? BLANKZERO_FLAG : 0);
    }  
    value = Serial.parseInt();
    v_symbols[0] = (byte)(value % 10);
    v_symbols[1] = (byte)((value/10) % 10);
    v_symbols[2] = (byte)((value/100) % 10);
    v_symbols[3] = (byte)((value/1000) % 10); 
    for(byte i = 0; i < 4; i++)
    for(uint16_t j = 0; j < fadeCycles; j++) {
        setSymbol(0, v_symbols[0], 0);
        setSymbol(1, i > 0 ? v_symbols[1]:0, ((i > 0) && (v_symbols[1] == 0)) ? 0 : BLANKZERO_FLAG);
        setSymbol(2, i > 1 ? v_symbols[2]:0, ((i > 1) && (v_symbols[2] == 0)) ? 0 : BLANKZERO_FLAG);
        setSymbol(3, i > 2 ? v_symbols[3]:0, ((i > 2) && (v_symbols[3] == 0)) ? 0 : BLANKZERO_FLAG);
    }   
}

void indicate()
{
    setSymbol(0, v_symbols[0], v_symbols[1] > 0 ? 0: BLANKZERO_FLAG);
    setSymbol(1, v_symbols[1], v_symbols[2] > 0 ? 0: BLANKZERO_FLAG);
    setSymbol(2, v_symbols[2], v_symbols[3] > 0 ? 0: BLANKZERO_FLAG);
    setSymbol(3, v_symbols[3], BLANKZERO_FLAG); 
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

//----------------------------------------------------------------------

void buttonPressed()
{
  delayMicroseconds(1000);
  if(digitalRead(BUTTON_PIN) == LOW) {
    delayMicroseconds(1000);
    if(digitalRead(BUTTON_PIN) == LOW) {
      fadeCycles = 0;
      switch(regime)  {
        case MEASUREMENTS_REGIME:
          regime = ANALOGREAD_REGIME;
          break;
        case ANALOGREAD_REGIME:
          regime = MEASUREMENTS_REGIME;
          break;    
      }
    }
  }
}

//----------------------------------------------------------------------
