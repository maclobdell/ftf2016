#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions

using namespace minar;

DigitalOut red(LED_RED);             // we create a variable 'red', use it as an out port

// YOUR CODE HERE

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    // we want to run the function 'blinky', every 500 ms.
    Scheduler::postCallback(blinky).period(milliseconds(500));
}
