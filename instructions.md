# SX Create: Making music on a microcontroller, instructions

Welcome to our session at SX Create! If you have any questions, please just give a shout out to Austin, Jan or Gregory, we're here to help everyone make the most of today.

In this session we'll work through eigth examples together. Jan will demonstrate everything, but this document should help you in case you're stuck or did not completely follow along. If you feel you're way faster than everyone else, we have some extra tasks at the bottom of each example. Please do not get ahead, the later examples all do something with music, and that will be very annoying for the other participants. :-)

## Setup

1. Connect the FRDM-K64F board to your computer. There's 2 micro-USB ports on the board, use the OpenSDA one!
1. If you're on Windows, please install the [serial driver](https://developer.mbed.org/handbook/Windows-serial-configuration). On OS/X and Linux, no action required.
1. The board mounts as a mass-storage device (like a USB drive). Verify that you can see it (drive name will be MBED).
1. Go to http://c9.io/auth/mbed, and sign up for an account.
1. After creating the account you will be at the dashboard. Click 'Create new workspace'.
1. Choose a workspace name (f.e. 'sxsw'), and choose 'Public' as the workspace type.
1. Under 'Clone from...' enter: https://github.com/janjongboom/sxsw.git

An IDE should open. Congratulations!

**Optional:** We can talk to the board via a serial port, but you might need some software. Read [this doc](https://developer.mbed.org/handbook/SerialPC#host-interface-and-terminal-applications) and install required software (like PuTTY on Windows).

## 1. Blinky

1. First we need to set up our project and our target. In the button next to 'Build' in Cloud9, select '1_blinky'.
1. On the button next to that, click 'Search in registry' and select 'frdm-k64f-gcc'.
1. Open sxsw/1_blinky/source/main.cpp in the tree.
1. We need to implement a function that will blink the LED.
1. Under '// YOUR CODE HERE' add the following code

```cpp
static void blinky() {
    // we inverse the value of 'red', toggling the LED every time this function is called
    red = !red;
}
```

1. Now press 'Build' (in the top bar)
1. A file downloads (blinky.bin)
1. Drag the file to the 'MBED' disk
1. The board will flash (green). When it stops, hit the 'Reset' button.
1. Blinky runs!

**Optional:** We use [minar](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/Full_Guide/MINAR/) as our event scheduler. We use it to periodically run the blinky function, but we can also use it to delay execution. E.g.:

```cpp
Scheduler::postCallback(blinky).delay(milliseconds(500));
```

Will call `blinky` once, in 500 ms. But does not repeat the process. Rewrite the app to use `delay` instead of `period`, but let it still blink the LED. Afterwards see if you can vary the time between blinks ([hint](http://www.cplusplus.com/reference/cstdlib/rand/)).

## 2. Two buttons, two colors

1. Switch projects, click `1_blinky`, and change to `2_two_buttons`
1. Open sxsw/2_two_buttons/source/main.cpp
1. We want to execute a function when the button is clicked
1. We already have code in place which responds when the buttons 'fall', which means the button is pressed
1. Let's write similar code as before, but now with two functions (one for green, one for red)
1. Under '// YOUR CODE HERE' add the following code

```cpp
static void toggle_red() {
    red = !red;
}
static void toggle_green() {
    green = !green;
}
```

1. Hit the Build button and flash the application. Clicking SW2 and SW3 will now toggle the LEDs. Having them both on will also allow you to create yellow.

**Optional:** We use `fall` currently, which triggers when the button is clicked. We can also use `rise` to get an event when the button is no longer clicked. Change the code so that the LEDs only burn when the button is pressed down.
