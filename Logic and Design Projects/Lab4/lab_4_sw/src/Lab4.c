	#include "xil_printf.h"
	#include <stdint.h>
	#include <stdbool.h>

	#define DELAY_UNIT 81

	#define LEDS  ( *(unsigned volatile *)0x40000000 ) // GPIO-0 16-bit
	#define SW    ( *(unsigned volatile *)0x40000008 ) // GPIO-0 16-bit
	#define JB    ( *(unsigned volatile *)0x40010000 ) // GPIO-1 8-bit
	#define DPSEG ( *(unsigned volatile *)0x40020000 ) // GPIO-2 {DP, SEG[6:0]}
	#define AN    ( *(unsigned volatile *)0x40020008 ) // GPIO-2 4-bit
	#define BTN   ( *(unsigned volatile *)0x40030000 ) // GPIO-3 4-bit, {btnR, btnL, btnD, btnU};

	void delay_ms(unsigned t){
	unsigned cntr1, cntr2;
	while(t--)
	for (cntr1=0; cntr1<100; cntr1++){
	for (cntr2=0; cntr2<DELAY_UNIT; cntr2++){}
	}
	}

	_Bool _Bool_up_button_press()
	{
	static enum {NOT_PRESSED, DEBOUNCE_PRESS, PRESSED, DEBOUNCE_RELEASE}
	state= NOT_PRESSED;
	static uint8_t cnt;
	_Bool retval=false;

	_Bool B= BTN&(1<<0);

	switch(state){
	case NOT_PRESSED:
		if( B ){
		cnt=0;
		state=DEBOUNCE_PRESS;
		}
		break;
	case DEBOUNCE_PRESS:
		if( B && cnt < 5 )
			cnt++;
		else if( B && cnt >= 5 ) {
			retval=true;
			state=PRESSED;
		}
	else if (!B)
		state = NOT_PRESSED;
		break;
	case PRESSED:
	if( !B ){
		cnt=0;
		state=DEBOUNCE_RELEASE;
	}
		break;
	case DEBOUNCE_RELEASE:
		if( !B && cnt < 5)
		cnt++;
	else if( !B && cnt >= 5 )
		state = NOT_PRESSED;
	else if( B )
		state = PRESSED;

	}
	return retval;
	}


	_Bool _Bool_down_button_press()
	{
	static enum {NOT_PRESSED, DEBOUNCE_PRESS, PRESSED, DEBOUNCE_RELEASE}
	state= NOT_PRESSED;
	static uint8_t cnt;
	_Bool retval=false;

	_Bool B= BTN&(1<<1);

	switch(state){
	case NOT_PRESSED:
	if( B ){
	cnt=0;
	state=DEBOUNCE_PRESS;
	}
	break;
	case DEBOUNCE_PRESS:
	if( B && cnt < 5 )
	cnt++;
	else if( B && cnt >= 5 ) {
	retval=true;
	state=PRESSED;
	}
	else if (!B)
	state = NOT_PRESSED;
	break;
	case PRESSED:
	if( !B ){
	cnt=0;
	state=DEBOUNCE_RELEASE;
	}
	break;
	case DEBOUNCE_RELEASE:
	if( !B && cnt < 5)
	cnt++;
	else if( !B && cnt >= 5 )
	state = NOT_PRESSED;
	else if( B )
	state = PRESSED;

	}
	return retval;
	}


	_Bool _Bool_left_button_press()
	{
	static enum {NOT_PRESSED, DEBOUNCE_PRESS, PRESSED, DEBOUNCE_RELEASE}
	state= NOT_PRESSED;
	static uint8_t cnt;
	_Bool retval=false;

	_Bool B= BTN&(1<<2);

	switch(state){
	case NOT_PRESSED:
	if( B ){
	cnt=0;
	state=DEBOUNCE_PRESS;
	}
	break;
	case DEBOUNCE_PRESS:
	if( B && cnt < 5 )
	cnt++;
	else if( B && cnt >= 5 ) {
	retval=true;
	state=PRESSED;
	}
	else if (!B)
	state = NOT_PRESSED;
	break;
	case PRESSED:
	if( !B ){
	cnt=0;
	state=DEBOUNCE_RELEASE;
	}
	break;
	case DEBOUNCE_RELEASE:
	if( !B && cnt < 5)
	cnt++;
	else if( !B && cnt >= 5 )
	state = NOT_PRESSED;
	else if( B )
	state = PRESSED;

	}
	return retval;
	}


	_Bool _Bool_right_button_press()
	{
	static enum {NOT_PRESSED, DEBOUNCE_PRESS, PRESSED, DEBOUNCE_RELEASE}
	state= NOT_PRESSED;
	static uint8_t cnt;
	_Bool retval=false;

	_Bool B= BTN&(1<<3);

	switch(state){
	case NOT_PRESSED:
	if( B ){
	cnt=0;
	state=DEBOUNCE_PRESS;
	}
	break;
	case DEBOUNCE_PRESS:
	if( B && cnt < 5 )
	cnt++;
	else if( B && cnt >= 5 ) {
	retval=true;
	state=PRESSED;
	}
	else if (!B)
	state = NOT_PRESSED;
	break;
	case PRESSED:
	if( !B ){
	cnt=0;
	state=DEBOUNCE_RELEASE;
	}
	break;
	case DEBOUNCE_RELEASE:
	if( !B && cnt < 5)
	cnt++;
	else if( !B && cnt >= 5 )
	state = NOT_PRESSED;
	else if( B )
	state = PRESSED;

	}
	return retval;
	}

	void seg_disp (uint8_t data[4], uint8_t cursor)
	{
	const uint8_t disp_lut[10] =
	{0b00111111,
	0b0000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111};
	static uint8_t digit = 0;
	static uint16_t cnt = 0; // flash counter

	AN = ~(1<<(3-digit)); // enable the LEDs
	DPSEG = ~disp_lut[data[digit]]; // write data??
	if (digit == cursor && cnt < 1000)
		DPSEG = 0b11111111;
	if (digit == 2)
		DPSEG ^= 0b10000000;
		digit = (digit + 1)%4;
		cnt = (cnt + 1)%2000; // flash counter modulo
	if( ++cnt == 2000)
		cnt = 0;
	}

	//Lab code
	void led_jb_flasher(int duty)
	{
		static uint16_t state = 0;

		if( state == 999 )
	{
	LEDS |= (1<<0);
	JB |= (1<<0);
	}

	if( state == duty - 1 )
	{
		LEDS &= ~(1<<0);
		JB &= ~(1<<0);
	}

	if( ++state == 1000)
		state = 0;

	}

	int main()
	{
		uint16_t duty = 500;
		uint8_t data[4];
		uint8_t cursor = 0;

		_Bool btnU, btnD, btnL, btnR;


		while(1)
		{
			delay_ms(1);

			btnU = _Bool_up_button_press();
			btnD = _Bool_down_button_press();
			btnL = _Bool_left_button_press();
			btnR = _Bool_right_button_press();

		// Prelab
	//	 data[0] = 1;
	//	 data[1] = 2;
	//	 data[2] = 3;
	//	 data[3] = 4;

			if (btnR == 1)
			{
				cursor++;
				if (cursor == 4)
					cursor = 0;
				}

				if (btnL == 1)
				{
					cursor--;
					if (cursor == 0b11111111)
						cursor = 3;
			}

			if (btnU == 1)
			{
				if (cursor == 3)
					duty++;
				if (cursor == 2)
					duty += 10;
				if (cursor == 1)
					duty += 100;
				if (cursor == 0)
					duty += 1000;

				if (duty > 1000)
					duty = 1000;
			}

			if (btnD == 1)
			{
				if (cursor == 3)
					duty--;
				if (cursor == 2)
					duty = duty - 10;
				if (cursor == 1)
					duty -= 100;
				if (cursor == 0)
					duty -= 1000;

				if (duty > 1000)
					duty = 0;
			}



		data[0] = duty / 1000;
		data[1] = (duty - data[0] * 1000) / 100;
		data[2] = (duty - data[0] * 1000 - data[1] * 100) / 10;
		data[3] = (duty - data[0] * 1000 - data[1] * 100 - data[2] * 10);

		seg_disp(data, cursor);

		led_jb_flasher(duty);

	}



}
