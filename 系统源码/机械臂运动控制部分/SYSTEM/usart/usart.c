#include "sys.h"
#include "usart.h"	  
#include "pwm.h"	  

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
//#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
//*******************************************
int adcx,pitch,roll,yaw; 			//ADCֵ+ŷ����
float balance_pitch,balance_roll,balance_yaw,balance_ADC;
u8 Res;
u8 flag=0;
u8 flag11=0;
u8 USART_receive[7];//���ݰ�



void uart1_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  //*******************************************************************
	USART_DeInit(USART1); //��λ���� 1
	
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
	
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}
//*******************************************
void uart3_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX   PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = 9600;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�

	USART_Cmd(USART3, ENABLE);					  //ʹ�ܴ���
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
}


//************************************************
void transport(void)
{
	int m;
	for(m=2;m<=4;m++)
	{
		if((USART_receive[m]&0xf0)==0x0f)
		{
			USART_receive[m]=USART_receive[m]&0x7f;
			USART_receive[m]=-USART_receive[m];
		}
	}
}

void Xianfu_Pwm_catch(void)
{	
	int Amplitude=2500;    
	int Amplitude1=1200;	
	if(balance_ADC>Amplitude)  balance_ADC=Amplitude;	
	if(balance_ADC<Amplitude1)  balance_ADC=Amplitude1;	
}

void Xianfu_Pwm_pitch(void)
{
	int Amplitude=2500;    
	int Amplitude1=500;	
	if(balance_pitch>Amplitude)  balance_pitch=Amplitude;
	if(balance_pitch<Amplitude1)  balance_pitch=Amplitude1;		
}
void Xianfu_Pwm_roll(void)
{
	int Amplitude=2500;    
	int Amplitude1=500;		
	if(balance_roll>Amplitude)  balance_roll=Amplitude;
	if(balance_roll<Amplitude1)  balance_roll=Amplitude1;		
}
void Xianfu_Pwm_yaw(void)
{
	int Amplitude=2500;    
	int Amplitude1=500;		
	if(balance_yaw>Amplitude)  balance_yaw=Amplitude;
	if(balance_yaw<Amplitude1)  balance_yaw=Amplitude1;		
}
	
void Pwm_control(void)
{
	//��ץ
	balance_ADC=800+(adcx)/90.0*4096/3.73;
	Xianfu_Pwm_catch();
	ServoPwmDutySet[1]=balance_ADC;
	
	
	balance_roll=1500-pitch*11.11;//�����õ���270������ÿһ������7.4pwmֵ��ԭ***********************************************7.4
	Xianfu_Pwm_roll();
	ServoPwmDutySet[2]=balance_roll;
	
	
	balance_pitch=1500-roll*13;//180������ԭ��**************************************8**********11.11
	Xianfu_Pwm_pitch();
	TIM4->CCR3=balance_pitch;				 
	if(roll>10)//
		ServoPwmDutySet[3]=2000-roll*20;
	else if(roll>5 &&roll<=10)
		ServoPwmDutySet[3]=2000-roll*10;
	else
		ServoPwmDutySet[3]=2000;//
	
	if(roll>10)//
		ServoPwmDutySet[4]=2200+roll*13;
	else
		ServoPwmDutySet[4]=2200;
	
	if(balance_pitch<1500)
		ServoPwmDutySet[5]=500+1500-roll*22;
	else
		ServoPwmDutySet[5]=500+balance_pitch;
		
	
	
	balance_yaw=1500+yaw*11.07;//��λ���ϵ��ʼ��ʱһ��Ҫ����ˮƽ���ã�ԭ**********************************************11.11
	Xianfu_Pwm_yaw();			//����MPU6050��yaw�ǳ����׷���ƫ��
	ServoPwmDutySet[6]=balance_yaw;
	
}

#if EN_USART3_RX   //���ʹ���˽���
//����3�жϷ������
void USART3_IRQHandler(void) 
{
	static u8 i=0;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE))//�����ж�(���յ������ݱ�����0xAA��ͷ   0xEE��β)
	{	
		Res =USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������	
		if(Res==0xEE)//���յ�֡β
		{
			flag=0;//��־λ��λ
			i=0;//����
			adcx=(int)USART_receive[0];//��λ adc����
			pitch=(int)((USART_receive[1]<<8)+USART_receive[2])-180;//��ԭ��������λ��ԭʼ����
			roll=(int)((USART_receive[3]<<8)+USART_receive[4])-180;	
			yaw=(int)((USART_receive[5]<<8)+USART_receive[6])-180;	
			Pwm_control();
			ServoPwmDutyHaveChange=1;//pwm�仯��־λ��1
			printf("wuwuwu%d",pitch);

		}
		if(flag==1)//��������
		{
		  USART_receive[i++]=Res;
		}
		if(Res==0xAA)//���յ�֡ͷ
		{
			flag=1;//��־λ��1
		}
	}
} 
#endif


#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
void USART1_IRQHandler(void) 
{
	static u8 i=0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE))//�����ж�(���յ������ݱ�����0xAA��ͷ   0xEE��β)
	{	
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������	
		if(Res==0xEE)//���յ�֡β
		{
			flag=0;//��־λ��λ
			i=0;//����
			adcx=(int)USART_receive[0];//��λ adc����
			pitch=(int)((USART_receive[1]<<8)+USART_receive[2])-180;//��ԭ��������λ��ԭʼ����
			roll=(int)((USART_receive[3]<<8)+USART_receive[4])-180;	
			yaw=(int)((USART_receive[5]<<8)+USART_receive[6])-180;	
			if(flag11==1)
			{
				Pwm_control();
				ServoPwmDutyHaveChange=1;//pwm�仯��־λ��1
			}
		}
		if(flag==1)//��������
		{
		  USART_receive[i++]=Res;
		}
		if(Res==0xAA)//���յ�֡ͷ
		{
			flag=1;//��־λ��1
		}
	}
} 
#endif





















//void USART1_IRQHandler(void)                	//����1�жϷ������
//	{
//	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		
//		if((USART_RX_STA&0x8000)==0)//����δ���
//			{
//			if(USART_RX_STA&0x4000)//���յ���0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//				else USART_RX_STA|=0x8000;	//��������� 
//				}
//			else //��û�յ�0X0D
//				{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//					}		 
//				}
//			}   		 
//     } 
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntExit();  											 
//#endif
//} 
//#endif	

