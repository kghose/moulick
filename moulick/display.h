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


// Display components

#define DIAL_X 100
#define DIAL_Y 120
#define DIAL_R 100
#define DIAL_COL YELLOW

struct Dial
{
  float last_f;
  void draw_fixture() { tft.drawCircle( DIAL_X, DIAL_Y, DIAL_R, DIAL_COL ); }
  void bead(float f, uint16_t color)
  {
    int x = DIAL_X + (DIAL_R - 7) * sin( 2 * M_PI * f),
        y = DIAL_Y - (DIAL_R - 7) * cos( 2 * M_PI * f);
    tft.drawCircle( x, y, 5, color );
    
  }
  void draw(float f)
  {
    bead(last_f, BACKGROUND );
    bead(f, BLUE );
    last_f = f;
  }
};


struct Odo
{
  Digits last_odo;
  int16_t x, y, font_size;
  uint16_t color;

  void letters( const char* d, uint16_t color)
  {
    // x - ull_digits * 3 * font_size, y - 4 * font_size
    char _d[ ull_digits + 1];
    for(int i = 0, j = 0; i < ull_digits + 1; i++)
      if( d[ i ] != 32 )
        _d[ j++ ] = d[ i ];
    
    tft.setCursor(x, y);
    tft.setTextSize( font_size );
    tft.setTextColor( color );  
    tft.print( _d );    
  }
  void draw(Digits &d)
  {
    letters( last_odo.digits(), BACKGROUND );
    last_odo.set( d );
    letters( last_odo.digits(), WHITE );
  }
};


// Come together to form a display
struct Display
{
  PrimeClock pc;
  Dial dial;
  Odo last_prime;

  Display()
  {
    last_prime.x = 50;
    last_prime.y = 110;
    last_prime.font_size = 2;
  }

  // Things like the circular clock and the histograms
  void draw_fixtures()
  {
    dial.draw_fixture();
  }


  /*
    This is called by the Arduino loop() code. It blocks until the current number has
    been tested and updates the display apropriately.  
  */
  void update()
  {
    if( pc.check_next() )
    {
      noInterrupts();
      last_prime.draw( pc.last_prime_digits );
      interrupts();            
    }
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
    dial.draw( (float) pc.k / (float) pc.k_max );
  }

};


/*
// Represents a numeric display
struct Numeric
{
  int x, y, font_size, font_color;
  char current_digits[ ull_digits + 1 ];  // We are currently displaying this
  char new_digits[ ull_digits + 1 ];      // We need to display this next

  virtual void draw() = 0;

  // Update with new digits
  void update(char digits[])
  {
    copy(new_digits, digits, ull_digits + 1);
    draw();
    copy(current_digits, digits, ull_digits + 1);    
  }
};


struct Histogram




struct Display
{
  Display() {;}
  void display(const PrimeClock &pc, bool pf)
  {
    ;
  }
};

*/

} // display

#endif //_DISPLAY_H_

