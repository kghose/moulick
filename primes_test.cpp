// Yes we have tests
// We ARE professional

// g++ primes_test.cpp -o pt

#include <iostream>
#include <cassert>
#include "moulick/primes.h"

using namespace primes;

void test_digits()
{
    Digits D;

    D.set( 1234 );
    assert( D.digits()[19] == '4' );
    assert( D.digits()[16] == '1' );
    assert( D.is_palindrome() == false); 
    assert( D.first_digit() == '1');
    assert( D.last_digit() == '4');
    
    D.set( 12344321 );
    assert( D.digits()[19] == '1' );
    assert( D.digits()[15] == '4' );
    assert( D.is_palindrome() == true); 
    assert( D.first_digit() == '1');
    assert( D.last_digit() == '1');

    D.set( 12321 );
    assert( D.is_palindrome() == true); 
        
    std::cout << "Digits test passed" << std::endl;
}


void test_primes()
{
    PrimeClock pc;
    assert( pc.check_next() == true );  // 2
    assert( pc.last_prime == 2 );
    assert( pc.check_next() == true );  // 3
    assert( pc.last_prime == 3 );

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

    PrimeClock son_of_pc;
    for(int i = 0; i < 1000; i++)
    {
        son_of_pc.check_next();
        // std::cout << son_of_pc.primes_found << " = " << son_of_pc.last_prime << std::endl;
        assert( primes[ son_of_pc.primes_found - 1 ] == son_of_pc.last_prime );
    }
    assert( son_of_pc.twin_primes_found == n_twin_primes );
    assert( son_of_pc.palindromic_primes_found == n_palindromes );
    
    std::cout << "Primes test passed" << std::endl;
}

int main()
{
    test_digits();
    test_primes();
}