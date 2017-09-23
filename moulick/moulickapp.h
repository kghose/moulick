/*
  Ties all the components together
*/
#include "display.h"

namespace moulickapp
{
  using namespace display;

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

}
