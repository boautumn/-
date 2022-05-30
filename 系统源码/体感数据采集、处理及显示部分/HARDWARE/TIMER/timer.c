#include "sys.h"
#include "usart3.h"	
unsigned char  buf[64];
unsigned char  buf1[64];
short temp;					//ÎÂ¶È	
float pitch,roll,yaw,balance_pitch1,balance_roll1,balance_yaw1; 		//Å·À­½Ç
u8 data[9]={0xAA,0,0,0,0,0,0,0,0xEE};//Êý¾Ý°ü Ö¡Í·0xaa£¬Ö¡Î²0xee
u16 balance_pitch,balance_roll,balance_yaw;
static u8 i=0;

float adcx10;

int flexADC;
float flexV=0.0;
float flexR=0.0;
float angle_1=0.0;
const float VCC =3.3; // Ä£¿é¹©µçµçÑ¹£¬ADC²Î¿¼µçÑ¹ÎªV
const float R_DIV =100000.0; // ·ÖÑ¹µç×èÎª100K¦¸
const float STRAIGHT_RESISTANCE =29089.0; // Æ½Ö±Ê±µÄµç×èÖµ  ÐèÐÞ¸Ä
const float BEND_RESISTANCE =42063.0; // 90¶ÈÍäÇúÊ±µÄµç×èÖµ  ÐèÐÞ¸Ä
long map(long x, long in_min, long in_max, long out_min, long out_max);



//¶¨Ê±Æ÷3ÖÐ¶Ï·þÎñ³ÌÐò	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//Òç³öÖÐ¶Ï
	{
				    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//Çå³ýÖÐ¶Ï±êÖ¾Î» 	    
}
//Í¨ÓÃ¶¨Ê±Æ÷3ÖÐ¶Ï³õÊ¼»¯
//ÕâÀïÊ±ÖÓÑ¡ÔñÎªAPB1µÄ2±¶£¬¶øAPB1Îª36M
//arr£º×Ô¶¯ÖØ×°Öµ¡£
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3Ê±ÖÓÊ¹ÄÜ    
 	TIM3->ARR=arr;  	//Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ    
	TIM3->PSC=psc;  	//Ô¤·ÖÆµÆ÷	  
	TIM3->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï	  
	TIM3->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2									 
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4Ê±ÖÓÊ¹ÄÜ    
 	TIM4->ARR=arr;  	//Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ  
	TIM4->PSC=psc;  	//Ô¤·ÖÆµÆ÷		  
	TIM4->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï	  
	TIM4->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷4
  	MY_NVIC_Init(0,0,TIM4_IRQn,2);//×î¸ßÓÅÏÈ¼¶£¡£¡£¡
}

void TIM4_IRQHandler(void)//¶¨Ê±·¢ËÍ´«¸ÐÆ÷Êý¾Ý
{ 		    		  			    
	if(TIM4->SR&0X0001)//Òç³öÖÐ¶Ï
	{
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			flexADC = Get_Adc_Average(1,25);	//10´ÎÆ½¾ùÖµ
			flexV=(flexADC*VCC)/4096;
			flexR= R_DIV * (VCC / flexV-1.0);
			angle_1 = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,0, 90.0);	
			printf("Bend:%fdegrees\r\n",angle_1);
			data[1]=angle_1;//Êý¾Ý°ü1Î
			DAC_SetChannel1Data(DAC_Align_12b_R, yaw+180);//ÉèÖÃDACÖµ
			printf("ÉèÖÃ:%f\r\n",yaw);
			
			adcx10 = Get_Adc_Average(10,25);	//10´ÎÆ½¾ùÖµ
			printf("adcx10:%f\r\n",adcx10);
			if(90<adcx10<120)adcx10=90;
			yawpidset=adcx10+180+yaw-90;
			printf("balance_yaw:%d\r\n",balance_yaw);
			
			balance_pitch=pitch+180;//±£Ö¤½Ç¶ÈÖµÈ«ÎªÕý
			balance_roll=roll+180;	//ÍÓÂÝÒÇÊý¾ÝÓÐ¸ºÖµ	
			balance_yaw=yawpidset;	//²»ÄÜÖ±½ÓÍ¨¹ý´®¿Ú·¢ËÍ
			
			balance_pitch1=balance_pitch-180;
			balance_roll1=balance_roll-180;
			balance_yaw1=balance_yaw-180;
	
			
			sprintf((char *)buf,"page1.t5.txt=\"%.1f\"",pitch); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf); //·¢ËÍyawµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
		
			sprintf((char *)buf,"page1.t6.txt=\"%.1f\"",roll); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf); //·¢ËÍpitchµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
		
			sprintf((char *)buf,"page1.t7.txt=\"%.1f\"",yaw); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf); //·¢ËÍrollµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
			
			sprintf((char *)buf1,"page1.t8.txt=\"%.1f\"",balance_pitch1); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf1); //·¢ËÍrollµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
		
			sprintf((char *)buf1,"page1.t9.txt=\"%.1f\"",balance_roll1); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf1); //·¢ËÍrollµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
			
			sprintf((char *)buf1,"page1.t10.txt=\"%.1f\"",balance_yaw1); //Ç¿ÖÆÀàÐÍ×ª»¯£¬×ª»¯Îª×Ö·û´®
			HMISends((char *)buf1); //·¢ËÍrollµÄÊý¾Ý¸øpage0Ò³ÃæµÄt3ÎÄ±¾¿Ø¼þ
			HMISendb(0xff);//½áÊø·û
			
			data[2]=(balance_pitch>>8)&0xff;//¸ß8Î»
			data[3]=balance_pitch&0xff;		//µÍ8Î»
			data[4]=(balance_roll>>8)&0xff;
			data[5]=balance_roll&0xff;
			data[6]=(balance_yaw>>8)&0xff;
			data[7]=balance_yaw&0xff;
			for(i=0;i<9;i++)	
				usart1_send_char(data[i]);//´«ÊäÊý¾Ý		
		}
		
	}				   
	TIM4->SR&=~(1<<0);//Çå³ýÖÐ¶Ï±êÖ¾Î» 	    
}

 long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }