Morse Code Interface for LCWO and PCWFistCheck
==============================================

Introduction
---------------

More than a year ago I have build the Morse Code USB/HID Interface (The Gadget). But after some testing never used.

This software give the already build interface an other goal. Which I think I will used more.
 It's now a simple interface for using the transmitting practice from LCWO end as interface for PCWFistCheck
  
Related Websites
---------------

- [For STL files and SeeeduinoXIAO pinout description][https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadet]
- [Learn CW Online][https://www.lcwo.net]
- [PCWFistCheck][https://www.qsl.net/dj7hs/download.htm]

Description
---------------

In the intial setup the program wait for an input sign. Depending on an the input of your morse key it will used a keyboard or a mouse.
  
Press the mors key;
- for less than a 1/2 seconds left mouse is used (PCWFistCheck)
- for 1/2 second or more , the space bare is used (LCWO)
By using a manipulator both paddles can be used for making the choice.
  
PlatformIO
---------------

Almost all my software is build with Visual Code. For the Arduino PlatformIO plugin is installed,
  
PlatformIO requires that functions be declared before they are called in the code. This is because PlatformIO uses a more standard C++ compilation process,
which adheres strictly to the requirement that functions must be declared before use.

SonarLint
---------------

For writing cleancode, the Sonarlint plug-in is installed in Visual Code. 
SonarLint is an open-source code analysis tool that serves several important functions to help developers write cleaner, safer, and higher-quality code. 
There is no need, for using all sonarlint rules but the most important are enabled.
  
Tested
---------------

Build and Upload is tested also with Arduino IDE 2.3 and Arduino 1.8.19. 

| **Key**          | **PCWFictCheck** | **LCWO** | **ResultOK** |
|------------------|:----------------:|:--------:|:------------:|
| **Straight Key** |  &check;         | &check;  | &check;      |
| **Swiper**       |  &check;         | &check;  | &check;      |
| **Paddle**       |  &check;         | &check;  | &check;      |
 
##Compatible
To avoid compatibility issues, the source is saved as an arduino.ino file.
