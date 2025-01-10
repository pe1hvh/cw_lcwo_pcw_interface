/******************************************************/
/* 
  LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
  by PE1HVH 
  version 2
  Date; 10-01-2025
  
  Used hardware as describe at  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
  Choise between USB Mouse or USB Keyboard emulation.
  
  Add Seeeduino to your Arduino IDE
        Click on File > Preference, and fill Additional Boards Manager URLs with the url below:
        https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
  
  After adding the interface to a computer USB port, the program wait for a initial key press
  The first pressed by the morse key (Using a manipulator,  both paddles can be used).
   - less than a 1/2 second => mouse lef tbutton (for TX on PCWFistcheck)
   - more than a 1/2 second => keyboard space bar (for TX on www.lcwo.net)
   - more than 2 seconds    => keyboard (the morse key as keyboard)  

  The original code of the keyboard handler is based on a sketch of  
   Barnacle Budd's Morse Code Decoder v. 0.1
   (c) 2011, Budd Churchward - WB7FHC

   Notes and Warnings

   When you use the Keyboard.print() command, the Arduino takes over your keyboard! 
   Make sure you have control before you use the command. 

*/  
/*****************************************************/


/****************************************************/
/* Librarys                                         */
/****************************************************/
#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>

#define inPin6   6
#define inPin7   7

#define KEY_SPACE_BAR 0x20

/*****************************************************/
/* @brief The BaseHandler                            */
/*****************************************************/

class BaseHandler {

  public:

    virtual void handleMorseKey(int pinState6 ,int pinState7) = 0;
    virtual void initHandler() = 0;

  private:

    virtual void keyIsUp()   = 0; 
    virtual void keyIsDown() = 0;
  
};


/*****************************************************/
/* @brief The Mouse Handler                          */
/*****************************************************/

class MouseHandler : public BaseHandler {

  public:
  
    void initHandler() override {
             Mouse.begin();
         }

    void handleMorseKey(int pinState6 ,int pinState7) override {
             if(pinState6 == LOW || pinState7 == LOW )  { //pin==LOW  => is closed (activated )
                 keyIsDown();
              } else {                                    //pin==HIGH => is open ( deactived )
                 keyIsUp();
              }  
           }


  private:
     
    void keyIsDown() override {
             Mouse.press(MOUSE_LEFT); // left mouse click;                
        }

    void keyIsUp() override {
             Mouse.release();
        }
};


/*****************************************************/
/* @brief The Space Bar Handler                      */
/*****************************************************/
class SpaceBarHandler : public BaseHandler {
  
  public:
  
    void initHandler() override {
            Keyboard.begin();
        }


    void handleMorseKey(int pinState6 ,int pinState7) override {
             if(pinState6 == LOW || pinState7 == LOW )  {  //pin==LOW  => is closed (activated )
                 keyIsDown();
              } else {                                    //pin==HIGH => is open ( deactived )
                 keyIsUp();
              }  
           }

    

  private:
      
    void keyIsDown() override {
            Keyboard.print(KEY_SPACE_BAR);      // print a space on the monitor window    
         }

    void keyIsUp() override {
            Keyboard.releaseAll();
        }
};



BaseHandler* myObjectHandler = nullptr;




/*********************************************/
/* @brief Calculation of key pressed   
   @return key press duration in milliseconds
*/
/*********************************************/ 
 unsigned long getKeyPressDuration() {
    unsigned long keyPressStartTime = 0;
   
    while (digitalRead(inPin6) == HIGH && digitalRead(inPin7)==HIGH) {
      //key is not Pressed
      delay(25);                // simple method against bouncing
    }
    // Key pressed, record the start time
    keyPressStartTime = millis();
    
    // Wait for the Key to be released
    while (digitalRead(inPin6) == LOW || digitalRead(inPin7) == LOW) {
      // Key still pressed, keep waiting
      delay(25);                // simple method against bouncing
    }
    
    // Key released, calculate the duration
    return (millis() - keyPressStartTime);  
 }    



/******************************************/
/* @brief SetUp:  
   - first time pressed morse key I9=2D
   - less than a 1/2 second => mouse (left button)
   - more than a 1/2 second => keyboard  (space bar)
*/
/******************************************/
void setup() {
  
  pinMode(inPin6, INPUT_PULLUP);
  pinMode(inPin7, INPUT_PULLUP);
  
  unsigned long keyPressDuration = 0;

  keyPressDuration = getKeyPressDuration();  
  // Determine which usb output (mouse or keyboard)
  
  if(keyPressDuration > 0 && keyPressDuration < 500) {
     myObjectHandler = new MouseHandler();
  } else {
     myObjectHandler = new SpaceBarHandler();
  }
  myObjectHandler->initHandler();
}    



/****************************************************/
/* @brief loop 
   1. Determine the status of pin6 and pin 7
   2. Use Keyboard spacebar or left mouseclick  
*/
/****************************************************/
void loop(){
 
     myObjectHandler->handleMorseKey(digitalRead(inPin6),digitalRead(inPin7));
 
}
