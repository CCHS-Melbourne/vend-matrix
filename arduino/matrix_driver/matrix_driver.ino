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

// Include LCD.
#include <Wire.h>
//#include <LCD.h>
//#include <LiquidCrystal_I2C.h>

// Include NFC reader
/*
  #include <PN532_HSU.h>
  #include <PN532.h>
      
  PN532_HSU pn532hsu(Serial);
  PN532 nfc(pn532hsu);
*/

// include network setup.
//#include <SPI.h>
//#include <Ethernet.h>


// LCD setup
//LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

//Pins connected to the 74HC595 for Motor driver
const int latchPin = 4;
const int clockPin = 3;
const int dataPin = 2;

//Pins connected to the CD4051 for Keypad
const int kLatchPin = 8;
const int kClockPin = 7;
const int kDataPin = 6;

//Pins connected to current sense
const int sensorPin = A0;
const int motorEnablePin = 5;

// Values for rotation tuning - Implemenation1.
const unsigned int sensorTollerance = 400;
const unsigned int idleTollerance = 10;

// Values for rotation tuning - Implemenation2
const unsigned int MOTOR_OFF_THRESHHOLD  = 15;
const unsigned int MOTOR_ON_THRESHHOLD   = 600;
const unsigned int MOTOR_OVER_THRESHHOLD = 950;

enum motorState {
  MOTOR_UNKNOWN,
  MOTOR_OFF,
  MOTOR_ON,
  MOTOR_OVERCURRENT
};

// etherenet setup
//byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
//EthernetClient client;

boolean stringComplete = false;

void setup() {
  //set pins Motor driver Pins to output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // set pins keyboard driver pins to output/input
  pinMode(kLatchPin, OUTPUT);
  pinMode(kClockPin, OUTPUT);
  pinMode(kDataPin, INPUT);
  
  //Serial for debuging
  //Serial.begin(9600);
  Serial.begin(57600);
  pinMode(motorEnablePin, OUTPUT);
  // turn off motor.
  digitalWrite(motorEnablePin, HIGH);
 
  // initialise lcd panel.
  //lcd.begin (16,2); // for 16 x 2 LCD module
  //lcd.setBacklightPin(3,POSITIVE);
  //lcd.setBacklight(HIGH);
  
  // nfc card stuff
  // configure board to read RFID tags
  //nfc.SAMConfig();
  
  // get an IP address
  // Ethernet.begin(mac);
  
  // boot process!
  while(millis() < 4000) {
  //  lcd.setCursor(0,0);
  //    lcd.print("Booting         ");
  }
  //lcd.clear();
  
  // debug
  Serial.println("Enter ; to trigger one rotation.");
}

void loop() {
  //count up routine
  
  // temporary read in using serial.
  
  if(Serial.available()){
     if ((char)Serial.read() == ';')
       stringComplete = true;
   }
   
   // display IP address
   /*
   lcd.setCursor(0,0);
   for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    lcd.print(Ethernet.localIP()[thisByte], DEC);
    lcd.print("."); 
  }
  */

  
  
  if (stringComplete) {
    // set the motor
    set_motor();
    // perform a single rotation.
    
    if (detect_rotation()) {
      //lcd.setCursor(0,1);
      //lcd.print("Enjoy product!  ");
    }
    else
    {
      //lcd.setCursor(0,1);
      //lcd.print("Motor Failure   ");
    }
    
    stringComplete = false;
  }
  
  
  
  
  //delay(10000);
}

void set_motor() {
  
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
  
}

// Detect Rotation
// Timer to stop if it's definately taking too long.
boolean detect_rotation() {
  unsigned int sensorValue = 0;
  unsigned int previousValue = 0;
  unsigned int diff;
  int count = 0;
  long productTimer = millis(); // timer to set maximum run time.
  
  // engage the motors!
  digitalWrite(motorEnablePin, LOW);
  
  // Run this while loop forever!  we break and return true or false values when we get them.
  while(true) {
  
      // read sensor.
      sensorValue = analogRead(sensorPin);
      
      if (sensorValue < previousValue) {
        diff = previousValue - sensorValue;
        
        if (diff > sensorTollerance)
        {
          if (sensorValue < idleTollerance)
          {
            count++;
            if (count > 1) {
              
              Serial.print("Rotation ");
              
              digitalWrite(motorEnablePin, HIGH);
              
              return true; // return success!
            }
            
            //Settle time
            delay(50);
          }
        
          previousValue = sensorValue; 
        }
      }
      else if (sensorValue > previousValue) {
        diff = sensorValue - previousValue;
        
        if (diff > sensorTollerance) {
          previousValue = sensorValue;
        }
      }
      
      // check timer.
      if (productTimer < (millis() - 4000)) {
        digitalWrite(motorEnablePin, HIGH);
        return false;
      }
      
  } // end while
  
}

// Detect Rotation2
// Timer to stop if it's definately taking too long.
boolean detect_rotation2() {
  unsigned int sensorValue = 0;
  motorState currentState = MOTOR_OFF;
  motorState newState = MOTOR_UNKNOWN;
  int count = 0;
  long productTimer = millis(); // timer to set maximum run time.
  
  // engage the motors!
  //digitalWrite(motorEnablePin, LOW);
  
  // Run this while loop forever!  we break and return true or false values when we get them.
  while(true) {
      // engage the motors!
      digitalWrite(motorEnablePin, LOW); 
    
      // read sensor.
      sensorValue = analogRead(sensorPin);

      if (sensorValue <= MOTOR_OFF_THRESHHOLD)
        newState = MOTOR_OFF;
      else if (sensorValue <= MOTOR_ON_THRESHHOLD)
        newState = MOTOR_UNKNOWN;
      else if (sensorValue <= MOTOR_OVER_THRESHHOLD)
        newState = MOTOR_ON;
      else
        newState = MOTOR_OVERCURRENT;
  
      if (newState != MOTOR_UNKNOWN && newState != currentState)
      {
        if (currentState == MOTOR_ON && newState == MOTOR_OFF)
        {
          count++;
          if (count >= 2)
          {
            //We have rotation
            
            Serial.print("Rotation ");
              
            digitalWrite(motorEnablePin, HIGH);
              
            return true; // return success!
          }
          
          //Settle time
          delay(50);
        }
        
        currentState = newState;
    
        if (currentState == MOTOR_OVERCURRENT)
        {
          //Over current, stop motor and pause briefly
          digitalWrite(motorEnablePin, HIGH);
          delay(50);
        }
      }
      
      // check timer.
      if (productTimer < (millis() - 4000)) {
        digitalWrite(motorEnablePin, HIGH);
        return false;
      }
      
  } // end while
}
 

