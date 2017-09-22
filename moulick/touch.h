/*
  Code to handle touch screen interaction. 
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
  
  TouchScreen ts = TouchScreen( XP, YP, XM, YM, TS_OHMS );

  prime_t point_to_prime(TSPoint p)
  {
    return 111848 * ( p.y - 160 ) * p.x;
  }

  TSPoint get_touch_point()
  {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    // scale raw touch coordinates to normalized coordinates
    // this is the original system, so for us x is vertical y is horizontal
    // 
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_X);
    p.y = map(p.y, TS_MINY, TS_MAXY, SCREEN_Y, 0);
  
    return p;
  }

  struct TouchScreen
  {
    enum class TouchCommandType { 
      Nothing=0,  // No touch, nothing to do 
      Set,        // user wants to set a new m
      Switch      // user wants to switch screens
    };
  
    TouchCommandType cmd_type;
    prime_t new_m;
    int screen_height, screen_width;

    void init( Elegoo_TFTLCD *tft )
    {
      pinMode(13, OUTPUT);
      cmd_type = TouchCommandType::Nothing;
    }

    // Tapping the left half of the display (with some margin) will switch screens
    // Tapping on the right half will cause the prime clock to restart
    // from a value of m that is mapped to the co-ordinates
    void interpret_touch( TSPoint p )
    {
      if( ( p.y < 100 ) )
      {
        cmd_type = TouchCommandType::Switch;      
      }
      else
      {
        cmd_type = TouchCommandType::Set;
        new_m = point_to_prime( p );
        delay( 250 );  // Give us time to remove the stylus
      }
    }
  
    bool poll()
    {
      cmd_type = TouchCommandType::Nothing;      
      TSPoint p = get_touch_point();
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
