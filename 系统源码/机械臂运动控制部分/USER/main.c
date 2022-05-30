#include "sys.h"
#include "delay.h"
#include "usart.h"	
#include "pwm.h"
#include "timer.h"


u8 t;
u16 a1=0;
u16 a11=0;
u16 a2=0;
u16 a22=0;
uint16 ServoPwmDutySet1[][8]={
															{1500,800,1500,2000,2200,1900,1500,1500},//复位
															{1500,800,1500,2000,2300,1900,1800,1500},//左转
	                            {1500,800,1500,1200,2300,1000,1800,1500},//伸手
                              {1500,2200,1500,1200,2300,1000,1800,1500},//夹紧
															{1500,2200,1500,1200,2000,1000,1800,1500},//抬手
															{1500,2200,1500,1200,2000,1000,1500,1500},//旋转回中
															{1500,2200,1500,1200,2300,1000,1500,1500},//下降
															{1500,800,1500,1200,2300,1000,1500,1500},//松手
															{1500,800,1500,2000,2200,1900,1500,1500},//复位

                               };

uint16 ServoPwmDutySet2[][8]={
															 {1500,800,1500,2000,2200,1900,1500,1500},//复位
															 {1500,800,1500,2000,2200,1900,1200,1500},//右转
                               {1500,800,1500,1200,2300,1000,1200,1500},//伸手
                               {1500,2200,1500,1200,2300,1000,1200,1500},//夹紧
														 	 {1500,2200,1500,1200,2000,1000,1200,1500},//抬手
															 {1500,2200,1500,1200,2000,1000,1500,1500},//旋转回中
															 {1500,2200,1500,1200,2300,1000,1500,1500},//下降
															 {1500,800,1500,1200,2300,1000,1500,1500},//松手
															 {1500,800,1500,2000,2200,1900,1500,1500},//复位

                               };

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  //初始化延时函数

	uart1_init(9600);
	InitPWM();
	a1=0;
  a11=0;
  a2=0;
  a22=0;
	
	while(1) 
	{
		if(keypwm3==1 && keypwm4==0)//抓取一号目标
		{
				flag11=0;
				a2=0;
				a22=0;
				ServoPwmDutySet[1]=ServoPwmDutySet1[a1][1];	
				ServoPwmDutySet[2]=ServoPwmDutySet1[a1][2];
				ServoPwmDutySet[3]=ServoPwmDutySet1[a1][3];
				ServoPwmDutySet[4]=ServoPwmDutySet1[a1][4];
				ServoPwmDutySet[5]=ServoPwmDutySet1[a1][5];
				ServoPwmDutySet[6]=ServoPwmDutySet1[a1][6];
				ServoPwmDutyHaveChange=1;
				if(syst%10==0)//每20ms进一次比较，根据具体情况调整
				{
					ServoPwmDutyCompare();	
					a11=a11+1;
				}
				if(a11%3000==0)
				{
					a11=a11+1;
					if(a1<8)
						a1=a1+1;
					printf("完成动作1\r\n");
				}
			
		}
		
		else if(keypwm3==1 && keypwm4==1)//抓取二号目标
		{
			flag11=0;
			a1=0;
			a11=0;
			ServoPwmDutySet[1]=ServoPwmDutySet2[a2][1];	
			ServoPwmDutySet[2]=ServoPwmDutySet2[a2][2];
			ServoPwmDutySet[3]=ServoPwmDutySet2[a2][3];
			ServoPwmDutySet[4]=ServoPwmDutySet2[a2][4];
			ServoPwmDutySet[5]=ServoPwmDutySet2[a2][5];
			ServoPwmDutySet[6]=ServoPwmDutySet2[a2][6];
			ServoPwmDutyHaveChange=1;
			if(syst%10==0)//每20ms进一次比较，根据具体情况调整
			{
				ServoPwmDutyCompare();	
				a22=a22+1;
			}
			if(a22%3000==0)
			{
				a22=a22+1;
				if(a2<8)
					a2=a2+1;
				printf("完成动作2\r\n");
			}		
			
		}
		
		else if(keypwm3==0 && keypwm4==1)//切换体感控制
		{
			a1=0;
			a11=0;
			a2=0;
			a22=0;
			flag11=1;
			if(syst%5==0)//每20ms进一次比较，根据具体情况调整
				ServoPwmDutyCompare();	
		}	
		
		else if(keypwm3==0 && keypwm4==0)
		{
			flag11=0;
			a1=0;
			a11=0;
			a2=0;
			a22=0;
			ServoPwmDutySet[1]=800;
			ServoPwmDutySet[2]=1500;
			ServoPwmDutySet[3]=2000;
			ServoPwmDutySet[4]=2200;
			ServoPwmDutySet[5]=1900;
			ServoPwmDutySet[6]=1500;
			ServoPwmDutyHaveChange=1;
			if(syst%10==0)//每20ms进一次比较，根据具体情况调整
				ServoPwmDutyCompare();	

		}
	}
}

