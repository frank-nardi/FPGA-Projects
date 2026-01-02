#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#define one_ms 100*1000
#define one_us 100

#define LEDS ( *(unsigned volatile *)0x40000000)
#define SW ( *(unsigned volatile *)0x40000008)
#define JB ( *(unsigned volatile *)0x40010000)
#define DPSEG ( *(unsigned volatile *)0x40020000)
#define AN ( *(unsigned volatile *)0x40020008)
#define BTN ( *(unsigned volatile *)0x40030000)

#define alarm_cntr (*(volatile unsigned *)0x44a10000)
#define alarm1_alarm0 (*(volatile unsigned *)0x44a10004)
#define alarm0_value (*(volatile unsigned *)0x44a10008)
#define alarm1_value (*(volatile unsigned *)0x44a1000C)

#define JXADC1_CH6 (*(volatile unsigned *)0x44a20258)

void delay_ms2(uint32_t val)
{
	while((alarm1_alarm0 &(1<<0))==0)
	{

	}
	alarm0_value= one_ms * val;
}

void sev_blinker(uint8_t data[4],uint8_t cursor)
{

	static uint16_t cnt=0;
		static uint16_t digit = 0;
		_Bool disp;
		const uint8_t disp_lut[10] = {0b00111111, 0b0000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111};

		if(cnt != cursor || disp){
			AN = ~(1<<cnt);
		}else {
			AN = 0xF;
		}

	    DPSEG=~disp_lut[data[cnt]];
	    if(++cnt == 4){
	    	cnt = 0;
	    }
	    if (digit == 1){
	    		DPSEG ^= 0b10000000;
	    }

	    if(digit < 500){
	    	disp = 1;
	    } else {
	    	disp = 0;
	    }

	    if(++digit == 1000) digit = 0;


}
void delay_us(uint32_t val)
{
	while((alarm1_alarm0 &(1<<1))==0)
	{

	}
	alarm1_value= one_us * val;
}

int main()
{
    init_platform();

    uint8_t data[4];
	data[3]=0;
	data[2]=0;
	data[1]=0;
	data[0]=0;


    print("Hello World\n\r");
    while(1)
    {
    	delay_ms2(1000);
    	xil_printf("%d \n",JXADC1_CH6);
    	xil_printf("%d \n",((JXADC1_CH6>>4)*244)/1000);
    	print("---------------");
    }
}
