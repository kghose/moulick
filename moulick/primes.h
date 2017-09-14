/*
  This contains code that handles the generation of primes
  and some abstractions required for their display and metrics
*/
#ifndef _PRIMES_H_
#define _PRIMES_H_

namespace primes {
  
typedef unsigned long long ull;


// Good enough code for finding the square root of m
// We only need to check divisors up to this
ull sqrt(ull m)
{
    float sqrt_m = m;
    for(int i = 0; i < 6; i++)
        sqrt_m = (sqrt_m * sqrt_m + m) / (2 * sqrt_m);
    return (ull) sqrt_m;
}


/*
// How many primes <= m do we expect?
// https://primes.utm.edu/howmany.html
ull pi(ull m)
{
  return (ull) (m / log(m));
}
*/


struct PrimeClock
{
  ull current_n,
      factors_to_check,
      factors_checked,
      last_prime, 
      primes_found,
      prime_pairs_found;

  PrimeClock()
  {
    current_n = 1;
    factors_to_check = 0;
    last_prime = 0;
    primes_found = 0;
    prime_pairs_found = 0;
  }

  // https://en.wikipedia.org/wiki/Primality_test
  // Implemented as a member function so that we can set the
  // number of factors to check, and current factor as member
  // variables. This allows us to show the internal progress
  // of the prime test if we pause the function 
  // (e.g. via an interrupt)
  bool is_prime(ull m)
  {
      if (m == 2 | m == 3) return true;
  
      if (m % 2 == 0) return false;
      if (m % 3 == 0) return false;
      
      ull m0 = sqrt(m),
          k = 1,
          m2 = 6 * k;
          
      // factors_to_check = (m0 * 1) / 6;
      
      while(m2 - 1 <= m0)  // divisible by 6*k +/- 1 ?
      {
          if (m % (m2 - 1) == 0) return false;
          if (m % (m2 + 1) == 0) return false;
          k++;
          m2 = 6 * k;        
      }
  
      return true;
  }


  // Increment the clock and test if this next number is prime
  bool test_next()
  {
    current_n++;
    if(is_prime(current_n))
    {
      if(current_n - last_prime == 2) prime_pairs_found++;
      last_prime = current_n;
      primes_found++;
      return true;
    }
    return false;
  }


  
};

const uint8_t max_decimal_digits = 20;  // log10(2^64 -1)
char ull_buf[ max_decimal_digits + 1 ];  // Null terminated string 

// Given an ULL integer turn it into a string
// This actually manipulates the global buffer c and
// returns a const pointer to it for the printing/displaying function   
const char* printull(ull m)
{
  char *s = ull_buf + max_decimal_digits;
  *s = '\0';  // Null terminated
  for(int i = 0 ; i < max_decimal_digits ; i++)
  {
    s--;
    *s = m ? (char) (m % 10 + 48) : 32;
    // Convert remainder to ASCII for 0, 1, 2....
    // unless m == 0 in which case print a blank
    m /= 10;
  }
  return (const char*) ull_buf;  
}


/*
 * This graph keeps folding onto itself
 * We start out with an x range say 1 to 10, each carrying a y value
 * As we go along x, we fill in the y-values
 * When we get to 11 (go past the range) we fold the graph by half:
 *   We take each adjacent bin, add the values and fold them into the left half of the range
 *   The upper half of the bins are now empty. We then consider the graph to represent double
 *   the range as before. We continue indefinitely as more x-values are added 
 */
const uint8_t graph_bins = 20;
struct FoldingGraph
{
  ull primes[ graph_bins ];
};


}


#endif // _PRIMES_H_
