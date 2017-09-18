/*
  Handles the generation of primes and tracks some metrics
*/
#ifndef _PRIMES_H_
#define _PRIMES_H_

namespace primes {

  /*
    I debated whether to make this uint32_t (which takes us to four billion)
    or uint64_t (which take us to eighteen quintillion).
    
    Rough measurements show the hardware I used (Uno + TFT display) tests 250
    numbers in 9s (or ~ 1600 numbers a minuite). This was when testing numbers
    below one million, and is expected to get slower as we go to larger numbers.
    
    Conservatively, four billion primes would take more than 5 years to 
    compute sequentially and eighteen quintillion would take a tad longer.

    I went through a period of mania where I imagined Moulick becoming a cult
    phenomenon and being buried in a time capsule with a magic battery being
    dug up by the descendants of man, still ticking. Then I decided to stick
    with uint32_t. If you want, though, you can uncomment the relevant lines
    though ...

    Also, we can start the clock anywhere ...  

    Interestingly, I did not measure any difference in performance between using
    uint32_t and uint64_t, which was strange. This being a 8 bit microcontroller
    uint64_t should take twice as long as uint32_t computations. 
  */
  // typedef unsigned long long prime_t;  // prime number type
  // const unsigned char p_max_d = 20;  // log10(2^64 -1)
  typedef uint32_t prime_t;  // prime number type
  const unsigned char p_max_d = 10;  // log10(2^32 -1)

  // Good enough code for finding the square root of m
  // We only need to check divisors up to this
  prime_t sqrt( prime_t m )
  {
      float sqrt_m = m;
      for( int i = 0; i < 6; i++ )
          sqrt_m = (sqrt_m * sqrt_m + m) / (2 * sqrt_m);
      return (prime_t) sqrt_m;
  }

  /*
    buf should have size p_max_d + 1
    Some or all of buf will be over-written
    String will be terminated by null '\0'
  */
  char* to_string( prime_t m, char *buf )
  {
    char tmp_buf[ p_max_d ];
    char *s = tmp_buf + p_max_d - 1;
    
    // First convert 'm' into a string
    // This is right justified
    for(int i = 0 ; i < p_max_d ; i++)
    {
      s--;
      if( m ) {
        *s = (char) (m % 10 + 48);  // Convert remainder to ASCII for 0, 1, 2....
        m /= 10;        
      } else {
        *s = 32; // unless m == 0 in which case print a blank
      }
    }

    // Now copy the string onto buf but left justified
    int i = 0, j = 0;
    for( ; i <= p_max_d; i++ )
    if( tmp_buf[ i ] != 32)
      buf[ j++ ] = tmp_buf[ i ];
      buf[ j ] = '\0';
    
    return buf;
  }


  // Given a string, check if it is a palindrome
  bool is_palindrome( const char *buf )
  {
    for( int i = 0, j = strlen(buf) - 1; i < strlen(buf); i++, j--)
    {
      if( buf[i] != buf[j] ) return false;
      if( (i == j) || (i + 1 == j) ) break;
    }
    return true;
  }

  int first_digit( const char *buf )
  {
    return buf[ 0 ] - 48;
  }

  int last_digit( const char *buf )
  {
    return buf[ strlen( buf ) - 1 ] - 48;
  }


  /*
    We structure the prime tester as a struct and expose
    some internal variables so that we can interrupt it's 
    operation and peek into how many factors have been
    tested if we so wish
  */
  struct PrimeTester
  {
    prime_t k_max,  // number of factors to check in total
            k;      // number of factors already checked

    PrimeTester() { k = 0; k_max = 1; }

    // https://en.wikipedia.org/wiki/Primality_test
    // Implemented as a member function so that we can set the
    // number of factors to check, and current factor as member
    // variables. This allows us to show the internal progress
    // of the prime test if we pause the function 
    // (e.g. via an interrupt)
    bool is_prime( prime_t m )
    {
        k = 1;
        k_max = 1;
        
        if( m == 2 | m == 3 ) return true;
    
        if( m % 2 == 0 ) return false;
        if( m % 3 == 0 ) return false;
        
        prime_t k6;
            
        k_max = (sqrt(m) + 1) / 6;
        for( k = 1; k <= k_max; k++ )  // divisible by 6*k +/- 1 ?
        {
          k6 = 6 * k;
          if( m % (k6 - 1) == 0 ) return false;
          if( m % (k6 + 1) == 0 ) return false;      
        }
    
        return true;
    }
  };

  struct PrimeClock
  {
    prime_t m,            // current number being tested,
            last_prime,   // most recent prime found
            primes_found, 
            twin_primes_found,
            palindromic_primes_found,
            first_digit_hist[ 9 ],  // can't use floats/doubles because of precision issues
            last_digit_hist[ 9 ];         
    PrimeTester pt;
    char *m_string;  // m as a string
    bool is_prime, is_twin_prime, is_palindromic_prime;

    PrimeClock()
    {
      m_string = new char[ p_max_d + 1 ];
      m = 1;
      last_prime = 1;
      primes_found = 0;
      twin_primes_found = 0;
      palindromic_primes_found = 0;
      for( int i = 0; i < 9; i++ )
      {
        first_digit_hist[ i ] = 0;
        last_digit_hist[ i ] = 0;
      }
    }

    ~PrimeClock()
    {
      delete[] m_string;  // Because we are neat
    }

    // Increment the clock and test if this next number is prime
    void check_next()
    {
      m++;
      if( pt.is_prime( m ) )
      {
        is_prime = true;
        primes_found++;

        // Test twin primes
        if( m - last_prime == 2 )
        {
          twin_primes_found++;
          is_twin_prime = true;        
        } else {
          is_twin_prime = false;
        }
        last_prime = m;


        m_string = to_string( m, m_string );        
        // Test palindrome
        if( is_palindrome( m_string ) )
        {
          palindromic_primes_found++;
          is_palindromic_prime = true;          
        } else {
          is_palindromic_prime = false;           
        }
        
        // Update histograms
        first_digit_hist[ first_digit( m_string ) - 1 ]++;
        last_digit_hist [ last_digit(  m_string ) - 1 ]++;    
      }
      else
        is_prime = false;  // The other flags don't matter then
    }

  };

}

#endif // _PRIMES_H_
