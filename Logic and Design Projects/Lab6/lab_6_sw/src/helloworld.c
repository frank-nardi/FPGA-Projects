#include "xil_printf.h"

#define DELAY_UNIT 81
#define JXADC1_CH1 (*(unsigned volatile *)0x44a10258)
#define JB (*(unsigned volatile*)0x40010000)

void delay_ms(unsigned t) {
    unsigned cntr1, cntr2;
    while (t--)
        for (cntr1 = 0; cntr1 < 100; cntr1++) {
            for (cntr2 = 0; cntr2 < DELAY_UNIT; cntr2++) {}
        }
}

unsigned read_adc() {
    unsigned temp = 0;
    for (unsigned i = 0; i < 64; i++) {
        temp += JXADC1_CH1;
    }
    return temp >> 6;
}

int main() {
    unsigned threshold = 0;
    unsigned prev_ADC = 0, current_ADC;
    unsigned prev_cnt = 0, cnt = 0;
    unsigned frequency = 0;
    unsigned rising_edge_detected = 0;
    unsigned test = 0;

    print("Sine Wave Period Estimated\n");

    for (unsigned i = 0; i < 100; i++) {
        threshold += read_adc();
        delay_ms(1);
    }
    threshold /= 100;

    xil_printf("Initial threshold: %d\n", threshold);

    while (1) {
        delay_ms(1);
        current_ADC = read_adc();

        test = (threshold * 99 + current_ADC) / 100;

        if (prev_ADC < test && current_ADC >= test) {
            if (rising_edge_detected) {
                unsigned period = cnt - prev_cnt;
                frequency = 1000 / period;
                prev_cnt = cnt;
            } else {
                rising_edge_detected = 1;
                prev_cnt = cnt;
            }
        }

        prev_ADC = current_ADC;
        cnt++;

        if (cnt % 1000 == 0 && rising_edge_detected) {
            xil_printf("Frequency: %d Hz\n", frequency);
        }
    }

    	if ((((JXADC1_CH1 >> 4) * 244) / 1000) > threshold)
    	{
    		JB = 0xFFFF;
       	}
    	else
    	{
    		JB = 0x0000;
    	}

    return 0;
}
