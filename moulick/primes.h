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


  // buf should have size p_max_d + 1 and end in '\0'
  inline char* prime_t_to_str( prime_t m, char *buf )
  {
    char *m_ptr = buf + p_max_d;
    for(int i = 0 ; i < p_max_d ; i++)
    {
      m_ptr--;
      *m_ptr = (char) (m % 10 + 48);  // Convert remainder to ASCII for 0, 1, 2....
      m /= 10;
      if( m == 0 ) break;
    }
    // m_ptr now points to start of the converted number
    return m_ptr;
  }


  // Good enough code for finding the square root of m
  // We only need to check divisors up to this
  inline prime_t sqrt( prime_t m )
  {
      float sqrt_m = m;
      for( int i = 0; i < 6; i++ )
          sqrt_m = (sqrt_m * sqrt_m + m) / (2 * sqrt_m);
      return (prime_t) sqrt_m;
  }

  // Given a string, check if it is a palindrome
  inline bool is_palindrome( const char *buf )
  {
    for( int i = 0, j = strlen(buf) - 1; i < strlen(buf); i++, j--)
    {
      if( buf[i] != buf[j] ) return false;
      if( (i == j) || (i + 1 == j) ) break;
    }
    return true;
  }

  inline int first_digit( const char *buf )
  {
    return buf[ 0 ] - 48;
  }

  inline int last_digit( const char *buf )
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
    volatile bool abrt;  // flag used by external interrupt to break our routine
                         // needs to be declared volatile, otherwise ISR won't be
                         // able to change the value the PrimeTester loop is seeing

    PrimeTester() { k = 0; k_max = 1; abrt = false; }

    // https://en.wikipedia.org/wiki/Primality_test
    // Implemented as a member function so that we can set the
    // number of factors to check, and current factor as member
    // variables. This allows us to show the internal progress
    // of the prime test if we pause the function 
    // (e.g. via an interrupt)
    bool is_prime( prime_t m )
    {
      abrt = false;
            
      k = 1;
      k_max = 1;
      
      if( m == 2 | m == 3 ) return true;
  
      if( m % 2 == 0 ) return false;
      if( m % 3 == 0 ) return false;
      
      prime_t k6;
          
      k_max = (sqrt(m) + 1) / 6;
      for( k = 1; k <= k_max; k++ )  // divisible by 6*k +/- 1 ?
      {
        if( abrt ) // Stop work and get out.
        {
          k = 0; // Hack to avoid printing a spurious bar
          return false;  
        }
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
    char m_string[ p_max_d + 1 ];  // m as a string
    char *m_ptr;  // pointer into m_string
    bool is_prime, is_twin_prime, is_palindromic_prime;

    void restart_clock_from( prime_t _m )
    {
      pt.abrt = true;
      m = _m;
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

    PrimeClock()
    {
      m_string[ p_max_d ] = '\0';      
      restart_clock_from( 1 );
    }

    void set_string_representation()
    {
      m_ptr = prime_t_to_str( m, m_string );
      // m_ptr now points to start of the converted number
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


        set_string_representation();        
        // Test palindrome
        if( is_palindrome( m_ptr ) )
        {
          palindromic_primes_found++;
          is_palindromic_prime = true;          
        } else {
          is_palindromic_prime = false;           
        }
        
        // Update histograms
        first_digit_hist[ first_digit( m_ptr ) - 1 ]++;
        last_digit_hist [ last_digit(  m_ptr ) - 1 ]++;    
      }
      else
        is_prime = false;  // The other flags don't matter then
    }

    // What fraction of the divisors have been tested?
    float fraction_tested() const
    {
      return ((float) pt.k - 1) / (float) pt.k_max;
    }

    const char *m_as_string() const
    {
      return (const char*) m_ptr;
    }
  };

}

#endif // _PRIMES_H_
