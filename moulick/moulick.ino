/*
  Moulick is a mathematical toy that uses an Aurdino to continually compute primes 
  and display them along with some fun statistics.
  
  Display
  -------
  The display consists of a clock screen and a stats screen. 
  We can switch between them by tapping on the display.
  
  The clock screen shows the most recently found prime at the center 
  with a radial chart surrounding it. The radial chart shows the fraction 
  of divisors that had to be tested for each of the last 999 numbers. 
  Primes are marked in red and show up as the longest bars (= 1.0) while
  non-primes are marked in white. Even numbers show up as the  
  Fun to see are non-primes which also had to be tested  extensively, 
  such as squares of primes. Even numbers show up flat (0) on this display.

  The stats screen shows:
  
  1. Total numbers checked
  2. Primes found + fraction
  3. Twin primes found + fraction
  4. Palindromes found + fraction
  5. Histograms of the first and last digit frequencies
  

  Code organization
  -----------------

  moulick.ino - the main "sketch"
  primes.h - code for computing primality
  display.h - hardware dependent code that drives the display 
  
  Hardware

  Arudino Uno + Elegoo TFT display


  Technical notes
  
  Display flickr
  
  To reduce display flickr it is best to only update the parts of the screen that have changed
  This means updating each component of the display separately, and potentially at separate times
  For example, the last prime, prime counts and digit histograms are updated only when a new prime
  is found. The circular indicator is interrupt driven and we cheat by only drawing the current
  position (and not the whole circle). We also don't draw it unless we dwell on a number for a  
  while.
  
  From https://github.com/notro/fbtft/issues/401 the display itself is updated at 70Hz, but it
  takes time for the microcontroller to load data into the display RAM.
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

