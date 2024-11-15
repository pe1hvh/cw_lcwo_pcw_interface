/******************************************************/
/* 
  LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
  by PE1HVH 
  Date; 14-11-2024
  Used hardware as describe at  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
*/  
/*****************************************************/

/****************************************************/
/* Librarys                                         */
/****************************************************/
#include "Arduino.h"
#include "Keyboard.h"
#include "Mouse.h"

#define inPin   6

/****************************************************/
/* Globals                                          */
/****************************************************/
byte outputType       = 0;    // output means interface behavior 1=mouse 2=keyboard

/****************************************************/
/* @brief Initalise keyboard library                             */
/****************************************************/
void setKeyboard(){
  outputType=2;
  Keyboard.begin();
}

/****************************************************/
/* @brief Use mouse    library                      */
/****************************************************/
void setMouse(){
  outputType=1;
  Mouse.begin();
}

/****************************************************/
/* @brief Write Keyboard or Mouse                   */
/****************************************************/
void startOutput(){
  if(outputType==2) {
     Keyboard.press(0x20);    // space bar
  } else {
     Mouse.press(MOUSE_LEFT); // left mouse click
  }
}

/****************************************************/
/* @brief Stop Keyboard or Mouse                    */
/****************************************************/
void stopOutput(){
  if(outputType==2) {
     Keyboard.releaseAll();
  } else {
     Mouse.release();
  }  
}

/******************************************/
/* @brief SetUp: read time pressed morse key 
   - less than a second => mouse
   - more than a second => keyboard  
*/
/******************************************/
void setup() {
  pinMode(inPin, INPUT_PULLUP);
  unsigned long keyPressStartTime = 0;
  unsigned long keyPressDuration = 0;

  while (digitalRead(inPin) == HIGH) {
    //key is not Pressed
  }
  // Key pressed, record the start time
  keyPressStartTime = millis();
  
  // Wait for the Key to be released
  while (digitalRead(inPin) == LOW) {
    // Key still pressed, keep waiting
  }
  
  // Key released, calculate the duration
  keyPressDuration = millis() - keyPressStartTime;  

  // Determine which usb output (mouse or keyboard)
  if(keyPressDuration > 50 && keyPressDuration < 900) {
    setMouse();
  } else {
    setKeyboard();
  }
}        

/****************************************************/
/* @brief loop 
   1. Determine the status op pin6 
   2. Use Keyboard spacebar or left mouseclick      */
/****************************************************/
void loop(){
   int pinState = digitalRead(inPin);
 
   if(pinState == LOW) {  //pin==LOW  => is closed (activated )
      startOutput();
   } else {               //pin==HIGH => is open ( deactived )
      stopOutput(); 
   }
}
