#include "xil_printf.h"

#define DELAY_UNIT 80

#define LEDS (*(unsigned volatile *)0x40000000)
#define SW (*(unsigned volatile *)0x40000008)
#define JB (*(unsigned volatile *)0x40010000)

// New GPIO ports for seven-segment display and buttons
#define SEG_PORT (*(unsigned volatile *)0x40020000) // 8-bit port for segments
#define ANO_PORT (*(unsigned volatile *)0x40020008)
#define BUTTONS (*(unsigned volatile *)0x40030000)

void delay_ms(unsigned t) {
    unsigned cntr1, cntr2;
    while (t--)
        for (cntr1 = 0; cntr1 < 100; cntr1++) {
            for (cntr2 = 0; cntr2 < DELAY_UNIT; cntr2++) {}
        }
}

// FSM for button press detection
_Bool up_button_press() {
    static _Bool prev_state = 0;
    _Bool current_state = (BUTTONS & (1 << 0)); // Check up button (bit 0)
    _Bool result = (current_state && !prev_state); // Detect rising edge
    prev_state = current_state;
    return result;
}

_Bool down_button_press() {
    static _Bool prev_state = 0;
    _Bool current_state = (BUTTONS & (1 << 1)); // Check down button (bit 1)
    _Bool result = (current_state && !prev_state);
    prev_state = current_state;
    return result;
}

_Bool left_button_press() {
    static _Bool prev_state = 0;
    _Bool current_state = (BUTTONS & (1 << 2)); // Check left button (bit 2)
    _Bool result = (current_state && !prev_state);
    prev_state = current_state;
    return result;
}

_Bool right_button_press() {
    static _Bool prev_state = 0;
    _Bool current_state = (BUTTONS & (1 << 3)); // Check right button (bit 3)
    _Bool result = (current_state && !prev_state);
    prev_state = current_state;
    return result;
}

// FSM to flash LED[0] at 1Hz with variable duty cycle
void flash_LED(int duty_cycle) {
    static int counter = 0;
    if (counter < duty_cycle) {
        LEDS |= (1 << 0); // Turn on LED[0]
    } else {
        LEDS &= ~(1 << 0); // Turn off LED[0]
    }
    counter = (counter + 1) % 1000; // Reset counter after 1000 cycles
}

int main() {

    int duty_cycle = 500; // Initial duty cycle (50%)

    print("\nWorking fine");

    while (1) {
        delay_ms(1);


        // New functionality for current lab
        if (up_button_press()) {
            duty_cycle += 100;
            if (duty_cycle > 1000) duty_cycle = 1000; // Limit to 100%
        }
        if (down_button_press()) {
            duty_cycle -= 100;
            if (duty_cycle < 0) duty_cycle = 0; // Limit to 0%
        }

        // Flash LED[0] with updated duty cycle
        flash_LED(duty_cycle);

        // Output SW[15:8] to segment/DP port when left button is pressed
        if (left_button_press()) {
            SEG_PORT = (SW >> 8) & 0xFF;
        }

        // Output SW[3:0] to anodes when right button is pressed
        if (right_button_press()) {
            ANO_PORT = SW;
        }
    }
}
