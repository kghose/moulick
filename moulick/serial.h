/*
  Handles commands sent over the serial port.

  Sending a series of digits terminated by a newline over the
  serial port is interpreted as a request to restart the
  prime clock from the given number.   

  Sending a newline by itself, or any string containing non
  digit characters is interpreted as a command to switch 
  screens.
*/
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "primes.h"

namespace serialport 
{
  using namespace primes;

  enum class SerialCommandType{Nothing=0, Set, Switch};
  void init();

  struct SerialPort
  {
    
    SerialCommandType cmd_type;
    prime_t new_m;

    // read serial line and return true if a command was recieved
    // set appropriate state 
    bool poll();  

    // Given a string read from serial input, figure out
    // what class of command it is and set state accordingly
    void interpret_serial_input( const String ser_str );
  };
}

#endif // _SERIAL_H_