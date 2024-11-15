/******************************************************/
/* 
  LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
  by PE1HVH 
  Date; 14-11-2024
  Used hardware as describe at  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
  Choise between USB Mouse or USB Keyboard emulation.

  After adding the interface to a computer USB port, the program wait for a initial key press
  The first pressed by the morse key (Using a manipulator,  both paddles can be used).
   - less than a 1/2 second => mouse
   - more than a 1/2 second => keyboard  
*/  
/*****************************************************/

/****************************************************/
/* Librarys                                         */
/****************************************************/
#include "Arduino.h"
#include "Keyboard.h"
#include "Mouse.h"

#define inPin6   6
#define inPin7   7

/****************************************************/
/* Globals                                          */
/****************************************************/
byte outputType       = 0;    // output means interface behavior 1=mouse 2=keyboard

/****************************************************/
/* @brief Initalise keyboard library                */
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

/*********************************************/
/* @brief Calculation of key pressed   
   @return key press duration in milliseconds
*/
/*********************************************/ 
 unsigned long getKeyPressDuration() {
    unsigned long keyPressStartTime = 0;
   
    while (digitalRead(inPin6) == HIGH && digitalRead(inPin7)==HIGH) {
      //key is not Pressed
      delay(20);                // simple method against bouncing
    }
    // Key pressed, record the start time
    keyPressStartTime = millis();
    
    // Wait for the Key to be released
    while (digitalRead(inPin6) == LOW || digitalRead(inPin7) == LOW) {
      // Key still pressed, keep waiting
      delay(20);                // simple method against bouncing
    }
    
    // Key released, calculate the duration
    return (millis() - keyPressStartTime);  
 }    

/******************************************/
/* @brief SetUp:  
   - first time pressed morse key 
   - less than a 1/2 second => mouse
   - more than a 1/2 second => keyboard  
*/
/******************************************/
void setup() {
  pinMode(inPin6, INPUT_PULLUP);
  pinMode(inPin7, INPUT_PULLUP);
  
  unsigned long keyPressDuration = 0;

  keyPressDuration = getKeyPressDuration();  
  // Determine which usb output (mouse or keyboard)
  if(keyPressDuration > 0 && keyPressDuration < 500) {
    setMouse();
  } else {
    setKeyboard();
  }
}   

/****************************************************/
/* @brief loop 
   1. Determine the status of pin6 and pin 7
   2. Use Keyboard spacebar or left mouseclick      
*/
/****************************************************/
void loop(){
   int pinState6 = digitalRead(inPin6);
   int pinState7 = digitalRead(inPin7);
 
   if(pinState6 == LOW || pinState7 == LOW )  {  //pin==LOW  => is closed (activated )
      startOutput();
   } else {                                      //pin==HIGH => is open ( deactived )
      stopOutput(); 
   }
}
