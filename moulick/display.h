/*
  Supplies code to translate the PrimeClock structure into a visual display

  The Display struct contai
  into a visual display description. The code maintains state for each of the 
  components and indicates if a component needs updating (drawing). This description
  is hardware independent but contains virtual functions that can be overridden in
  a derived class that implements hardware dependent code that actually draws on
  a display.

  The function draw() should be over-ridden for each component. The function update()
  if called whenever a component needs updating and this calls draw()
*/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include "primes.h"

namespace display {


#define TFT_ID 0x9341  // This was my display driver chip

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define LTBLUE  0b1000010000011111
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



using namespace primes;


void initialize_timer1(float refresh_rate = 20)
{
  /*
   * 16 MHz clock
   * 16 bit register (65536 vals)
   * 256 prescaler
   * 
   * 16e6 / 256 = 62500
   * 62500 / 20 Hz = 3125
   */
  
  // 20 hz is good. Higher rates run the risk of not touching the main loop at all!

  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = (int) (62500 / refresh_rate) ;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


#define BACKGROUND BLACK

void initialize_display()
{
  tft.reset();
  tft.begin( TFT_ID );
  tft.setRotation(1);  // Landscape, with the Uno's USB port  to the right
  tft.fillScreen(BACKGROUND);
}


// Positions of display components

// The yellow circle round the prime digits and forming
// the base of the radial chart
#define CHART_X 320 / 2
#define CHART_Y 240 / 2
#define CHART_BASE_R0 75
#define CHART_BASE_R1 77
#define CHART_BASE_COL CYAN

// Radial chart extents
#define CHART_R0 80
#define CHART_DR 38
#define CHART_LINE_COL GREEN
#define CHART_PRIME_COL RED

#define CHART_N 250

// Display components

struct RadialChart
{
  void draw_fixture() 
  { 
    for(int r = CHART_BASE_R0; r <= CHART_BASE_R1; r++)
      tft.drawCircle( CHART_X, CHART_Y, r, CHART_BASE_COL ); 
  }

  void radial_line( uint16_t n, float f, uint16_t color )
  {
    float theta = 2 * M_PI * (float)(n % CHART_N) / (float) CHART_N,
          st = sin( theta ), 
          ct = cos( theta );
    int x0 = CHART_X + CHART_R0 * st,
        y0 = CHART_Y - CHART_R0 * ct,
        x1 = CHART_X + ( CHART_R0 + CHART_DR * f ) * st,
        y1 = CHART_Y - ( CHART_R0 + CHART_DR * f ) * ct;
    tft.drawLine( x0, y0, x1, y1, color );
  }

  void draw(ull m, float f, bool is_prime = false)
  {
    for( int i = 0; i <= 10 ; i++)
      radial_line(m, (float) i / 10.0, BACKGROUND ); 
      // Erase lines ahead to give a nice advancing blank border
    radial_line(m, f, is_prime ? CHART_PRIME_COL : CHART_LINE_COL );
  }

};


// This takes care of displaying a series of digits
enum class Alignment{Left=0, Right, Center};

struct Odo
{
  // It's faster to print the last string in BG color than to print a
  // whole rectangle in BG color in order to erase the previous display
  char last_odo[ ull_digits ]; 
  int16_t x, y;
  uint16_t color;
  byte font_size;
  Alignment alignment;

  void set(int16_t _x, int16_t _y, byte _font_size, uint16_t _color, Alignment al)
  { 
    x = _x; y = _y; font_size = _font_size; color = _color; alignment = al;
    last_odo[ 0 ] = '\0';
  }
  
  void draw_string( const char* d, uint16_t col )
  {
    int16_t _x;
    switch( alignment )
    {
      case Alignment::Left:    
                    _x = x; 
                    break;
      case Alignment::Right:  
                     _x = x - strlen( d ) * 6 * font_size;
                    break;
      case Alignment::Center:
                    _x = x - strlen( d ) * 3 * font_size;
                    break;
    }
        
    tft.setCursor(_x, y);
    tft.setTextSize( font_size );
    tft.setTextColor( col );  
    tft.print( d );    
  }
  
  void draw(const char *d)
  {
    draw_string( last_odo, BACKGROUND );
    for( int i = 0, j = 0; i <= ull_digits; i++ )
      if( d[ i ] != 32)
        last_odo[ j++ ] = d[ i ];
    draw_string( last_odo, color );
  }

};


// Come together to form a display
struct Display
{
  PrimeClock pc;
  RadialChart radial_chart;
  Odo last_prime;

  Display()
  {
    last_prime.set( CHART_X, CHART_Y - 5, 2, WHITE, Alignment::Center );
  }

  // Draw any fixed background objects
  void draw_fixtures()
  {
    radial_chart.draw_fixture();
  }


  /*
    This is called by the Arduino loop() code. It blocks until the current number has
    been tested and updates the display apropriately.  
  */
  void update()
  {
    noInterrupts();
    // We may get glitches if we don't stop the background drawing routine
    bool is_prime = pc.check_next();
    radial_chart.draw( pc.m, min(((float) pc.k - 1) / (float) pc.k_max, 1.0), is_prime );
    if( is_prime )
    {
      last_prime.draw( pc.last_prime_digits.digits() );
    }
    interrupts();                
  }

  /*
    The update function blocks until the current number has been tested. This function
    is meant to be driven by an interrupt and updates the circular display based on
    how many factors have been tested for the current candidate number. It serves
    as a heartbeat as well as a dramatic display for larger primes that take a while
    to assess.
  */
  void heartbeat()
  {
    // dial.draw( (float) pc.k / (float) pc.k_max );
  }

};

} // display

#endif //_DISPLAY_H_

