#include "sys.h"
	
//float adcx;
u16 yawpidset;
//float angle=0;
int main(void)
 {     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	 	//��ʱ������ʼ��	  
	uart_init(9600);	 		//���ڳ�ʼ��Ϊ9600
	uart3_init(115200);
	Adc_Init();
	Dac1_Init();
	MPU_Init();					//��ʼ��MPU6050
	delay_ms(100);//�ȴ�ģ���ʼ�����
	while(mpu_dmp_init())
 	{
 		delay_ms(500);
	} 
	delay_ms(100);	
	TIM4_Int_Init(49,7199);//5ms
 	while(1)
	{	
		
	}
}

