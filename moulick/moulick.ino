/*
  Main file for Moulick - a mathematical toy that uses an Aurdino to
  continually compute primes and display them along with some fun
  statistics.
  
  Primes are tested using simple division. The current number being
  tested is displayed along with the last prime found. 
  A circular indicator shows testing progress for the current number. 
  Also shown are counts for how many primes have been found, how many twin primes and how many 
  palindromes. Two histograms show the frequency of the first and last digits.
  

  Code organization

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
  prime_display.pc.m = 1000000;


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
  /*
  Serial.println( (uint32_t) prime_display.pc.m );
  Serial.println( (uint32_t) prime_display.pc.k );
  Serial.println( (uint32_t) prime_display.pc.k_max );
  */
}

