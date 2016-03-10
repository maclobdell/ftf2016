#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions
#include "tones.h"                   // list of all the tones and their frequencies

using namespace minar;
using namespace mbed::util;

PwmOut buzzer(D3);                   // our buzzer is a PWM output (pulse-width modulation)

static int BPM = 120;

static void silence() {
    buzzer.write(0.0f); // silence!
}

// this is our function that plays a tone. 
// Takes in a tone frequency, and after duration (in ms.) we stop playing again
static void play_tone(int tone) {
    buzzer.period_us(tone);
    buzzer.write(0.10f); // 10% duty cycle, otherwise it's too loud
}

static void play_song(int notes_left, int* melody, int* duration) {
    // YOUR CODE HERE
}

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    // declare a melody
    int melody[] = {
        NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, 
        NOTE_D4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, 
        NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, 
        NOTE_C4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4
    };

    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int duration[] = {
        4, 4, 4, 4, 4, 4, 2, 
        4, 4, 2, 4, 4, 2,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 2
    };
    
    // melody & duration are on the heap, need to get them on the stack
    int *m = new int[sizeof(melody) / sizeof(int)];
    memcpy(m, melody, sizeof(melody));
    int *d = new int[sizeof(duration) / sizeof(int)];
    memcpy(d, duration, sizeof(duration));
    
    if (sizeof(melody) != sizeof(duration)) {
        printf("Melody and duration do not have same number of elements! Aborting!\r\n");
        return;
    }
    
    play_song(sizeof(melody) / sizeof(int), m, d);
}
