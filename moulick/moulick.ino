/*
  Moulick is a mathematical toy that uses an Arduino to continually compute 
  and display prime numbers.
  
  See the Readme file for details.
*/  
#include "touch.h"
#include "display.h"

#if 0
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

ISR(TIMER1_COMPA_vect)  // timer compare interrupt service routine
{
  prime_display.heartbeat();
}
#endif

//serialport::SerialPort si;
display::MoulickApp moulick;
touchscreen::TouchScreen ts;

void setup() 
{
  // put your setup code here, to run once:
  // serialport::init();
  Serial.begin(9600);
  moulick.init();
  ts.init();

  // display::initialize_timer1(20);  // 20 Hz heartbeat do this last
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if( ts.poll() )
  {
    switch( ts.cmd_type )
    {
      case touchscreen::TouchCommandType::Set:
        Serial.println( ts.new_m );
        moulick.set_new_m( ts.new_m);
        break;

      case touchscreen::TouchCommandType::Switch:
        // Not implemented
        break;
    }
  }  
  moulick.update();
}


