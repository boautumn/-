//#include "led.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"


void TIM2_Int_Init(u16 arr,u16 psc)//寄存器方式初始化，觉得库函数用起来好累，
{									//sys文件里添加了相应函数和声明，添加寄存器版本代码到本工程不会报错
	RCC->APB1ENR|=1<<0;	//TIM2时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值 
	TIM2->PSC=psc;  	//预分频器	  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器2
	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//抢占1，子优先级3，组2	配置NVIC，开启定时器中断								 
}
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM3时钟使能    
 	TIM5->ARR=arr;  		//设定计数器自动重装值 
	TIM5->PSC=psc;  		//预分频器	  
	TIM5->DIER|=1<<0;   //允许更新中断	  
	TIM5->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(1,2,TIM5_IRQn,2);	//抢占1，子优先级3，组2	配置NVIC，开启定时器中断								 
}


