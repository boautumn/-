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
															{1500,800,1500,2000,2200,1900,1500,1500},//��λ
															{1500,800,1500,2000,2300,1900,1800,1500},//��ת
	                            {1500,800,1500,1200,2300,1000,1800,1500},//����
                              {1500,2200,1500,1200,2300,1000,1800,1500},//�н�
															{1500,2200,1500,1200,2000,1000,1800,1500},//̧��
															{1500,2200,1500,1200,2000,1000,1500,1500},//��ת����
															{1500,2200,1500,1200,2300,1000,1500,1500},//�½�
															{1500,800,1500,1200,2300,1000,1500,1500},//����
															{1500,800,1500,2000,2200,1900,1500,1500},//��λ

                               };

uint16 ServoPwmDutySet2[][8]={
															 {1500,800,1500,2000,2200,1900,1500,1500},//��λ
															 {1500,800,1500,2000,2200,1900,1200,1500},//��ת
                               {1500,800,1500,1200,2300,1000,1200,1500},//����
                               {1500,2200,1500,1200,2300,1000,1200,1500},//�н�
														 	 {1500,2200,1500,1200,2000,1000,1200,1500},//̧��
															 {1500,2200,1500,1200,2000,1000,1500,1500},//��ת����
															 {1500,2200,1500,1200,2300,1000,1500,1500},//�½�
															 {1500,800,1500,1200,2300,1000,1500,1500},//����
															 {1500,800,1500,2000,2200,1900,1500,1500},//��λ

                               };

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();  //��ʼ����ʱ����

	uart1_init(9600);
	InitPWM();
	a1=0;
  a11=0;
  a2=0;
  a22=0;
	
	while(1) 
	{
		if(keypwm3==1 && keypwm4==0)//ץȡһ��Ŀ��
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
				if(syst%10==0)//ÿ20ms��һ�αȽϣ����ݾ����������
				{
					ServoPwmDutyCompare();	
					a11=a11+1;
				}
				if(a11%3000==0)
				{
					a11=a11+1;
					if(a1<8)
						a1=a1+1;
					printf("��ɶ���1\r\n");
				}
			
		}
		
		else if(keypwm3==1 && keypwm4==1)//ץȡ����Ŀ��
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
			if(syst%10==0)//ÿ20ms��һ�αȽϣ����ݾ����������
			{
				ServoPwmDutyCompare();	
				a22=a22+1;
			}
			if(a22%3000==0)
			{
				a22=a22+1;
				if(a2<8)
					a2=a2+1;
				printf("��ɶ���2\r\n");
			}		
			
		}
		
		else if(keypwm3==0 && keypwm4==1)//�л���п���
		{
			a1=0;
			a11=0;
			a2=0;
			a22=0;
			flag11=1;
			if(syst%5==0)//ÿ20ms��һ�αȽϣ����ݾ����������
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
			if(syst%10==0)//ÿ20ms��һ�αȽϣ����ݾ����������
				ServoPwmDutyCompare();	

		}
	}
}

