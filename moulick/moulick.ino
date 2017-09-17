/*
  Moulick is a mathematical toy that uses an Arduino to continually compute 
  and display primes numbers.
  
  See the Readme file for details.
*/  
#include "display.h"

display::Display prime_display;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Moulick - a prime number toy");
  display::initialize_display();
  
  prime_display.draw_fixtures();

  // JUST FOR TESTING
  // prime_display.pc.m = 1000000;


  display::initialize_timer1(20);  // 20 Hz heartbeat do this last
}

void loop() 
{
  // put your main code here, to run repeatedly:
  prime_display.update();
}


ISR(TIMER1_COMPA_vect)  // timer compare interrupt service routine
{
  prime_display.heartbeat();
}

