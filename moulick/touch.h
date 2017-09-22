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
#include "display.h"  // for BACKGROUND color. TODO: May be move this to MoulickApp.h


namespace touchscreen 
{
  using namespace primes;

  // A prime_t display but that can be moved
  struct MovableDigits
  {
    Elegoo_TFTLCD *tft;
    char buf[ p_max_d + 1 ];
    char *number;
    int x, y;
    uint8_t text_size;

    void init( Elegoo_TFTLCD *_tft )
    {
      tft = _tft;
      x = 0,
      y = 0;
      buf[ 0 ] = '\0';
      number = buf;
      text_size = 1;
    }

    void draw( int _x, int _y, prime_t m )
    {
      if( (x == _x) && (y == _y) ) return;
      draw_digits( BACKGROUND );
      number = prime_t_to_str( m, buf );
      x = _x; y = _y;
      draw_digits( BLUE );      
    }

    void draw_digits( uint16_t color )
    {
      tft->setCursor( x, y );
      tft->setTextSize( text_size );
      tft->setTextColor( color );  
      tft->print( number );
    }
  };


  enum class TouchCommandType { 
    Nothing=0,  // No touch, nothing to do 
    Set,        // user wants to set a new m
    Switch      // user wants to switch screens
  };


  TouchScreen ts = TouchScreen( XP, YP, XM, YM, TS_OHMS );

  struct TouchScreen
  {
    TouchCommandType cmd_type;
    prime_t new_m;
    MovableDigits md;

    void init( Elegoo_TFTLCD *tft )
    {
      pinMode(13, OUTPUT);
      md.init( tft );
    }

    TSPoint get_touch_point()
    {
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);

      // if sharing pins, you'll need to fix the directions of the touchscreen pins
      // Need to 
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      // scale raw touch coordinates to normalized coordinates
      // swap x/y since we've rotated the display
      int tmp_x = p.x;
      p.x = map(p.y, TS_MINY, TS_MAXY, 1024, 0);
      p.y = map(tmp_x, TS_MINX, TS_MAXX, 0, 1024);

      return p;
    }

    // Tapping the bottom left corner of the display will switch screens
    // Tapping any where else will cause the prime clock to restart
    // from a value of m that is mapped to the co-ordinates
    void interpret_touch( TSPoint p )
    {
      if( ( p.x < 100 ) & ( p.y < 100 ) )
      {
        cmd_type = TouchCommandType::Switch;      
      }
      else
      {
        cmd_type = TouchCommandType::Set;
        while( p.z > MINPRESSURE && p.z < MAXPRESSURE )
        {
          new_m = (prime_t) p.x * (prime_t) p.y * (prime_t) p.y;
          md.draw( p.x, p.y, new_m );
          delay( 250 );
          p = get_touch_point();
        }
      }
    }

    // return true if there was a touch. Command type will be
    // in the cmd_type variable
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
