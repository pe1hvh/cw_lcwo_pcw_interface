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

      // The place a letter appears here matches the value we pars
      //                                     0    1    2    3    4    5    6    7    8    9
     static constexpr char mySet[116][3]=  {"#", "#", "T", "E", "M", "N", "A", "I", "O", "G", //  00 -  9
                                            "K", "D", "W", "R", "U", "S", "#", "#", "Q", "Z", //  10 - 19
                                            "Y", "C", "X", "B", "J", "P", "#", "L", "#", "F", //  20 - 29
                                            "V", "H", "0", "9", "#", "8", "#", "#", "#", "7", //  30 - 39
                                            "#", "#", "#", "#", "#", "#", "#", "6", "1", "#", //  40 - 49
                                            "#", "#", "#", "#", "#", "#", "2", "#", "#", "#", //  50 - 59
                                            "3", "#", "4", "5", "#", "#", "#", "#", "#", "#", //  60 - 69
                                            "#", ":", "#", "#", "#", "#", ",", "#", "#", "#", //  70 - 79
                                            "#", "#", "#", "#", "!", "#", "#", "#", "#", "#", //  80 - 89
                                            "#", "#", "#", "#", "-", "#", "#", "'", "#", "#", //  90 - 99
                                            "#", "@", "#", "#", "#", "#", ".", "#", "#", "#", // 100 -109
                                            "#", "#", "#", "#", "#", "?" };                   // 110 -115

                                       
                                       

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


/*****************************************************/
/* @brief The Keyboard Handler                       */
/*****************************************************/
class KeyboardHandler : public BaseHandler {
  
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
         Keyboard.begin();
         writeInitMessage();     
    
    }
    
    void handleMorseKey(int pinState6 ,int pinState7) override {

           if(pinState6 == LOW || pinState7 == LOW )  {  //pin==LOW  => is closed (activated )
               keyIsDown();
            } else {                                     //pin==HIGH => is open ( deactived )
               keyIsUp();
            }  
        
    }

  private:  

    
    void writeInitMessage() {
           Keyboard.print("Morse CW text writer");   
           delay(2000);
           Keyboard.releaseAll();
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
       
        // Punctuation marks will make a BIG myNum
        if (myNum > 115)  {  
          printPunctuation();   // The value we parsed is bigger than our character array
                                // It is probably a punctuation mark so go figure it out.
          return;               // Go back to the main loop(), we're done here.
        }

        sendToScreen(*mySet[myNum]);
 
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
       
        if (letterCount < 20)  { 
          sendToScreen(' ');                // go figure out where to put it on the display
        }
      }
      
    /*****************************************************/
    /* @brief Print the Puctuation                       */
    /*****************************************************/ 
      void printPunctuation()   {
        // Punctuation marks are made up of more dits and dahs than
        // letters and numbers. Rather than extend the character array
        // out to reach these higher numbers we will simply check for
        // them here. This function only gets called when myNum is greater than 116
        char printChar = '#';    // Should not get here     
       
        if(myNum == 246) {
           printChar = '$';
        }
        
        sendToScreen(printChar);          // Go figure out where to put it on the display
      }


    /*****************************************************/
    /* @brief Send the char to the screen                */
    /*****************************************************/       
      void sendToScreen(char printChar) {
 
        if (letterCount == lineEnd)  {
          Keyboard.println();  
        } else {
          Keyboard.print(printChar); // Print our character at the current cursor location
        }
        Keyboard.releaseAll();
      }
    /*****************************************************/
    /* @brief reset of all important variabelen          */
    /*****************************************************/   
      void resetDefaults()    {
        downTime       = 0;         // Clear Input timers
        upTime         = 0;         // Clear Input timers
        startDownTime  = 0;         // Clear Input timers
        startUpTime    = 0;         // Clear Input timers
        dit            = 10;        // We start by defining a dit as 10 milliseconds
        averageDah     = 100;       // A dah should be 3 times as long as a dit
        fullWait       = 6000;      // The time between letters
        waitWait       = 6000;      // The time between dits and dahs
   
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
   - more than 2 seconds => set morse key as keyboard 
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
  } else if(keyPressDuration > 4000) {
     myObjectHandler = new KeyboardHandler();
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
