/*
  Moulick is a mathematical toy that uses an Arduino to continually compute 
  and display primes numbers.
  
  See the Readme file for details.
*/  
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

display::MoulickApp moulick;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while( !Serial ) { ; }  // Wait for serial port to connect
  Serial.println("Moulick - a prime number toy");
  
  moulick.init();

  // JUST FOR TESTING
  // prime_display.pc.m = 1000000;


  // display::initialize_timer1(20);  // 20 Hz heartbeat do this last
}

void loop() 
{
  // put your main code here, to run repeatedly:
  moulick.update();

  /*
  // Check serial port for commands
  if( Serial.available() )
  {
    Serial.print( Serial.readString() );
  }
  */
}


