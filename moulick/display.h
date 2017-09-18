/*
  Hardware dependent code to translate the PrimeClock structure into a visual display.
*/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include "tftconstants.h"  // Chip ID, Pinouts etc. for TFT display
#include "primes.h"

/*
  The following constants determine the layout and color of the display
*/

// The yellow circle round the prime digits that forms
// the base of the radial chart
#define CHART_X 320 / 2
#define CHART_Y 240 / 2
#define CHART_BASE_R0 75
#define CHART_BASE_R1 77
#define CHART_BASE_COL YELLOW

// Radial chart extents
#define CHART_R0 80
#define CHART_DR 38
#define CHART_LINE_COL GREEN
#define CHART_PRIME_COL RED
#define CHART_TWIN_PRIME_COL BLUE
#define CHART_PALINDROMIC_PRIME_COL CYAN
#define CHART_TWIN_AND_PALINDROMIC_PRIME_COL WHITE

// How many numbers does the radial chart represent
#define CHART_N 250
#define D_THETA ( 2 * M_PI / (float) CHART_N )

// The erase/background color
#define BACKGROUND BLACK


namespace display {

  using namespace primes;

  #define NONPRIME    0b00000000
  #define PRIME       0b00000001
  #define TWIN        0b00000010
  #define PALINDROME  0b00000100

  #define PRIME_TWIN             ( PRIME | TWIN )
  #define PRIME_TWIN_PALINDROME  ( PRIME | TWIN | PALINDROME )
  
  struct RadialChart
  {
    Elegoo_TFTLCD *tft;              // This is the pysical display    
    void init(Elegoo_TFTLCD *_tft);  // Draw fixed elements of the display
    void draw( prime_t m, float f, byte m_type );
    void cursor( float theta, uint16_t color );    
    void radial_line( float theta, float f, uint16_t color );
  };
  
  struct DigitDisplay
  {
    Elegoo_TFTLCD *tft;              // This is the pysical display
    enum class Alignment{Left=0, Right, Center};
    char number[ p_max_d ]; 
    int16_t x, y;
    uint16_t color;
    byte font_size;
    Alignment alignment;
    
    void init(
      Elegoo_TFTLCD *_tft, 
      int16_t _x, int16_t _y, byte _font_size, uint16_t _color, 
      Alignment al);
    void draw( const char *d );
    void draw_digits( const char* d, uint16_t col );    
    void update_number( const char* d );    
  };    


  // The radial/coronal display
  struct Clock
  {
    Elegoo_TFTLCD *tft;  
    PrimeClock const *pc;
    RadialChart radial_chart;
    DigitDisplay digit_display;

    void init( Elegoo_TFTLCD *_tft, PrimeClock *_pc );
    void draw();
  };


  // The stats display 
  // TODO: This is a stub right now
  struct Stats
  {
    Elegoo_TFTLCD *tft;  
    PrimeClock const *pc;

    void init( Elegoo_TFTLCD *_tft, PrimeClock *_pc ) 
    { 
      tft = _tft;
      pc = _pc;
      tft->fillScreen( RED );
    }
    void draw() { ; }
  };


  //
  struct MoulickApp
  {
    Elegoo_TFTLCD *tft;  
    PrimeClock const *pc;
    
    Clock corona_disp;
    Stats stats_disp;
    enum class Screen{Corona=0, Stats};
    Screen screen_to_display;

    void init();
    void initialize_display();    
    void switch_to( Screen scr );    
    void update();
  };

} // display

#endif //_DISPLAY_H_
