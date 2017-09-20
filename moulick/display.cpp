/*
  Translates the PrimeClock structure into a visual display
*/
#include "display.h"

namespace display {

  using namespace primes;

  void RadialChart::init( Elegoo_TFTLCD *_tft )
  {
    tft = _tft;
    tft->fillCircle( CHART_X, CHART_Y, CHART_BASE_R1, CHART_BASE_COL );
    tft->fillCircle( CHART_X, CHART_Y, CHART_BASE_R0, BACKGROUND );  
  }

  void RadialChart::draw(prime_t m, float f, byte m_type)
  {
    float theta = D_THETA * (float)(m % CHART_N);
    uint16_t color = CHART_LINE_COL;
    switch( m_type )
    {
      case PRIME: 
        color = CHART_PRIME_COL;
        break;
      case TWIN: 
        color = CHART_TWIN_PRIME_COL;
        break;
      case PALINDROME: 
        color = CHART_PALINDROMIC_PRIME_COL;
        break;        
      case TWIN & PALINDROME: 
        color = CHART_TWIN_AND_PALINDROMIC_PRIME_COL;
        break;
    }

    cursor( theta + D_THETA, BACKGROUND );
    cursor( theta, CHART_BASE_COL );
    
    // Erase lines ahead to give a nice advancing blank border
    for( int i = 1; i < 5; i++) radial_line( theta, i / 4.0, BACKGROUND ); 
    radial_line( theta + D_THETA, 1.0, BACKGROUND );
    radial_line( theta, f, color );
  }

  void RadialChart::cursor( float theta, uint16_t color )
  {
    float st = sin( theta ), 
          ct = cos( theta );
    int x0 = CHART_X + CHART_BASE_R0 * st,
        y0 = CHART_Y - CHART_BASE_R0 * ct,
        x1 = CHART_X + CHART_BASE_R1 * st,
        y1 = CHART_Y - CHART_BASE_R1 * ct;
    tft->drawLine( x0, y0, x1, y1, color );
  }

  void RadialChart::radial_line( float theta, float f, uint16_t color )
  {
    float st = sin( theta ), 
          ct = cos( theta );
    int x0 = CHART_X + CHART_R0 * st,
        y0 = CHART_Y - CHART_R0 * ct,
        x1 = CHART_X + ( CHART_R0 + CHART_DR * f ) * st,
        y1 = CHART_Y - ( CHART_R0 + CHART_DR * f ) * ct;
    tft->drawLine( x0, y0, x1, y1, color );
  }

  
  void DigitDisplay::init(
    Elegoo_TFTLCD *_tft,
    int16_t _x, int16_t _y, byte _font_size, uint16_t _color, 
    Alignment al)
  {
    tft = _tft; 
    x = _x; y = _y; font_size = _font_size; color = _color; 
    alignment = al;
    number[ 0 ] = '\0';
  }

  void DigitDisplay::draw( const char *d )
  {
    draw_digits( number, BACKGROUND );
    update_number( d );
    draw_digits( number, color );
  }
  
  void DigitDisplay::draw_digits( const char* d, uint16_t col )
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
        
    tft->setCursor(_x, y);
    tft->setTextSize( font_size );
    tft->setTextColor( col );  
    tft->print( d );
  }

  void DigitDisplay::update_number( const char* d )
  {
    int i = 0, j = 0;
    for( ;; )
    {
      number[ i ] = d[ j ];
      if( d[ j ] == '\0') break;
      i++; j++;
    }
  }

    
  void Clock::init( Elegoo_TFTLCD *_tft, PrimeClock *_pc )
  {
    tft = _tft;
    pc = _pc;
    tft->fillScreen( BACKGROUND );
    
    radial_chart.init(  tft );
    digit_display.init( tft, 
                        CHART_X, CHART_Y - 5, 2, WHITE, 
                        DigitDisplay::Alignment::Center );
  }  
  
  void Clock::draw()
  {
    byte m_type = COMPOSITE;
    if( pc->is_prime ) m_type = PRIME;
    if( pc->is_twin_prime ) m_type &= TWIN;
    if( pc->is_palindromic_prime ) m_type &= PALINDROME; 
    radial_chart.draw( pc->m, min( pc->fraction_tested(), 1.0 ), m_type );

    if( pc->is_prime ) digit_display.draw( pc->m_as_string() );
  
    m_at_last_draw = pc->m;
  }

  void Clock::partial_draw()
  {
    // We only refresh this display if we are stuck on the same prime
    if ( m_at_last_draw != pc->m ) 
    {
      m_at_last_draw = pc->m; 
      // We've moved to a new m. We want to remember this, because if the 
      // function is called again with this same m, we want to refresh
      return;      
    }

    byte m_type = COMPOSITE;  // We don't know what it is yet
    radial_chart.draw( pc->m, min( pc->fraction_tested(), 1.0 ), m_type );
  }

  void Histogram::init( Elegoo_TFTLCD *_tft )
  {
    tft = _tft;
    for( int i = 0 ; i < 9; i++ ) values[ i ] = 0;
  }

  void Histogram::draw( const float *h )
  {

  }

  void Histogram::draw_hist( uint16_t col )
  {

  } 

  void Histogram::update_values( const float *h )
  {

  }


  void Stats::init( Elegoo_TFTLCD *_tft, PrimeClock *_pc ) 
  { 
    tft = _tft;
    pc = _pc;
    tft->fillScreen( BACKGROUND );

    // Labels
    tft->setTextSize( 1 );

    tft->setCursor( CNTR_X - 6 * 1 * 6, CNTR_PRIME_Y - 10);
    tft->setTextColor( CHART_PRIME_COL );  
    tft->print( "Primes" );

    tft->setCursor( CNTR_X - 6 * 1 * 11, CNTR_TWIN_Y - 10);
    tft->setTextColor( CHART_TWIN_PRIME_COL );  
    tft->print( "Twin primes" );

    tft->setCursor( CNTR_X - 6 * 1 * 18, CNTR_PLNDR_Y - 10);
    tft->setTextColor( CHART_PALINDROMIC_PRIME_COL );  
    tft->print( "Palindromic primes" );
    
    // Odometers
    primes_found.init( tft, 
                      CNTR_X, CNTR_PRIME_Y, 2, WHITE, 
                      DigitDisplay::Alignment::Right );
    twins_found.init( tft, 
                      CNTR_X, CNTR_TWIN_Y, 2, WHITE, 
                      DigitDisplay::Alignment::Right );
    palindromes_found.init(  tft, 
                      CNTR_X, CNTR_PLNDR_Y, 2, WHITE, 
                      DigitDisplay::Alignment::Right );     
  }

  void Stats::draw()
  {
    if( !pc->is_prime ) return;
    primes_found.draw( prime_t_to_str( pc->primes_found, m_string ) );
    twins_found.draw( prime_t_to_str( pc->twin_primes_found, m_string ) );
    palindromes_found.draw( prime_t_to_str( pc->palindromic_primes_found, m_string ) );    
  }



  void MoulickApp::init()
  {
    tft = new Elegoo_TFTLCD(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
    pc = new PrimeClock();
    initialize_display();
  }

  void MoulickApp::initialize_display()
  {
    tft->reset();
    tft->begin( TFT_ID );
    tft->setRotation(1);  // Landscape, with the Uno's USB port  to the right

    switch_to( Screen::Corona );
  }

  void MoulickApp::toggle_screen()
  {
    if( screen_to_display == Screen::Corona )
      switch_to( Screen::Stats );
    else
      switch_to( Screen::Corona );
  }

  void MoulickApp::switch_to( Screen scr )
  {
    screen_to_display = scr;
    switch( screen_to_display )
    {
      case Screen::Corona:
        corona_disp.init( tft, pc );
        break;

      case Screen::Stats:
        stats_disp.init( tft, pc );
        break;
    }
  }

  void MoulickApp::next_tick()
  {
    pc->check_next();
        
    noInterrupts();
    // We may get glitches if we don't stop any background drawing routine
    switch( screen_to_display )
    {
      case Screen::Corona:
        corona_disp.draw();
        break;

      case Screen::Stats:
        stats_disp.draw();
        break;
    }
    interrupts();
  }

  // We call this periodically (say using an interrupt) to animate the
  // display when prime testing is taking long and the display would
  // otherwise look frozen
  void MoulickApp::refresh_display()
  {
    switch( screen_to_display )
    {
      case Screen::Corona:
        corona_disp.partial_draw();
        break;

      case Screen::Stats:
        // There is no partial refresh for stats screen
        break;
    }
  }

  void MoulickApp::set_new_m( prime_t m)
  {
    pc->restart_clock_from( m );
    switch_to( screen_to_display );
  }

} // display
