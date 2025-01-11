/******************************************************/
/* 
       Title:   LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
      Author:   JA van Hernen, www.pe1hvh.nl
        Date:   10 Jan 2025
     Version:   2.0   
    Hardware:   Seeeduino XIAO samd
         IDE:   Arduino IDE 1.8.19
       Legal:   Copyright (c) 2025  JA van Hernen.
                Open Source under the terms of the MIT License. 

  Description

  Simple program to generate a mouse click or keystroke with your favorite morse key and 
  learning software like Learn CW Online or PCWFistCheck etc.
  
  After adding the interface to a computer USB port, the program wait for a initial key press
  The first pressed by the morse key (Using a manipulator,  both paddles can be used).
   - less than a 1/2 second => mouse lef tbutton (for TX on PCWFistcheck or www.lcwo.net)
   - more than a 1/2 second => keyboard space bar (for TX on www.lcwo.net)

  Used hardware as describe at  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
  
  Add Seeeduino to your Arduino IDE
        Click on File > Preference, and fill Additional Boards Manager URLs with the url below:
        https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
    

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

#define inPin6   6   // Single Key
#define inPin7   7   // Paddle

#define KEY_SPACE_BAR 0x20

/*****************************************************/
/* @brief The BaseHandler                            */
/*****************************************************/

class BaseHandler {

  public:

    virtual void handleMorseKey(int pinDot ,int pinDash) = 0;
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

    void handleMorseKey(int pinDot ,int pinDash) override {
             if(pinDot == LOW || pinDash == LOW )  { //pin==LOW  => is closed (activated )
                 keyIsDown();
              } else {                               //pin==HIGH => is open ( deactived )
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


    void handleMorseKey(int pinDot ,int pinDash) override {
             if(pinDot == LOW || pinDash == LOW )  { //pin==LOW  => is closed (activated )
                 keyIsDown();
              } else {                               //pin==HIGH => is open ( deactived )
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


/*********************************************/
/* @brief Calculation of key pressed         */
/*********************************************/ 
class Timer {
  
  private:
    
    unsigned long keyPressStartTime = 0;
    unsigned long keyPressDuration  = 0;

  public:
  
    /******************************************/
    /* @brief The constructor                 */   
    /******************************************/
    Timer() {
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
         keyPressDuration = (millis() - keyPressStartTime);  
    }

    /******************************************/
    /* @brief The getter for keyPressDuration */   
    /******************************************/
    unsigned long getKeyPressDuration(){
         return keyPressDuration;
    }
          
};


BaseHandler* myObjectHandler = nullptr;  // Declare the pointer to the BaseHandler class and initialize it to nullptr
                                         // myObjectHandler is the pointer that hold the address of a BaseHandler object.
                                         // it is initiazed to nullptr, meaning it currently do not point (now) to any object





/******************************************/
/* @brief SetUp:  
   - first time pressed morse key 
   - less than a 1/2 second => mouse (left button)
   - more than a 1/2 second => keyboard  (space bar)
*/
/******************************************/
void setup() {
  
  pinMode(inPin6, INPUT_PULLUP); // straight key
  pinMode(inPin7, INPUT_PULLUP); // swiper or paddle
  
  Timer  myTimer;                // Declare the object myTimer of type Timer
  
  if(myTimer.getKeyPressDuration() > 0 && myTimer.getKeyPressDuration()  < 500) {
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
