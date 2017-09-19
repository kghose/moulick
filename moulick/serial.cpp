#include "serial.h"

namespace serialport
{
  void init()
  {
    Serial.begin(9600);
    while( !Serial ) { ; }  // Wait for serial port to connect
    Serial.println("Moulick - a prime number toy");    
  }

  bool SerialPort::poll()
  {
    if( Serial.available() )
    {
      interpret_serial_input( Serial.readStringUntil( '\n' ) );
      return true;  
    } 
    else
    {
      cmd_type = SerialCommandType::Nothing;  
      return false;
    }
  }

  void SerialPort::interpret_serial_input( const String ser_str )
  {
    //Try to interpret this as a numeric value
    //If we encounter a non-digit anywhere, interpret this
    //as a switch command
    cmd_type = SerialCommandType::Set;
    new_m = 0;
    for(int i = ser_str.length() - 1; i >= 0; i--)
    {
      if( !isDigit( ser_str[ i ] ) ) 
      {
        cmd_type = SerialCommandType::Switch;
        break;    
      }
      new_m = new_m * 10 + ( ser_str[ i ] - '0' );
    }
  }
}