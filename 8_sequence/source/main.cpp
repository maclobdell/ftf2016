#include "mbed-drivers/mbed.h"       // this tells us to load mbed OS related functions
#include "tones.h"                   // list of all the tones and their frequencies
#include "fxos8700cq/fxos8700cq.h"   // library for the accelerometer
#include "mbed-drivers/rtc_time.h"   // read the value of the realtime clock (for timing)
#include "mbed-hal/us_ticker_api.h"  // ticker value
#include "security.h" // get this from https://connector.mbed.com/#credentials
#ifdef MBED_DOMAIN
#include "mbed-client-ethernet-c-style/client.h"

using namespace minar;

DigitalOut red(LED_RED);
DigitalOut green(LED_GREEN);
DigitalOut blue(LED_BLUE);

InterruptIn btn2(SW2);               // we create a variable 'btn2', use it as an in port
InterruptIn btn3(SW3);               // we create a variable 'btn3', use it as an in port

PwmOut buzzer(D3);                   // our buzzer is a PWM output (pulse-width modulation)

// Set up the accelerometer (this is specific to the onboard one)
InterruptIn accel_interrupt_pin(PTC13);
FXOS8700CQ accel(PTE25, PTE24, FXOS8700CQ_SLAVE_ADDR1);

static unsigned long start_time;
static unsigned long last_event;
static void countdown(int);
static std::vector<std::string>* sequence = new std::vector<std::string>();
static bool recording = false;

void registered() {
    printf("Connected to mbed Device Connector!\r\n");
    green = !green;
    
    countdown(6);
}

static void silence() {
    buzzer.write(0.0f); // silence!
}

// this is our function that plays a tone. 
// Takes in a tone frequency, and after duration (in ms.) we stop playing again
static void play_tone(int tone) {
    buzzer.period_us(tone);
    buzzer.write(0.10f); // 10% duty cycle, otherwise it's too loud
}

// when done, we blink blue
static void blink_blue() {
    blue = !blue;
}

static void check_no_change() {
    if (!recording) return;
    
    if (last_event != 0 && (us_ticker_read() / 1000L) - last_event > 5000) {
        printf("No change for 5s, updating\r\n");
        
        // YOUR CODE HERE (2)
    }
}

// YOUR CODE HERE (1)

static void play_note1() {
    if (!recording) return;
    
    save_event(1);
    
    play_tone(NOTE_C4);
}
static void play_note2() {
    if (!recording) return;

    save_event(2);
    
    play_tone(NOTE_D4);
}
static void play_note3() {
    if (!recording) return;

    save_event(3);
    
    play_tone(NOTE_E4);
    Scheduler::postCallback(silence).delay(milliseconds(200));
}

void start_recording() {
    recording = true;
    
    // play note when we fall
    btn2.fall(play_note1);
    btn3.fall(play_note2);
    
    // silence when we rise
    btn2.rise(silence);
    btn3.rise(silence);
    
    // accelerometer setup
    accel_interrupt_pin.fall(play_note3);
    accel_interrupt_pin.mode(PullUp);
    
    accel.config_int();      // enabled interrupts from accelerometer
    accel.config_feature();  // turn on motion detection
    accel.enable();          // enable accelerometer
    
    start_time = us_ticker_read() / 1000L;
    
    Scheduler::postCallback(check_no_change).period(milliseconds(200));
}

static void countdown(int times_left) {
    if (times_left != 0) {
        red = !red;
        
        Scheduler::postCallback(FunctionPointer1<void, int>(countdown).bind(times_left - 1)).delay(milliseconds(1000));
    }
    else {
        start_recording();
    }
}

// this code runs when the microcontroller starts up
void app_start(int, char**) {
    green = red = blue = 1; // turn all LEDs off
    
    mbed_client_define_resource("buzzer/recorded", string(""), M2MBase::GET_ALLOWED, true);
    
    struct MbedClientOptions opts = mbed_client_get_default_options();
    opts.DeviceType = "instrument";
    
    bool setup = mbed_client_setup(opts);
    if (!setup) {
        printf("Setup failed (e.g. couldn't get IP address. Check serial output.\r\n");
        return;
    }
    mbed_client_on_registered(&registered);
    
    buzzer = 0.0f;
}

#endif
