#include <main.h>

#INT_TIMER1
void  TIMER1_isr(void) 
{

}

void main()
{
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);		//21.8 ms overflow


	enable_interrupts(INT_TIMER1);
	enable_interrupts(GLOBAL);

	while(TRUE)
	{
		//TODO: User Code
	}

}
