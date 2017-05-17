/*------------------------------------------- Command Library --------------------------------------------*/
/* This Library support almost all need function to work with Galileo Linux command 
 * 
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 13 April 2017 - version 1
 * by Nguyen Thanh Long

 * File require: MyLinuxCommand.h, MyLinuxCommand.cpp
 * Assembling: Arduino 1.6.9
 * Reference link:
   --> https://www.arduino.cc/en/Reference/StringObject

 * Acronyms table:
   --> nothing
 *
 * This code is released in public domains
 */
#ifndef MY_LINUX_COMMAND_H_
#define MY_LINUX_COMMAND_H_

//////////////////////////////////////////
///////SOME OLD IDE ERROR CONFIG//////////
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"  // Beta version
#else
  #include "WProgram.h" // Non - Beta version
#endif
//////////////////////////////////////////

class LinuxCommand {
  public:
    static bool CheckCommand(String str);
  private:
    String _string;
};

#endif // MY_LINUX_COMMAND_H_
