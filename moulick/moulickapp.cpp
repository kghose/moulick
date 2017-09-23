#include "moulickapp.h"

namespace moulickapp
{

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

}