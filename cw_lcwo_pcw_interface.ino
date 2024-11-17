/******************************************************/
/* 
  LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
  by PE1HVH 
  Date; 14-11-2024
  Used hardware as describe at  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
  Choise between USB Mouse or USB Keyboard emulation.

  After adding the interface to a computer USB port, the program wait for a initial key press
  The first pressed by the morse key (Using a manipulator,  both paddles can be used).
   - less than a 1/2 second => mouse click
   - more than a 1/2 and less than 1 and 1/2 second => keyboardspace bar  
   - more than 2 seconds => textwriting with your morse key in your favourite textwriter.

   For the last option the function are baded on the source of Budd Churchward ( WB7FHC) Morse Code Decoder v. 0.1 and is used with permission of Barnacle Budd.
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
byte    outputType       = 0;         // output means interface behavior 1=mouseclick 2=keyboardspacebar 3=keyboard texting
int     bounce           = 2;         // software bounce handles the normal key bounce

int     val              = 0;         // A value for key up and down

boolean ditOrDah         = true;      // We have a full dit or a full dah
int     dit              = 10;        // If we loop less than this with keydown it's a dit else a dah
int     averageDah       = 100;       // Start with this value we will adjusted it each time he sends a dah


boolean characterDone    = true;      // A full character has been sent
int     downTime         = 0;         // We are going to count the cycles we loop while key is down

long    FullWait         = 6000;      // This value will be set by the sender's speed - the gap between letters
long    WaitWait         = 6000;      // WaitWait is for the gap between dits and dahs
long    newWord          = 0;         // For the gap between words

int nearLineEnd          = 60;        // How far do you want to type across your monitor window?
int letterCount          = 0;         // To keep track of how many characters have been printed on the line

int  myNum               = 0;         // We will turn the dits and dahs into a data stream and parse a value that we will store here

// The Morse Code is embedded into the binary version of the numbers from 2 - 63
// The place a letter appears here matches myNum that we parsed out of the code
// #'s are miscopied characters
char mySet[]             = "##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#######61#######2###3#45";

/****************************************************/
/* @brief Initalise keyboard library                */
/****************************************************/
void setKeyboard(byte type){
  outputType=type;
  Keyboard.begin();
}

/****************************************************/
/* @brief Use mouse    library                      */
/****************************************************/
void setMouseClick(){
  outputType=1;
  Mouse.begin();
}

/****************************************************/
/* @brief Write Keyboard or Mouse                   */
/****************************************************/
void startOutput(){
  if(outputType==1) {
     Mouse.press(MOUSE_LEFT); // left mouse click
  } else if(outputType==2){
     Keyboard.press(0x20);    // space bar
  } else {
     keyIsDown();  
  }
  
}

/****************************************************/
/* @brief Stop Keyboard or Mouse                    */
/****************************************************/
void stopOutput(){
  if(outputType==1) {
     Mouse.release();
  } else if(outputType==2) {   
     Keyboard.releaseAll();
  } else {
     keyIsUp();    
  }     
}

/***********************************************/
/* @brief Count how long the key was pressed   */
/***********************************************/ 
 void keyIsDown() {
   WaitWait=FullWait;     // Reset our Key Up countdown
   downTime++;   //Count how long the key is down

  if (myNum==0) {       // myNum will equal zero at the beginning of a character
      myNum=1;          // This is our start bit  - it only does this once per letter
    }

  characterDone=false;  // we aren't finished with the character yet, there could be more
  ditOrDah=false;       // we don't know what it is yet - key is still down
  delay(bounce);      // short delay to keep the real world in synch with Arduino
 }

/***********************************************/
/* @brief Determinated if a full char was sent */
/***********************************************/ 
 void keyIsUp() {
    // If we haven't already started our timer, do it now
   if (startUpTime == 0){
    startUpTime = millis();
   }
   
   // Find out how long we've gone with no tone
   // If it is twice as long as a dah print a space
   
   upTime = millis() - startUpTime;
   
   if (upTime<20) return;
  
   // Farnsworth setting.
   if (upTime > (averageDah*Farns)) 
   {    
      printSpace();
   }
   
   // Only do this once after the key goes up
   if (startDownTime > 0)
   {
     downTime = millis() - startDownTime;  // how long was the tone on?
     startDownTime=0;      // clear the 'Key Down' timer
   }
 
   if (!ditOrDah) 
   {   
     // We don't know if it was a dit or a dah yet
      shiftBits();    // Let's go find out! And do our Magic with the bits
   }

    // If we are still building a character ...
    if (!characterDone) 
    {
       // Are we done yet?
       if (upTime > dit) 
       { 
         // BINGO! we're done with this one  
         printCharacter();       // Go figure out what character it was and print it       
         characterDone=true;     // We got him, we're done here
         myNum=0;                // This sets us up for getting the next start bit
       }
         downTime=0;             // Reset our keyDown counter
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
      delay(bounce);                // simple method against bouncing
    }
    // Key pressed, record the start time
    keyPressStartTime = millis();

    // Wait for the Key to be released
    while (digitalRead(inPin6) == LOW || digitalRead(inPin7) == LOW) {
      // Key still pressed, keep waiting
      delay(bounce);                // simple method against bouncing
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
  // Determine which usb output (mouse or keyboard)
  keyPressDuration = getKeyPressDuration();  
  if(keyPressDuration > 0 && keyPressDuration < 500) {
     setMouseClick();
  } else if (keyPressDuration > 500 && keyPressDuration < 1500){
     setKeyboard((byte)2);
  } else {
     setKeyboard((byte)3);
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
