#include "xil_printf.h"

#define LED (*(unsigned volatile *)0x40000000)
#define SW (*(unsigned volatile *)0x40000008)
#define JB (*(unsigned volatile *)0x40010000)
#define DELAY_UNIT 81

void delay_ms(unsigned t){
	unsigned cntr1, cntr2;
	while(t--)
	{
		for (cntr1=0; cntr1 < 100; cntr1++)
		{
			for (cntr2=0; cntr2<DELAY_UNIT; cntr2++){}
		}
	}
}

int main()
{
//	unsigned cntr = 0;
//
//	while(1)
//	{
//		delay_ms(1);
//		if(++cntr == 500)
//		{
//			cntr = 0;
//			LED ^= SW;
//		}
//	}
	int cntrA = 0;
	int cntrB = 0;
	int cntrC = 0;
	int cntr = 0;
	print("Start\n");
	while(1)
	{
		delay_ms(1);
		if(++cntrC==1000)
		{
			cntrC=0;
		}
		if(++cntrA==100)
		{
			JB ^=(1<<0);
			cntrA=0;
		}
		if(++cntrB==50)
		{
		   JB ^=(1<<1);
		   cntrB=0;
		}
		//     SWITCH PART
		if(((SW & (1<<14))==0) &&((SW & (1<<15))==0))
		{
			LED=0;
		}
		else if(((SW & (1<<14))!=0) &&((SW & (1<<15))==0))
		{
			LED ^=0xFFFF;
		}
		else if(((SW & (1<<14))==0) &&((SW & (1<<15))!=0))
		{
			LED=SW;
		}
		else
		{
			if(cntrC==0)
			{
				LED ^=0xFFFF;
			}
		}
		if(++cntr == 5000)
		{
			for(int i =15; i>0;i--)
			{
				if((SW & (1<<i))==0)
				{
					print("0");
				}
			else
			{
				print("1");
			}
		}
			print("\n");
		cntr=0;
		}
    }
}
