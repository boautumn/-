#include "sys.h"
#include "usart3.h"	
unsigned char  buf[64];
unsigned char  buf1[64];
short temp;					//�¶�	
float pitch,roll,yaw,balance_pitch1,balance_roll1,balance_yaw1; 		//ŷ����
u8 data[9]={0xAA,0,0,0,0,0,0,0,0xEE};//���ݰ� ֡ͷ0xaa��֡β0xee
u16 balance_pitch,balance_roll,balance_yaw;
static u8 i=0;

float adcx10;

int flexADC;
float flexV=0.0;
float flexR=0.0;
float angle_1=0.0;
const float VCC =3.3; // ģ�鹩���ѹ��ADC�ο���ѹΪV
const float R_DIV =100000.0; // ��ѹ����Ϊ100K��
const float STRAIGHT_RESISTANCE =29089.0; // ƽֱʱ�ĵ���ֵ  ���޸�
const float BEND_RESISTANCE =42063.0; // 90������ʱ�ĵ���ֵ  ���޸�
long map(long x, long in_min, long in_max, long out_min, long out_max);



//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
				    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ    
	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2									 
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ  
	TIM4->PSC=psc;  	//Ԥ��Ƶ��		  
	TIM4->DIER|=1<<0;   //��������ж�	  
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��4
  	MY_NVIC_Init(0,0,TIM4_IRQn,2);//������ȼ�������
}

void TIM4_IRQHandler(void)//��ʱ���ʹ���������
{ 		    		  			    
	if(TIM4->SR&0X0001)//����ж�
	{
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			flexADC = Get_Adc_Average(1,25);	//10��ƽ��ֵ
			flexV=(flexADC*VCC)/4096;
			flexR= R_DIV * (VCC / flexV-1.0);
			angle_1 = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,0, 90.0);	
			printf("Bend:%fdegrees\r\n",angle_1);
			data[1]=angle_1;//���ݰ�1�
			DAC_SetChannel1Data(DAC_Align_12b_R, yaw+180);//����DACֵ
			printf("����:%f\r\n",yaw);
			
			adcx10 = Get_Adc_Average(10,25);	//10��ƽ��ֵ
			printf("adcx10:%f\r\n",adcx10);
			if(90<adcx10<120)adcx10=90;
			yawpidset=adcx10+180+yaw-90;
			printf("balance_yaw:%d\r\n",balance_yaw);
			
			balance_pitch=pitch+180;//��֤�Ƕ�ֵȫΪ��
			balance_roll=roll+180;	//�����������и�ֵ	
			balance_yaw=yawpidset;	//����ֱ��ͨ�����ڷ���
			
			balance_pitch1=balance_pitch-180;
			balance_roll1=balance_roll-180;
			balance_yaw1=balance_yaw-180;
	
			
			sprintf((char *)buf,"page1.t5.txt=\"%.1f\"",pitch); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf); //����yaw�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
		
			sprintf((char *)buf,"page1.t6.txt=\"%.1f\"",roll); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf); //����pitch�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
		
			sprintf((char *)buf,"page1.t7.txt=\"%.1f\"",yaw); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf); //����roll�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
			
			sprintf((char *)buf1,"page1.t8.txt=\"%.1f\"",balance_pitch1); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf1); //����roll�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
		
			sprintf((char *)buf1,"page1.t9.txt=\"%.1f\"",balance_roll1); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf1); //����roll�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
			
			sprintf((char *)buf1,"page1.t10.txt=\"%.1f\"",balance_yaw1); //ǿ������ת����ת��Ϊ�ַ���
			HMISends((char *)buf1); //����roll�����ݸ�page0ҳ���t3�ı��ؼ�
			HMISendb(0xff);//������
			
			data[2]=(balance_pitch>>8)&0xff;//��8λ
			data[3]=balance_pitch&0xff;		//��8λ
			data[4]=(balance_roll>>8)&0xff;
			data[5]=balance_roll&0xff;
			data[6]=(balance_yaw>>8)&0xff;
			data[7]=balance_yaw&0xff;
			for(i=0;i<9;i++)	
				usart1_send_char(data[i]);//��������		
		}
		
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}

 long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }