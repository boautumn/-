//#include "led.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"


void TIM2_Int_Init(u16 arr,u16 psc)//�Ĵ�����ʽ��ʼ�������ÿ⺯�����������ۣ�
{									//sys�ļ����������Ӧ��������������ӼĴ����汾���뵽�����̲��ᱨ��
	RCC->APB1ENR|=1<<0;	//TIM2ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM2->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM2->DIER|=1<<0;   //��������ж�	  
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//��ռ1�������ȼ�3����2	����NVIC��������ʱ���ж�								 
}
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM3ʱ��ʹ��    
 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ 
	TIM5->PSC=psc;  		//Ԥ��Ƶ��	  
	TIM5->DIER|=1<<0;   //��������ж�	  
	TIM5->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1,2,TIM5_IRQn,2);	//��ռ1�������ȼ�3����2	����NVIC��������ʱ���ж�								 
}


