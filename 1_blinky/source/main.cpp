#include "mbed.h"  // this tells us to load mbed related functions

DigitalOut red(LED_RED);             // we create a variable 'red', use it as an out port
Ticker flipper;   //Ticker = recurring interrupt to repeatedly call a function at a specified rate

void flip() {
    red = !red;
}

// YOUR CODE HERE

// this code runs when the microcontroller starts up 
int main() {
    red = 1;  //turn the led off, (1=off, I know it's weird)
    
    // we want to blink an led, every 500 ms.
    flipper.attach(&flip, 0.5); // the address of the function to be attached (flip) and the interval (in seconds)
 
    // spin in a main loop. flipper will interrupt it to call flip
    while(1) {}
}
