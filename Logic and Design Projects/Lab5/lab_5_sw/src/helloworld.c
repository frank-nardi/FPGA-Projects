#include "xil_printf.h"
#include "stdint.h"

#define ONE_US 100
#define ONE_MS 100*1000

#define LEDS ( *(unsigned volatile *)0x40000000 )
#define SW   ( *(unsigned volatile *)0x40000008 )
#define JB   ( *(unsigned volatile *)0x40010000 )
#define SEGDP  ( *(unsigned volatile *)0x40020000 )
#define AN   ( *(unsigned volatile *)0x40020008 )
#define BTN  ( *(unsigned volatile *)0x40030000 )
#define ALARM_CNTR ( *(unsigned volatile *)0x44a00000 )
#define ALARM0   ( *(unsigned volatile *)0x44a00004 )
#define ALARM0_VALUE   ( *(unsigned volatile *)0x44a00008 )

void delay_ms2(uint32_t val){
	ALARM0_VALUE = ONE_MS * val;
	while ((ALARM0 && (1 << 0)) == 0){
	}
}

void delay_us(uint32_t val){
	ALARM0_VALUE = ONE_US * val;
	while ((ALARM0 && (1 << 0)) == 0){
	}
}

/*void delay_ms(unsigned t){
	unsigned cntr1, cntr2;
	while(t--)
		for (cntr1 = 0; cntr1 < 100; cntr1++){
			for (cntr2 = 0; cntr2 < DELAY_UNIT; cntr2++){}
		}
}*/

_Bool btn_pressed(){
	static enum {NOT_PRESSED, DEBOUNCE_PRESS, PRESSED, DEBOUNCED_RELEASE}
	state = NOT_PRESSED;
	static uint8_t cnt;
	_Bool retval = FALSE;

	_Bool B = BTN & (1<<3);

	switch(state){
		case NOT_PRESSED:
			if (B){
				cnt = 0;
				state = DEBOUNCE_PRESS;
			}
			break;
		case DEBOUNCE_PRESS:
			if (B && cnt < 5){
				cnt++;
			}else if (B && cnt >= 5){
				retval = TRUE;
				state = PRESSED;
			}else if (!B){
				state = NOT_PRESSED;
			}
			break;
		case PRESSED:
			if (!B){
				state = DEBOUNCED_RELEASE;
				cnt = 0;
			}
			break;
		case DEBOUNCED_RELEASE:
			if (!B && cnt < 5){
				cnt++;
			} else if (!B && cnt >= 5){
				state = NOT_PRESSED;
			}
			break;
	}
	return retval;
}

void seg_disp(uint8_t data [4], uint8_t cursor){
	static uint8_t digit = 0;
	static uint16_t cnt = 0;
	const uint8_t disp_lut[10] = {
			0b11000000, //0
			0b11111001, //1
			0b10100100, //2
			0b10110000, //3
			0b10011001, //4
			0b10010010, //5
			0b10000010, //6
			0b11111000, //7
			0b10000000, //8
			0b10011000, //9
	};

	//AN = (1 << cursor);
	SEGDP = disp_lut[data[digit]];
	AN = ~(1 << (3-digit));

	if (digit == 1) {
		SEGDP ^= (1 << 7);
	}

	// Loops Counter
	cnt++;
	if (cnt == 1000){
		cnt = 0;
	}

	if (cnt >= 500 && digit == cursor) {
		AN = 0b1111;
	} else {
		AN = ~(1 << digit);
	}

	digit = (digit + 1) % 4;

	/*xil_printf("CNT: %d\n", cnt);*/

}

void pwm_out(unsigned duty){
	static uint16_t state = 0;
	if (state < duty){
		LEDS |= (1 << 0);
		JB |= (1 << 0);
	} else {
		LEDS &= ~(1 << 0);
		JB &= ~(1 << 0);
	}
	if (++state == 1000) state = 0;
}

void generate_pulse(){
	JB |= (1 << 3);
	delay_us(10);
	JB &= ~(1 << 3);
}

_Bool left_button_press()
{
	static _Bool last_state = 0;
	_Bool current_state = (BTN & 0x04) >> 2; // Check if the button is pressed

	if (!last_state && current_state) { // Detect rising edge
	    last_state = current_state;
	    return 1;
	}

	last_state = current_state;
	return 0;
}

_Bool right_button_press()
{
	static _Bool last_state = 0;
	_Bool current_state = (BTN & 0x08) >> 3; // Check if the button is pressed

	if (!last_state && current_state) { // Detect rising edge
	    last_state = current_state;
	    return 1;
	}

	last_state = current_state;
	return 0;
}

_Bool up_button_press()
{
	static _Bool last_state = 0;
	_Bool current_state = (BTN & 0x01); // Check if the button is pressed

	if (!last_state && current_state) { // Detect rising edge
	    last_state = current_state;
	    return 1;
	}

	last_state = current_state;
	return 0;
}

_Bool down_button_press()
{
	static _Bool last_state = 0;
	_Bool current_state = (BTN & 0x02) >> 1; // Check if the button is pressed

	if (!last_state && current_state) { // Detect rising edge
	    last_state = current_state;
	    return 1;
	}

	last_state = current_state;
	return 0;
}


int main(){
	print("Lab5 launches\n\r");

	uint8_t cursor = 0;
	uint8_t data[4] = {0, 0, 0, 0};
	unsigned duty_cycle = 0;

	while(1){
		_Bool mode = SW & 0x01; //using switch[0]
		seg_disp(data, cursor);
		pwm_out(duty_cycle);

		if(mode){
			if (left_button_press() == 1){
				if (cursor <= 2)
					cursor = cursor + 1;
				else {
					cursor = 0;
				}
				xil_printf("Left Pressed, Cursor: %d\n", cursor);
			}
		}

		if(mode){
			if (right_button_press() == 1){
				if (cursor >= 1)
					cursor = cursor - 1;
				else {
					cursor = 3;
				}
				xil_printf("Right Pressed, Cursor: %d\n", cursor);
			}
		}

		if(mode){
			if (up_button_press() == 1){
				if (data[cursor] < 9) data[cursor]++;
			}
		}
		if(mode){
			if (down_button_press()){
				if (data[cursor] > 0) data[cursor]--;
			}
			duty_cycle = (data[0] * 1000) + (data[1] * 100) + (data[2] * 10) + data[3];
		} else
			if (down_button_press()){
				generate_pulse();
				xil_printf("10us pulse generated\n");
			}

		delay_ms2(1);
	}
}
