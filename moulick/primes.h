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
    for( int i = 0; i < 6; i++ )
        sqrt_m = (sqrt_m * sqrt_m + m) / (2 * sqrt_m);
    return (ull) sqrt_m;
}


const unsigned char ull_digits = 20;  // log10(2^64 -1)

// Convert an ull to a string
struct Digits
{
  char ull_buf[ ull_digits + 1 ];  // Null terminated string 

  void set( ull m )
  {
    char *s = ull_buf + ull_digits;
    *s = '\0';  // Null terminated
    for(int i = 0 ; i < ull_digits ; i++)
    {
      s--;
      *s = m ? (char) (m % 10 + 48) : 32;
      // Convert remainder to ASCII for 0, 1, 2....
      // unless m == 0 in which case print a blank
      m /= 10;
    }
  }

  bool is_palindrome()
  {
    unsigned char i = 0, j, k;
    while( ull_buf[i] == 32 ) i++;
    if( i == ull_digits ) return false;  // Blank
    for( j = i, k = ull_digits - 1 ;; )
    {
      if( ull_buf[j] != ull_buf[k] ) return false;
      if( (j == k) || (j + 1 == k) ) break;
      j++;
      k--;
    }
    return true;
  }
  
  const char* digits() { return (const char*) ull_buf; }  

  int first_digit()
  {
    unsigned char i = 0;
    while( ull_buf[i] == 32 ) i++;
    return ull_buf[i] - 48;
  }

  int last_digit()
  {
    return ull_buf[ ull_digits - 1 ] - 48;
  }

};


struct PrimeClock
{
  ull m,      // current number being tested,
      k_max,  // factors_to_check,
      k,      // factors_checked,
      last_prime, 
      primes_found,
      twin_primes_found,
      palindromic_primes_found,
      first_digit_hist[ 10 ],  // can't use floats/doubles because of precision issues
      last_digit_hist[ 10 ];
      
  
  Digits last_prime_digits;

  PrimeClock()
  {
    m = 1;
    k = 0;
    k_max = 1;
    last_prime = 1;
    primes_found = 0;
    twin_primes_found = 0;
    palindromic_primes_found = 0;
    for( int i = 0; i < 10; i++ )
    {
      first_digit_hist[ i ] = 0;
      last_digit_hist[ i ] = 0;
    }
  }

  // https://en.wikipedia.org/wiki/Primality_test
  // Implemented as a member function so that we can set the
  // number of factors to check, and current factor as member
  // variables. This allows us to show the internal progress
  // of the prime test if we pause the function 
  // (e.g. via an interrupt)
  bool is_prime()
  {
      if( m == 2 | m == 3 ) return true;
  
      if( m % 2 == 0 ) return false;
      if( m % 3 == 0 ) return false;
      
      ull k6;
          
      k_max = (sqrt(m) + 1) / 6;
      for( k = 1; k <= k_max; k++ )  // divisible by 6*k +/- 1 ?
      {
        k6 = 6 * k;
        if( m % (k6 - 1) == 0 ) return false;
        if( m % (k6 + 1) == 0 ) return false;      
      }
  
      return true;
  }


  // Increment the clock and test if this next number is prime
  bool check_next()
  {
    m++;
    if( is_prime() )
    {
      found_prime();
      return true;
    }
    return false;
  }
  
  void found_prime()
  {
    primes_found++;
    if( m - last_prime == 2 ) twin_primes_found++;
    last_prime = m;
    
    last_prime_digits.set( m );    
    if( last_prime_digits.is_palindrome() ) palindromic_primes_found++;
    first_digit_hist[ last_prime_digits.first_digit() ]++;
    last_digit_hist[ last_prime_digits.last_digit() ]++;    
  }

};

}

#endif // _PRIMES_H_
