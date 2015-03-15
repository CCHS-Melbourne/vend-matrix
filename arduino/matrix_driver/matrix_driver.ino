//**************************************************************
//
// This code has several functions.
//
// 3x 74HC595 Shift registers in series to turn motors on and off
// 1x CD4051 Shift in register to read button presses on the keypad
// 1x i2c 2x16 character display.
// 1x voltage drop reader
// 1x coin changer [_]
// 
// 
//
//**************************************************************

//Pins connected to the 74HC595 for Motor driver
int latchPin = 4;
int clockPin = 3;
int dataPin = 2;

//Pins connected to the CD4051 for Keypad
int kLatchPin = 8;
int kClockPin = 7;
int kDataPin = 6;

void setup() {
  //set pins Motor driver Pins to output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // set pins keyboard driver pins to output/input
  pinMode(kLatchPin, OUTPUT);
  pinMode(kClockPin, OUTPUT);
  pinMode(kDataPin, INPUT);
  
  
}

void loop() {
  //count up routine
  
  byte anode  =      B10000000;
  byte cathode1_8 =  B01000000;
  byte cathode9_16 = B00000000;
  
  
  digitalWrite(latchPin, LOW);
  
    
    shiftOut(dataPin, clockPin, MSBFIRST, cathode9_16);
    shiftOut(dataPin, clockPin, MSBFIRST, cathode1_8);
    shiftOut(dataPin, clockPin, MSBFIRST, anode);
    
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);
  
  
  delay(10000);
} 
 
