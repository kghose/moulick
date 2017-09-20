// Yes we have tests
// We ARE professional

// g++ primes_test.cpp -o pt

#include <iostream>
#include <cassert>
#include "moulick/primes.h"

using namespace primes;

void test_digits()
{
    PrimeClock pc;
    pc.m = 1234;
    pc.set_string_representation();    
    const char *buf = pc.m_as_string();

    assert( buf[ 0 ] == '1' );
    assert( buf[ 3 ] == '4' );
    assert( is_palindrome( buf ) == false); 
    assert( first_digit( buf ) == 1);
    assert( last_digit( buf ) == 4);
    
    pc.m = 12344321;
    pc.set_string_representation();
    buf = pc.m_as_string();
    assert( buf[ 0 ] == '1' );
    assert( buf[ 7 ] == '1' );
    assert( is_palindrome( buf ) == true); 
    assert( first_digit( buf ) == 1);
    assert( last_digit( buf ) == 1);

    pc.m = 12321;
    pc.set_string_representation();
    buf = pc.m_as_string();
    assert( is_palindrome( buf ) == true); 
        
    std::cout << "Digits test passed" << std::endl;
}


void test_primes()
{
    // http://www.primos.mat.br/indexen.html
    int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};
    int n_primes = 168;

    // https://primes.utm.edu/lists/small/1ktwins.txt
    int twin_primes[] = {3,  5, 11, 17, 29, 41, 59, 71, 101, 107, 137, 149, 179, 191, 197, 
        227, 239, 269, 281, 311, 347, 419, 431, 461, 521, 569, 599, 617, 641, 659, 809, 
        821, 827, 857, 881};
    int n_twin_primes = 35; 

    // https://oeis.org/A002385
    int palindromic_primes[] = {2, 3, 5, 7, 11, 101, 131, 151, 181, 191, 313, 353, 373, 383, 727, 757, 787, 797, 919, 929};  
    int n_palindromes = 20;

    PrimeClock pc;
    for(int i = 0; i < 1000; i++)
    {
        pc.check_next();
        if( pc.is_prime )
        {
            assert( primes[ pc.primes_found - 1 ] == pc.last_prime );
            if( pc.is_twin_prime )
            {
                assert( twin_primes[ pc.twin_primes_found - 1 ] == pc.last_prime - 2 );
            }
            if( pc.is_palindromic_prime )
            {
                assert( palindromic_primes[ pc.palindromic_primes_found - 1 ] == pc.last_prime );
            }            
        }
    }
    assert( pc.twin_primes_found == n_twin_primes );
    assert( pc.palindromic_primes_found == n_palindromes );

    std::cout << "Primes test passed" << std::endl;
}


void test_large_primes()
{
    PrimeTester pt;

    /*
        https://primes.utm.edu/lists/small/millions/
        "... downloading primes is a better use of bandwidth then much of the downloading done on the Internet."
        - Chris K. Caldwell

        Thank's Chris!
    */
    uint32_t large_prime[] = { 15485863, 86028121, 472882027, 715225739, 982451653, (1L << 31) - 1L /* Euler */};
    for( int i = 0; i < 6; i++ )
        assert( pt.is_prime( large_prime[ i ]) );

    // 715225739 + 2 is a prime
    uint32_t large_composites[] = { 15485863 - 4, 86028121 - 2, 472882027 - 6, 715225739 + 4, 982451653 - 2, (1L << 31) + 3L };
    for( int i = 0; i < 6; i++ )
        assert( pt.is_prime( large_composites[ i ]) == false );

    std::cout << "Large primes test passed" << std::endl;
}

int main()
{
    test_digits();
    test_primes();
    test_large_primes(); 
}