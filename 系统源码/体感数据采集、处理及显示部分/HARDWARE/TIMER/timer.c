#include "sys.h"
#include "usart3.h"	
unsigned char  buf[64];
unsigned char  buf1[64];
short temp;					//温度	
float pitch,roll,yaw,balance_pitch1,balance_roll1,balance_yaw1; 		//欧拉角
u8 data[9]={0xAA,0,0,0,0,0,0,0,0xEE};//数据包 帧头0xaa，帧尾0xee
u16 balance_pitch,balance_roll,balance_yaw;
static u8 i=0;

float adcx10;

int flexADC;
float flexV=0.0;
float flexR=0.0;
float angle_1=0.0;
const float VCC =3.3; // 模块供电电压，ADC参考电压为V
const float R_DIV =100000.0; // 分压电阻为100KΩ
const float STRAIGHT_RESISTANCE =29089.0; // 平直时的电阻值  需修改
const float BEND_RESISTANCE =42063.0; // 90度弯曲时的电阻值  需修改
long map(long x, long in_min, long in_max, long out_min, long out_max);



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
			flexADC = Get_Adc_Average(1,25);	//10次平均值
			flexV=(flexADC*VCC)/4096;
			flexR= R_DIV * (VCC / flexV-1.0);
			angle_1 = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,0, 90.0);	
			printf("Bend:%fdegrees\r\n",angle_1);
			data[1]=angle_1;//数据包1�
			DAC_SetChannel1Data(DAC_Align_12b_R, yaw+180);//设置DAC值
			printf("设置:%f\r\n",yaw);
			
			adcx10 = Get_Adc_Average(10,25);	//10次平均值
			printf("adcx10:%f\r\n",adcx10);
			if(90<adcx10<120)adcx10=90;
			yawpidset=adcx10+180+yaw-90;
			printf("balance_yaw:%d\r\n",balance_yaw);
			
			balance_pitch=pitch+180;//保证角度值全为正
			balance_roll=roll+180;	//陀螺仪数据有负值	
			balance_yaw=yawpidset;	//不能直接通过串口发送
			
			balance_pitch1=balance_pitch-180;
			balance_roll1=balance_roll-180;
			balance_yaw1=balance_yaw-180;
	
			
			sprintf((char *)buf,"page1.t5.txt=\"%.1f\"",pitch); //强制类型转化，转化为字符串
			HMISends((char *)buf); //发送yaw的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
		
			sprintf((char *)buf,"page1.t6.txt=\"%.1f\"",roll); //强制类型转化，转化为字符串
			HMISends((char *)buf); //发送pitch的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
		
			sprintf((char *)buf,"page1.t7.txt=\"%.1f\"",yaw); //强制类型转化，转化为字符串
			HMISends((char *)buf); //发送roll的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
			
			sprintf((char *)buf1,"page1.t8.txt=\"%.1f\"",balance_pitch1); //强制类型转化，转化为字符串
			HMISends((char *)buf1); //发送roll的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
		
			sprintf((char *)buf1,"page1.t9.txt=\"%.1f\"",balance_roll1); //强制类型转化，转化为字符串
			HMISends((char *)buf1); //发送roll的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
			
			sprintf((char *)buf1,"page1.t10.txt=\"%.1f\"",balance_yaw1); //强制类型转化，转化为字符串
			HMISends((char *)buf1); //发送roll的数据给page0页面的t3文本控件
			HMISendb(0xff);//结束符
			
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

 long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }