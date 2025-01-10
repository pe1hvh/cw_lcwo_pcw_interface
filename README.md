Morse Code Interface for LCWO and PCWFistCheck
==============================================

Introduction
---------------

More than a year ago I have build the Morse Code USB/HID Interface (The Gadget). But after some testing never used.

This software give the already build interface an other goal. Which I think I will used more.
 It's now a simple interface for using the transmitting practice from LCWO end as interface for PCWFistCheck
  
Related Websites
---------------

- [For STL files and SeeeduinoXIAO pinout description](https://hackaday.io/project/184702-morse-code-usbhid-interface-the-gadet)
- [Learn CW Online](https://www.lcwo.net)
- [PCWFistCheck](https://www.qsl.net/dj7hs/download.htm)

Description
---------------

In the intial setup the program wait for an input sign. Depending on an the input of your morse key it will used a keyboard or a mouse.
  
Press the mors key;
- for less than a 1/2 seconds left mouse is used (PCWFistCheck)
- for 1/2 second or more , the space bare is used (LCWO)
By using a manipulator both paddles can be used for making the choice.

Use Smart Phone to Key CW Morse
-------------------------------
Connect the Seeeduino to your phone and you can also use the tx function of www.lcwo.net on your phone with your prefered morse key.  First navigate to this function and then connect the seeduino.
![image](https://github.com/user-attachments/assets/2a77f970-697b-4497-9fc2-f78e8dddcc48)

  
PlatformIO
---------------

Almost all my software is build with Visual Code. For the Arduino PlatformIO plugin is installed,
  
PlatformIO requires that functions be declared before they are called in the code. This is because PlatformIO uses a more standard C++ compilation process,
which adheres strictly to the requirement that functions must be declared before use. For compatiblity reasons testing is also done from the Arduino IDE and 
the source is save from the Arduino IDE.

SonarLint
---------------

For writing cleancode, the Sonarlint plug-in is installed in Visual Code. 
SonarLint is an open-source code analysis tool that serves several important functions to help developers write cleaner, safer, and higher-quality code. 
There is no need, for using all sonarlint rules but the most important are enabled. 
  
Tested
---------------

Build and Upload is tested also with Arduino IDE 2.3 and Arduino 1.8.19.

For software test see table;

| **Key**          | **PCWFictCheck** | **LCWO** | **ResultOK** |
|------------------|:----------------:|:--------:|:------------:|
| **Straight Key** |  &check;         | &check;  | &check;      |
| **Swiper**       |  &check;         | &check;  | &check;      |
| **Paddle**       |  &check;         | &check;  | &check;      |
 
Notes and Warnings
---------------
When you use the Keyboard.print() or Keyboard.press() command, the Arduino board takes over your keyboard! Make sure you have control before you use the command. A pushbutton to toggle the keyboard control state is effective.

73 [PE1HVH ](https://www.pe1hvh.nl)

