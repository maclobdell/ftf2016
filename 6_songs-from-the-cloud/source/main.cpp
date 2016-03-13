#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions
#include "tones.h"                   // list of all the tones and their frequencies
#include "security.h" // get this from https://connector.mbed.com/#credentials
#ifdef MBED_DOMAIN
#include "mbed-client-ethernet-c-style/client.h"

using namespace minar;
using namespace mbed::util;

DigitalOut green(LED_GREEN);

PwmOut buzzer(D3);                   // our buzzer is a PWM output (pulse-width modulation)

static int BPM = 35;

void registered() {
    printf("Connected to mbed Device Connector!\r\n");
    green = !green;
}

// this is our function that plays a tone. 
// Takes in a tone frequency, and after duration (in ms.) we stop playing again
static void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
        buzzer = 0.1f;
        wait_us(tone);
        buzzer = 0.0f;
        wait_us(tone);
    }
}

static void play_song(std::vector<int>* melody, std::vector<int>* duration) {
    for (int i = 0; i < melody->size(); i++) {
        int tone = melody->at(0);
        // BPM is quarter notes per minute, so length in milliseconds is:
        int length = static_cast<int>(static_cast<float>(1000 / duration->at(0)) * (60000.0f / static_cast<float>(BPM * 1000)));
        
        // printf("tone %d, length %d, duration %d\r\n", tone, length, duration->at(0));
        
        if (melody->at(i) != 0) {
            playTone(melody->at(i), length);
        }
        else {
            buzzer = 0.0f;
            wait_ms(length);
        }
    }
}

static void play_song_cloud(void*) {
    std::stringstream notesPattern(mbed_client_get("buzzer/notes"));
    std::vector<int>* notes = new std::vector<int>;
    {
        std::string item;
        while (std::getline(notesPattern, item, ':')) {
            notes->push_back(atoi((const char*)item.c_str()));
        }
    }
    
    std::stringstream durationPattern(mbed_client_get("buzzer/duration"));
    std::vector<int>* durations = new std::vector<int>;
    {
        std::string item;
        while (std::getline(durationPattern, item, ':')) {
            durations->push_back(atoi((const char*)item.c_str()));
        }
    }
    
    if (notes->size() != durations->size()) {
        printf("Notes and duration have different sizes (%d vs %d), abort!\r\n", notes->size(), durations->size());
        return;
    }
    
    play_song(notes, durations);
}

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    green = 1; // turn green off
    
    mbed_client_define_resource("buzzer/notes", string("262:277"), M2MBase::GET_PUT_ALLOWED, true);
    mbed_client_define_resource("buzzer/duration", string("4:4"), M2MBase::GET_PUT_ALLOWED, true);
    mbed_client_define_function("buzzer/play", &play_song_cloud);
    
    struct MbedClientOptions opts = mbed_client_get_default_options();
    opts.DeviceType = "instrument";
    
    bool setup = mbed_client_setup(opts);
    if (!setup) {
        printf("Setup failed (e.g. couldn't get IP address. Check serial output.\r\n");
        return;
    }
    mbed_client_on_registered(&registered);
}

#endif
