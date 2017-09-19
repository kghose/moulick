/*
  Code to handle touch screen interaction
*/

#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include "tftconstants.h"
#include "primes.h"

namespace touchscreen 
{
  using namespace primes;

  enum class TouchCommandType { 
    Nothing=0,  // No touch, nothing to do 
    Set,        // user wants to set a new m
    Switch      // user wants to switch screens
  };


  TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

  struct TouchScreen
  {
    // TouchScreen *ts;
    TouchCommandType cmd_type;
    prime_t new_m;

    void init()
    {
      // ts = new TouchScreen( XP, YP, XM, YM, 300 );
      pinMode(13, OUTPUT);
    }

    // Tapping the top left corner of the display will switch screens
    // Tapping any where else will cause the prime clock to restart
    // from a value of m that is mapped to the co-ordinates
    void interpret_touch( TSPoint p )
    {
      // scale raw touch coordinates to normalized coordinates
      int y = map(p.x, TS_MINX, TS_MAXX, 0, 1024),  // We've rotated the display
          x = map(p.y, TS_MINY, TS_MAXY, 1024, 0);

      if( ( x < 100 ) & ( y < 100 ) )
      {
        cmd_type = TouchCommandType::Switch;      
      }
      else
      {
        cmd_type = TouchCommandType::Set;      
        new_m = (prime_t) x * (prime_t) y * (prime_t) y; 
      }
    }

    // return true if there was a touch. Command type will be
    // in the cmd_type variable
    bool poll()
    {
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);

      // if sharing pins, you'll need to fix the directions of the touchscreen pins
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      // we have some minimum pressure we consider 'valid'
      // pressure of 0 means no pressing!
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
      {
        interpret_touch( p );
        return true;
      }
      else
      {
        cmd_type = TouchCommandType::Nothing;
        return false;
      }
    }
    
  };
}

#endif // _TOUCH_H_
