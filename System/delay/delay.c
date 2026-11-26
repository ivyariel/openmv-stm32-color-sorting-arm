#include "delay.h"



static uint32_t fac_us=0;							//SysTick ticks per microsecond
		   
//Initialize SysTick-based delay timing
//The caller supplies the system clock frequency in MHz
//SysTick uses the AHB clock
//SYSCLK: system clock frequency in MHz
void delay_init(uint8_t SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//Use HCLK as the SysTick clock
	fac_us=SYSCLK;						//Store ticks per microsecond for delay calculations

}								    

//Delay for nus microseconds
//nus: requested delay in microseconds.	
//The maximum delay is limited by the 32-bit tick count.	 
void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//SysTick reload value	    	 
	ticks=nus*fac_us; 						//Required SysTick ticks 
	told=SysTick->VAL;        				//Counter value at entry
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//SysTick counts down.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//Exit once the requested delay has elapsed.
		}  
	};
}

//Delay for nms milliseconds
//nms: requested delay in milliseconds
void delay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) delay_us(1000);
}
			 



































