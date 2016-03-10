#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions

DigitalOut red(LED_RED);             // we create a variable 'red', use it as an out port
DigitalOut green(LED_GREEN);         // we create a variable 'green', use it as an out port

InterruptIn btn2(SW2);               // we create a variable 'btn2', use it as an in port
InterruptIn btn3(SW3);               // we create a variable 'btn3', use it as an in port

// YOUR CODE HERE

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    green = red = 1; // turn off green and red on startup (1=off, I know it's weird)
    
    btn2.fall(toggle_red);
    btn3.fall(toggle_green);
}
