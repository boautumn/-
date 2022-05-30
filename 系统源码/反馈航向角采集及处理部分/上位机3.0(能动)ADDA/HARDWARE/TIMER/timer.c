#include "sys.h"

short temp;					//温度	
float pitch,roll,yaw; 		//欧拉角
u8 data[9]={0xAA,0,0,0,0,0,0,0,0xEE};//数据包 帧头0xaa，帧尾0xee
u16 balance_pitch,balance_roll,balance_yaw;
static u8 i=0;

//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
				    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值    
	TIM3->PSC=psc;  	//预分频器	  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2									 
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值  
	TIM4->PSC=psc;  	//预分频器		  
	TIM4->DIER|=1<<0;   //允许更新中断	  
	TIM4->CR1|=0x01;    //使能定时器4
  	MY_NVIC_Init(0,0,TIM4_IRQn,2);//最高优先级！！！
}

void TIM4_IRQHandler(void)//定时发送传感器数据
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			balance_pitch=pitch+180;//保证角度值全为正
			balance_roll=roll+180;	//陀螺仪数据有负值	
			balance_yaw=yaw+180;	//不能直接通过串口发送
	
			data[2]=(balance_pitch>>8)&0xff;//高8位
			data[3]=balance_pitch&0xff;		//低8位
			data[4]=(balance_roll>>8)&0xff;
			data[5]=balance_roll&0xff;
			data[6]=(balance_yaw>>8)&0xff;
			data[7]=balance_yaw&0xff;
			for(i=0;i<9;i++)	
				usart1_send_char(data[i]);//传输数据		
		}
		
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}









