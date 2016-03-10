#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions
#include "tones.h"                   // list of all the tones and their frequencies

using namespace minar;

InterruptIn btn2(SW2);               // we create a variable 'btn2', use it as an in port
InterruptIn btn3(SW3);               // we create a variable 'btn3', use it as an in port

PwmOut buzzer(D3);                   // our buzzer is a PWM output (pulse-width modulation)

static void silence() {
    buzzer.write(0.0f); // silence!
}

// this is our function that plays a tone. 
// Takes in a tone frequency, and after duration (in ms.) we stop playing again
static void play_tone(int tone, int duration) {
    buzzer.period_us(tone);
    buzzer.write(0.10f); // 20% duty cycle, otherwise it's too loud
    
    // we wait for duration ms. and then call the silence function
    Scheduler::postCallback(silence).delay(milliseconds(duration));
}

// YOUR CODE HERE

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    btn2.fall(play_note1);
    btn3.fall(play_note2);
}
