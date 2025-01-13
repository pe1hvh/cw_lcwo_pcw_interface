/******************************************************/
/* 
       Title:   LCWO PCWFistCheck Interface with a SeeeduinoXIAO.
      Author:   JA van Hernen, www.pe1hvh.nl
        Date:   10 Jan 2025
    Hardware:   Seeeduino XIAO samd
     Version:   3.0   
         IDE:   Arduino IDE 1.8.19
       Legal:   Copyright (c) 2025  JA van Hernen.
                Open Source under the terms of the MIT License. 
    
 Description:   
  
  For a STL file see and pin connection https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadget  
  
  Add Seeeduino to your Arduino IDE
        Click on File > Preference, and fill Additional Boards Manager URLs with the url below:
        https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
  
  After adding the interface to a computer USB port, the program wait for a initial key press
  The first pressed by the morse key (Using a manipulator,  both paddles can be used).
   - less than a 1/2 second => mouse lef tbutton (for TX on PCWFistcheck)
   - more than a 1/2 second => keyboard space bar (for TX on www.lcwo.net)
   - more than 2 seconds    => Morse to the Web page  

 Notes and Warnings:

   When you use the Keyboard.print() command, the Arduino takes over your keyboard! 
   Make sure you have control before you use the command. 

   
           >>>>>> THIS VERSION UNDER DEVELOPMENT AND NEED SOME MAINTAINCE <<<<<<

*/  
/*****************************************************/


/****************************************************/
/* Librarys                                         */
/****************************************************/
#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <SimpleWebSerial.h>
#include <FlashStorage.h>

#define inPin6   6
#define inPin7   7

#define KEY_SPACE_BAR 0x20

//                       10        20        30        40        50        60        70        80        90        100       110
//             0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 12345
char mySet[] =" #TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#######61#######2###3#45#######:####,#######!#########-##'###@####.########?";
                                       
SimpleWebSerial WebSerial;                                       

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


/*****************************************************/
/* @brief MorseDecoder                               */
/*****************************************************/
class MorseDecoder : public BaseHandler {
  
  private:
  
      long pitchTimer     = 0;    // Keep track of how long since right button was pressed 
                                  // so we can reverse direction of pitch change
      long downTime       = 0;    // How long the tone was on in milliseconds
      long upTime         = 0;    // How long the tone was off in milliseconds
      
      long startDownTime  = 0;    // Arduino's internal timer when tone first comes on
      long startUpTime    = 0;    // Arduino's internal timer when tone first goes off
      long lastChange     = 0;    // Keep track of when we make changes 
      
      long lastDahTime    = 0;    // Length of last dah in milliseconds
      long lastDitTime    = 0;    // Length of last dit in milliseconds
      
   
      boolean ditOrDah      = true;        // We have a full dit or a full dah
      int     dit           = 10;          // If we loop less than this with keydown it's a dit else a dah
      int     averageDah    = 150;         // Start with this value we will adjusted it each time he sends a dah
  
  
      boolean characterDone = true;        // A full character has been sent
      boolean justDid       = true;        // Makes sure we only print one space during long gaps
      
      int     myBounce      = 2;           // Handles normal keybounce but we needed to do more later
  
      long    fullWait      = 6000;         // This value will be set by the sender's speed - the gap between letters
      long    waitWait      = fullWait;    // WaitWait is for the gap between dits and dahs
      long    newWord       = 0;           // For the gap between words
  
      int     letterCount   = 0;           // To keep track of how many characters have been printed on the line
      int     lastWordCount = 0;           // Keeps track of how may characters are in the current word 
      int     lastSpace     = 0;           // Keeps track of the location of the last 'space' 
      int     myNum         = 0;           // We will turn the dits and dahs into a data stream and parse
      int     lineEnd       = 60;          // One more than number of characters across display    
                                           // a value that we will store here
  
      int Farns             = 1;           // Used to calculate the Farnsworth space between words
      int FarnsTime         = 0;           // Toggle value for Farnsworth setting
      int oldFarns          = 1;           // Holds previous value of Farns
      
  public:

     void initHandler() override {
         Serial.begin(115200);
         while(!Serial);
         delay(200); 
         writeInitMessage();     
    
     }
    
    void handleMorseKey(int pinDot ,int pinDash) override {
             if(pinDot == LOW || pinDash == LOW )  { //pin==LOW  => is closed (activated )
                 keyIsDown();
              } else {                               //pin==HIGH => is open ( deactived )
                 keyIsUp();
              }  
           }
           
  private:  

    void writeInitMessage() {
         delay(50);
         WebSerial.send("cw_log", "Morse CW text writer\n");  
         delay(2000);
    }


    /*****************************************************/
    /* @brief Key is Down                                */
    /*****************************************************/
     void keyIsDown() override {
       
       if (startUpTime>0)   {  
         // We only need to do this once, when the key first goes down
         startUpTime=0;    // clear the 'Key Up' timer
       }
       // If we haven't already started our timer, do it now
       if (startDownTime == 0)  {
           startDownTime = millis();  // Get the Arduino's current clock time
       }
    
         characterDone=false;  // We're still building a character
         ditOrDah=false;       // The key is still down we're not done with the tone
         delay(myBounce);      // Take a short breath here
         
       if (myNum == 0)   {     // myNum will equal zero at the beginning of a character
          myNum = 1;           // This is our start bit  - it only does this once per letter
       }
     }

 
    /*****************************************************/
    /* @brief Key is Up                                  */
    /*****************************************************/
    void keyIsUp() override {
           
       
       // If we haven't already started our timer, do it now
       if (startUpTime == 0) {
        startUpTime = millis();
       }
       
       // Find out how long we've gone with no tone
       // If it is twice as long as a dah print a space
       
       upTime = millis() - startUpTime;
       
      if (upTime<20) return;

    
       // Farnsworth setting.
       if (upTime > (averageDah*Farns))  {    
          printSpace();
       }
       
       // Only do this once after the key goes up
       if (startDownTime > 0)  {
         downTime = millis() - startDownTime;  // how long was the tone on?
         startDownTime=0;      // clear the 'Key Down' timer
       }
     
     
       if (!ditOrDah)  {   
         // We don't know if it was a dit or a dah yet
          shiftBits();    // Let's go find out! And do our Magic with the bits
       }
    
        // If we are still building a character ...
        if (!characterDone)  {
           // Are we done yet?
           if (upTime > dit) { 
             // BINGO! we're done with this one  
             printCharacter();       // Go figure out what character it was and print it       
             characterDone=true;     // We got him, we're done here
             myNum=0;                // This sets us up for getting the next start bit
           }
             downTime=0;             // Reset our keyDown counter
        }
    }
   

   
    /*****************************************************/
    /* @brief Shift the Bits                             */
    /*****************************************************/
    void shiftBits() {
          // we know we've got a dit or a dah, let's find out which
          // then we will shift the bits in myNum and then add 1 or not add 1
          
          if (downTime < dit/3) return;  // Ignore my keybounce
          myNum = myNum << 1;            // shift bits left
          ditOrDah = true;               // We will know which one in two lines 
          
          
          // If it is a dit we add 1. If it is a dah we do nothing!
          if (downTime < dit) {
             myNum++;                   // add one because it is a dit
          } else {
            // The next three lines handle the automatic speed adjustment:
            averageDah = (downTime+averageDah) / 2;  // running average of dahs
            dit = averageDah / 3;                    // normal dit would be this
            dit = dit * 2;          // Double it to get the threshold between dits and dahs
          }
    }

    
    /*****************************************************/
    /* @brief Print the Char                             */
    /*****************************************************/
    void printCharacter()  {           
        justDid = false;       // OK to print a space again after this
       
       
        sendToScreen(myNum);
 
    }

      /*****************************************************/
    /* @brief Print the Space                            */
    /*****************************************************/   
    void printSpace() {
        if (justDid) return;    // only one space, no matter how long the gap
        justDid = true;         // so we don't do this twice
        Farns = 2 + FarnsTime;  // Confirm latest Farns value

        lastWordCount=0;               // start counting length of word again
        lastSpace=letterCount;         // keep track of this, our last, space
       
        // We don't need to print the space if we are at the very end of the line
        sendToScreen(0);                // go figure out where to put it on the display
      
    }
      
      

    /*****************************************************/
    /* @brief Send the char to the screen                */
    /*****************************************************/       
      void sendToScreen(int mNum) {
        char printChar = '#';    // Should not get here     
       
        if(myNum == 246) {
             printChar = '$';
        } else {
            printChar = (char) mySet[mNum];
        }
      
       if (letterCount == lineEnd)  {
           WebSerial.send("cw_log", "\n" );
        } else {
           WebSerial.send("cw_log", printChar );
        }
      }


      
    /*****************************************************/
    /* @brief reset of all important variabelen          */
    /*****************************************************/   
      void resetDefaults()    {
        downTime       = 0;         // Clear Input timersMorse CW text writer
        upTime         = 0;         // Clear Input timers
        startDownTime  = 0;         // Clear Input timers
        startUpTime    = 0;         // Clear Input timers
        dit            = 10;        // We start by defining a dit as 10 milliseconds
        averageDah     = 100;       // A dah should be 3 times as long as a dit
        fullWait       = 6000;      // The time between letters
        waitWait       = 6000;      // The time between dits and dahs
   
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
   - first time pressed morse key I9=2D
   - less than a 1/2 second => mouse (left button)
   - more than a 1/2 second => keyboard  (space bar)
   - more than 2 seconds => set morse key as keyboard 
*/
/******************************************/
void setup() {
  
  pinMode(inPin6, INPUT_PULLUP); // straight key
  pinMode(inPin7, INPUT_PULLUP); // swiper or paddle
  
  Timer  myTimer;                // Declare the object myTimer of type Timer
  
  if(myTimer.getKeyPressDuration() > 0 && myTimer.getKeyPressDuration()  < 500) {
     myObjectHandler = new MouseHandler();
  } else if (myTimer.getKeyPressDuration()  > 5000){
     myObjectHandler = new MorseDecoder();
  } else {
     myObjectHandler = new SpaceBarHandler();
  }   
  myObjectHandler->initHandler();
}


/****************************************************/
/* @brief loop 
   1. Determine the status of pin6 and pin 7
   2. Use Keyboard spacebar or left mouseclick  
   3. or use morse key as text writer    
*/
/****************************************************/
void loop(){
 
     myObjectHandler->handleMorseKey(digitalRead(inPin6),digitalRead(inPin7));
 
}
