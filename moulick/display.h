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

// Coronal display

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

// Stats page

// Histograms
#define HIST_X 170
#define HIST_W 140

#define HIST_1_Y 110
#define HIST_N_Y 10

// Counters
#define CNTR_X 150

#define CNTR_PRIME_Y  40
#define CNTR_TWIN_Y   120
#define CNTR_PLNDR_Y  200

// The erase/background color
#define BACKGROUND BLACK


namespace display {

  using namespace primes;


  // KG's modification for Bresenham's algorithm for the radial plot
  // modified from https://gist.github.com/bert/1085538#file-plot_line-c
  inline void fractional_bresenham(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    float f,
    uint16_t color,
    Elegoo_TFTLCD *tft)
  {
    int16_t dx    =  abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1,
            dy    = -abs( y1 - y0 ), sy = y0 < y1 ? 1 : -1, 
            err   = dx + dy, e2, /* error value e_xy */
            x_end = x0 + sx * dx * f;  // fractional end point
   
    for (;;)
    {
      tft->drawPixel( x0, y0, color );
      if( ((sx == 1) && (x0 >= x_end)) || ((sx == -1) && (x0 <= x_end)) ) break;
      e2 = 2 * err;
      if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
      if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
  }

  // The values are designed so that we can meaningfully AND them
  #define COMPOSITE   0b000
  #define PRIME       0b111
  #define TWIN        0b101
  #define PALINDROME  0b110

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

    prime_t m_at_last_draw;  
    // Used to determine if a partial_draw should be executed

    void init( Elegoo_TFTLCD *_tft, PrimeClock *_pc );
    void draw();
    void partial_draw();  // A partial update when prime testing is taking long
  };


  struct Histogram
  {
    Elegoo_TFTLCD *tft;     // This is the pysical display
    int16_t x, y, w;    
    uint8_t values[ 9 ];    // What we currently display
    void init( Elegoo_TFTLCD *_tft );
    void draw( const float *h );   
    void draw_hist( uint16_t col );    
    void update_values( const float *h );     
  };

  // The stats display 
  // TODO: This is a stub right now
  struct Stats
  {
    Elegoo_TFTLCD *tft;  
    PrimeClock const *pc;
    char m_string[ p_max_d + 1 ];  // string to hold prime_t numbers   
    DigitDisplay primes_found, twins_found, palindromes_found;
    Histogram h1, hn;

    void init( Elegoo_TFTLCD *_tft, PrimeClock *_pc );
    void draw();
  };


  //
  struct MoulickApp
  {
    Elegoo_TFTLCD *tft;  
    PrimeClock *pc;
    
    Clock corona_disp;
    Stats stats_disp;
    enum class Screen{Corona=0, Stats};
    Screen screen_to_display;
    bool just_restarted;  // We need this to avoid a refresh

    void init();
    void initialize_display();
    void toggle_screen();
    void switch_to( Screen scr );    
    void next_tick();
    void refresh_display(); // A partial redraw when prime testing is taking long  
    void set_new_m( prime_t m);
  };

} // display

#endif //_DISPLAY_H_
