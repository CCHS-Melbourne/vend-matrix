//**************************************************************//
//  Name    : shiftOutCode, Hello World                         //
//  Author  : Carlyn Maw,Tom Igoe                               //
//  Date    : 25 Oct, 2006                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                            //
//****************************************************************

//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 3;
////Pin connected to DS of 74HC595
int dataPin = 2;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  //count up routine
  
  byte anode  =      B10000000;
  byte cathode1_8 =  B11000000;
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
 
