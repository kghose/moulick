Moulick (Bengali - মৌলিক - for "prime") is a mathematical toy that uses an Arduino to continually compute 
and display prime numbers.

[Video demo](https://youtu.be/VL-EnUDCtP4)


## Coronal display
[![Moulick coronal display - click for video](media/coronal-display.jpeg)](https://youtu.be/VL-EnUDCtP4)
When you first switch it on Moulick will start computing primes from zero, showing the last found prime in the center
of the circle, while showing details of the most recent computations on the clock face (which I also call the corona
display because it reminds me of [this][solar-corona]). 

The circle represents 250 numbers and the length of the coronal lines emanating from it represents the fraction of divisors that had to be testing before deciding if that number was prime or not. Thus the longest lines (where all the divisors had to be tested) are primes. They are marked in red. Primes that are palindromic are marked in cyan. Primes that are the second of a pair of twin primes are marked in blue.  

[solar-corona]: https://en.wikipedia.org/wiki/Corona#/media/File:Solar_eclipse_1999_4_NR.jpg

Use a stylus to tap on the left half of the screen. You may have to hold down the stylus for upto a quarter second. This will switch the display from the corona to the stats page. Tapping the same corner again will go back to the coronal display.

## Stats display
![alt text](media/stats-display.jpg)
The stats display shows how many primes, twin primes and palindromic primes have been found. The cryptic looking matrix on the right shows the last digit transition frequencies of consecutive primes. Say a prime ends in 1 and the following prime ends in 9. This adds a count to the (1, 9) (top right hand corner) box. If a prime ends in 9 and the following one ends in 9 it adds to the count in the (9, 9) (bottom right hand corner) box, and so on. If prime numbers are random then the matrix should glow an even grayish color, indicating that all transitions are equally probable. Watch the stats display for a while. Does it glow an even color? If you are curious, read the following [popular science account](rloks) (and this [paper](rloks-paper) - which is a little dense in mathematical notation).

## Resetting the clock
If you tap on the right hand half of the display you will reset the clock to a different starting point and it will continue to find primes from there. Spend some time tapping at different points on the screen. Is there a pattern to where the clock restarts from when you tap?

## Other Fun things to notice
The line indicating a prime that is the second of a pair of twin primes will be colored differently from regular primes. A prime that is also a palindrome has it's own color while A twin prime that is also a palindrome has yet another color. The height of the little green lines indicate what fraction of divisors had to be checked before deciding whether a number is composite or prime (hence all the prime numbers have the longest bars). Some numbers - like squares of primes will have long bars. If the number is a large prime, testing all the factors will take appreciable time and you can see the green bar growing as the computation progresses.


Hardware
--------
Arduino UNO + 2.8" TFT touchscreen display shield.

* Note: The code uses an Elegoo 2.8" display. This can be readily adapted to Arduino's more official
TFT touchscreens by changing the header includes in `display.h`, and `touch.h` and probably by changing
some values in `tftconstants.h`.

Software
--------
All the Arduino code required to create this clock is in this repository.

### Code organization

- [moulick.ino](moulick/moulick.ino) - the main "sketch" (entry point)
- [moulickapp.h](moulick/moulickapp.h) / [.cpp](moulick/moulickapp.cpp) - application code that ties components together
- [primes.h](moulick/primes.h) - computes primality
- [display.h](moulick/display.h) / [.cpp](moulick/display.cpp) - handles drawing on the display
- [touch.h](moulick/touch.h) - handles touch screen interaction
- [tftconstants.h](moulick/tftconstants.h) - hardware constants gathered together

Miscellaneous code

- [primes_test.cpp](primes_test.h) - test suite for core prime finding algorithms
- [fractional-bresenham.ipynb](fractional-bresenham.ipynb) - educational notebook about Bresenham's line algorithm and a modification I made to it


Blogposts
---------
- [Main post introducing the project][main]
- [Debouncing a touch screen][debounce]  ([Touchscreen oscilloscope app][osc])
- [Why I needed `volatile`][volatile]
- [A slight modification of Bresenham's algorithm][bresenham] ([IPython notebook](fractional-bresenham.ipynb))

[main]: https://kaushikghose.wordpress.com/2017/09/27/%E0%A6%AE%E0%A7%87%E0%A7%97%E0%A6%B2%E0%A6%BF%E0%A6%95-a-prime-number-toy/
[debounce]: https://kaushikghose.wordpress.com/2017/09/21/debouncing-a-touch-screen/
[osc]: https://gist.github.com/kghose/0434bfa77d47ddabc4418efa3eee2a31
[volatile]: https://kaushikghose.wordpress.com/2017/09/21/volatile/
[bresenham]: https://kaushikghose.wordpress.com/2017/09/25/a-modification-to-bresenhams-algorithm/


References
----------
- [Arduino timer interrupts][timer]
- [Last digit transition asymmetry][rloks] ([Original paper][rloks-paper] - the part I understood was the table, the rest is introduced with number theory formalism that I'm not trained in.)

[timer]: https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
[rloks]: https://www.nature.com/news/peculiar-pattern-found-in-random-prime-numbers-1.19550
[rloks-paper]: https://arxiv.org/pdf/1603.03720.pdf