Morse Code Interface for LCWO and PCWFistCheck
==============================================

  Introduction
  -----------
  More than a year ago I have build the Morse Code USB/HID Interface (The Gadget). But after some testing never used.

  This software give the already build interface an other goal. Which I think I will used more.
  It's now a simple interface for using the transmitting practice from LCWO end as interface for PCWFistCheck
  
  Related Websites
  ----------------
  https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadet (For STL files and SeeeduinoXIAO pinout description)
  https://www.lcwo.net (Learn CW Online)
  https://www.qsl.net/dj7hs/download.htm (PCWFistCheck download)

  Description
  -----------
  In the intial setup the program wait for an input sign. Depending on an the input of your morse key it will used 
  a. Make a signal for 1/2 second or mote , the space bare is used (LCWO)
  b. Make a signal for less than a 1/2 seconds left mouse is used (PCWFistCheck)
  By using a manipulator both paddles can be used for making the choice.
  
  PlatformIO
  ----------
  Almost all my software is build with Visual Code. For the Arduino PlatformIO plugin is installed,
  
  PlatformIO requires that functions be declared before they are called in the code. This is because PlatformIO uses a more standard C++ compilation process,
  which adheres strictly to the requirement that functions must be declared before use.

  SonarLint
  ---------
  For writing cleancode, the Sonarlint plug-in is installed in Visual Code. 
  SonarLint is an open-source code analysis tool that serves several important functions to help developers write cleaner, safer, and higher-quality code. 
  There is no need, for using all sonarlint rules but the most important are enabled.
  
  Tested
  ------
  Build and Upload is tested also with Arduino IDE 2.3 and Arduino 1.8.19. 
  
  Compatible
  ----------
  To avoid compatibility issues, the source is saved as an arduino.ino file
  

  ` 
