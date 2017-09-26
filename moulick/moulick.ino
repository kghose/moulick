/*
  Moulick is a mathematical toy that uses an Arduino to continually compute 
  and display prime numbers.
  
  See the Readme file for details.
*/  
#include "touch.h"
#include "moulickapp.h"


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


//serialport::SerialPort si;
moulickapp::MoulickApp moulick;
touchscreen::TouchScreen ts;

#define REDRAW_PERIOD 4   // We ask for a re-draw at 1/4 the rate of the touchscreen poll
#define TS_POLL_RATE  16  // We poll the touchscreen at 12 Hz
uint8_t redraw_counter = REDRAW_PERIOD;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  moulick.init();
  ts.init( moulick.tft );

  initialize_timer1( TS_POLL_RATE );  // Initialize this last
}

void loop() 
{
  // put your main code here, to run repeatedly:
  moulick.next_tick();
}


void poll()
{
  // poll the touch screen
  if( ts.poll() )
  {
    switch( ts.cmd_type )
    {
      case touchscreen::TouchScreen::TouchCommandType::Set:
        moulick.set_new_m( ts.new_m );
        break;

      case touchscreen::TouchScreen::TouchCommandType::Switch:
        moulick.toggle_screen();
        break;
    }
  }  

  if( --redraw_counter == 0 )
  {
    // do a partial display refresh (the App decides if this is needed)
    moulick.refresh_display();
    redraw_counter = REDRAW_PERIOD;
  }
}


ISR(TIMER1_COMPA_vect)  // timer compare interrupt service routine
{
  poll();
}
