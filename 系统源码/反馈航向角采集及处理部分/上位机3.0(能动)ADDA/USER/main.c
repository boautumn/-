#include "sys.h"

/*NCHUµÚÊ®Áù½ìÈýÐ¡ÏîÄ¿ Ìå¸Ð»úÐµ±Û-ÉÏÎ»»ú*/

/*
  ½áºÏ¶à·½×ÊÁÏºÍÏÖÓÐ×ÊÔ´Íê³É£¬Ò²ËãÊÇ¸öÔÓ½»³É¹û¡¾doge¡¿
  ¸ÐÐ»BÕ¾UPÖ÷£º@ÎÞ·ãRaclette¡¢@joÁ¼joÓ°x¡¢@Cyber_Mice
  ÏîÄ¿Áé¸ÐÀ´×Ô@ÎÞ·ãRaclette ÏÂÎ»»ú²¿·Ö³õÊ¼´úÂëÀ´×Ô@joÁ¼joÓ°x 
  ÏîÄ¿¸Ä½øË¼Â·¿É²Î¿¼@Cyber_Mice 
*/

//u8 t=0;	
float adcx;
float adcx10;

int flexADC;
float flexV=0.0;
float flexR=0.0;
float angle_1=0.0;
const float VCC =3.3; // Ä£¿é¹©µçµçÑ¹£¬ADC²Î¿¼µçÑ¹ÎªV
const float R_DIV =100000.0; // ·ÖÑ¹µç×èÎª100K¦¸
const float STRAIGHT_RESISTANCE =29089.0; // Æ½Ö±Ê±µÄµç×èÖµ  ÐèÐÞ¸Ä
const float BEND_RESISTANCE =48063.0; // 90¶ÈÍäÇúÊ±µÄµç×èÖµ  ÐèÐÞ¸Ä
long map(long x, long in_min, long in_max, long out_min, long out_max);
//short aacx,aacy,aacz;		//¼ÓËÙ¶È´«¸ÐÆ÷Ô­Ê¼Êý¾Ý
//short gyrox,gyroy,gyroz;	//ÍÓÂÝÒÇÔ­Ê¼Êý¾Ý
float angle=0;

//´«ËÍÊý¾Ý¸øÄäÃûËÄÖáÉÏÎ»»úÈí¼þ(V2.6°æ±¾)
//fun:¹¦ÄÜ×Ö. 0XA0~0XAF
//data:Êý¾Ý»º´æÇø,×î¶à28×Ö½Ú!!
//len:dataÇøÓÐÐ§Êý¾Ý¸öÊý
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//×î¶à28×Ö½ÚÊý¾Ý 
	send_buf[len+3]=0;	//Ð£ÑéÊýÖÃÁã
	send_buf[0]=0X88;	//Ö¡Í·
	send_buf[1]=fun;	//¹¦ÄÜ×Ö
	send_buf[2]=len;	//Êý¾Ý³¤¶È
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//¸´ÖÆÊý¾Ý
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//¼ÆËãÐ£ÑéºÍ	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//·¢ËÍÊý¾Ýµ½´®¿Ú1 
}
//data:Êý¾Ý»º´æÇø,×î¶à28×Ö½Ú!!
//len:dataÇøÓÐÐ§Êý¾Ý¸öÊý
void usart1_report(u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//×î¶à28×Ö½ÚÊý¾Ý 
//	send_buf[len+3]=0;	//Ð£ÑéÊýÖÃÁã
//	send_buf[0]=0X88;	//Ö¡Í·
	send_buf[len]=len;//Êý¾Ý³¤¶È
	send_buf[len+1]=0x0d;//Ö¡Î²1
	send_buf[len+2]=0x0a;//Ö¡Î²2
	
	for(i=0;i<len;i++)send_buf[i]=data[i];			//¸´ÖÆÊý¾Ý
	
//	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//¼ÆËãÐ£ÑéºÍ		
	for(i=0;i<len+3;i++)usart1_send_char(send_buf[i]);	//·¢ËÍÊý¾Ýµ½´®¿Ú1 
}
//·¢ËÍ¼ÓËÙ¶È´«¸ÐÆ÷Êý¾ÝºÍÍÓÂÝÒÇÊý¾Ý
//aacx,aacy,aacz:x,y,zÈý¸ö·½ÏòÉÏÃæµÄ¼ÓËÙ¶ÈÖµ
//gyrox,gyroy,gyroz:x,y,zÈý¸ö·½ÏòÉÏÃæµÄÍÓÂÝÒÇÖµ
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
//	usart1_niming_report(0XA1,tbuf,12);//×Ô¶¨ÒåÖ¡,0XA1
	usart1_report(tbuf,12);
}	
//Í¨¹ý´®¿Ú1ÉÏ±¨½áËãºóµÄ×ËÌ¬Êý¾Ý¸øµçÄÔ
//aacx,aacy,aacz:x,y,zÈý¸ö·½ÏòÉÏÃæµÄ¼ÓËÙ¶ÈÖµ
//gyrox,gyroy,gyroz:x,y,zÈý¸ö·½ÏòÉÏÃæµÄÍÓÂÝÒÇÖµ
//roll:ºá¹ö½Ç.µ¥Î»0.01¶È¡£ -18000 -> 18000 ¶ÔÓ¦ -180.00  ->  180.00¶È
//pitch:¸©Ñö½Ç.µ¥Î» 0.01¶È¡£-9000 - 9000 ¶ÔÓ¦ -90.00 -> 90.00 ¶È
//yaw:º½Ïò½Ç.µ¥Î»Îª0.1¶È 0 -> 3600  ¶ÔÓ¦ 0 -> 360.0¶È
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//Çå0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//·É¿ØÏÔÊ¾Ö¡,0XAF
}   

void show_init(void)
{
	OLED_ShowString(5,0, " ADC :    ",12,1);
	OLED_ShowString(5,10," Temp:    ",12,1);	
	OLED_ShowString(5,20," Pitch:   ",12,1);	
	OLED_ShowString(5,30," Roll:    ",12,1);	 
	OLED_ShowString(5,40," Yaw :    ",12,1);		
	OLED_Refresh();

}
void show(void)
{
		adcx=Get_Adc_Average(ADC_Channel_1,3);//¶ÁÈ¡»Ò¶È´«¸ÐÆ÷adÖµ
		adcx=(float)adcx/4096*33;
	  printf("dddddd\r\n");
	  printf("%d\r\n",adcx);
		data[1]=adcx;//Êý¾Ý°ü1Î» 
		OLED_ShowNum(5+48+8,0,adcx,2,12,1);	
//		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
//		{ 	
				temp=MPU_Get_Temperature();//µÃµ½ÎÂ¶ÈÖµ
				if(temp<0)
				{
					OLED_ShowString(5+48,10,"-",12,1);		//ÏÔÊ¾¸ººÅ
					temp=-temp;		//×ªÎªÕýÊý
				}
				else OLED_ShowString(5+48,10," ",12,1);		//È¥µô¸ººÅ 
				OLED_ShowNum(5+48+8,10,temp/100,2,12,1);		//ÏÔÊ¾ÕûÊý²¿·Ö	    
				OLED_ShowString(5+48+24,10,".",12,1);
				OLED_ShowNum(5+48+30,10,temp%10,1,12,1);		//ÏÔÊ¾Ð¡Êý²¿·Ö 
				
				temp=pitch*10;
				if(temp<0)
				{
					OLED_ShowString(5+48,20,"-",12,1);		//ÏÔÊ¾¸ººÅ
					temp=-temp;		//×ªÎªÕýÊý
				}else OLED_ShowString(5+48,20," ",12,1);		//È¥µô¸ººÅ 
				OLED_ShowNum(5+48+8,20,temp/10,2,12,1);		//ÏÔÊ¾ÕûÊý²¿·Ö	    
				OLED_ShowString(5+48+24,20,".",12,1);
				OLED_ShowNum(5+48+30,20,temp%10,1,12,1);		//ÏÔÊ¾Ð¡Êý²¿·Ö 
				
				temp=roll*10;
				if(temp<0)
				{
					OLED_ShowString(5+48,30,"-",12,1);		//ÏÔÊ¾¸ººÅ
					temp=-temp;		//×ªÎªÕýÊý
				}else OLED_ShowString(5+48,30," ",12,1);		//È¥µô¸ººÅ 
				OLED_ShowNum(5+48+8,30,temp/10,2,12,1);		//ÏÔÊ¾ÕûÊý²¿·Ö	    
				OLED_ShowString(5+48+24,30,".",12,1);
				OLED_ShowNum(5+48+30,30,temp%10,1,12,1);		//ÏÔÊ¾Ð¡Êý²¿·Ö 
									
				temp=yaw*10;
				if(temp<0)
				{
					OLED_ShowString(5+48,40,"-",12,1);		//ÏÔÊ¾¸ººÅ
					temp=-temp;		//×ªÎªÕýÊý
				}else OLED_ShowString(5+48,40," ",12,1);		//È¥µô¸ººÅ 
				OLED_ShowNum(5+48+8,40,temp/10,2,12,1);		//ÏÔÊ¾ÕûÊý²¿·Ö	 
				OLED_ShowString(5+48+24,40,".",12,1);				
				OLED_ShowNum(5+48+30,40,temp%10,1,12,1);		//ÏÔÊ¾Ð¡Êý²¿·Ö  
				LED=!LED;//LEDÉÁË¸
			OLED_Refresh();	
//	}
}
/*
up±¾À´ÏëÔÚÉÏÎ»»úÓÃÁ½¸ömpu6050Ä£¿é´«½Ç¶ÈÊý¾Ý£¬
Ò»¸öÔÚÊÖ±³ÉÏ£¬Ò»¸öÔÚÐ¡±ÛÉÏ£¬¾Í²»ÓÃpitchÍ¬Ê±¿ØÖÆ3¡¢4ºÅ¶æ»ú
ÄÎºÎ¹Ù·½¿âÖ»ÄÜÕë¶ÔÄÇ1¸öIICµØÖ·µÄÄ£¿é½øÐÐÅäÖÃ£¬
ÏëÔÚÍ¬Ò»¸öIIC×ÜÏßÉÏÔÙ¼ÓÒ»¸ö²»Í¬µØÖ·µÄmpu6050¾ÍÏàµ±Óë
ÖØÐÂÐ´Ò»±émpuiic¡¢inv_mpu¡¢inv_mpu_dmp_motionÎÄ¼þ£¬
ÏÖ½×¶Î±¾ÈËË®Æ½ÓÐÏÞ£¬³¢ÊÔ¹ýºóÊµÔÚÐÐ²»Í¨T.T¡£
tbËÑ GY-25Ä£¿é ½áºÏ@Cyber_MiceµÄ·½°¸£¬
¿ÉÒÔÊµÏÖÕû¸öÊÖ±ÛµÄ×ËÌ¬¿ØÖÆ£¬¸ÐÐËÈ¤¿ÉÒÔ²Î¿¼¡£
¹ØÓÚÏîÄ¿µÄÈÎºÎÎÊÌâ»¶Ó­ÔÚÔ­ÊÓÆµÏÂÌÖÂÛ£¬ÏòÉñÍ¨¹ã´óµÄBÓÑÃÇÇóÖú^.^¡£
*/
int main(void)
 {     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	 	//ÑÓÊ±º¯Êý³õÊ¼»¯	  
	uart_init(9600);	 		//´®¿Ú³õÊ¼»¯Îª9600
	LED_Init();		  			//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú
	Adc_Init();
	Dac1_Init();
//	KEY_Init();					//³õÊ¼»¯°´¼ü
	MPU_Init();					//³õÊ¼»¯MPU6050
	OLED_Init();
	OLED_ColorTurn(0);//0Õý³£ÏÔÊ¾£¬1 ·´É«ÏÔÊ¾
	OLED_DisplayTurn(0);//0Õý³£ÏÔÊ¾ 1 ÆÁÄ»·­×ªÏÔÊ¾
	delay_ms(100);//µÈ´ýÄ£¿é³õÊ¼»¯Íê³É
	OLED_DisPlay_On();
	while(mpu_dmp_init())
 	{
		OLED_ShowString(8,16,"MPU6050 Error",16,1);
		OLED_Refresh();
 		delay_ms(500);
		OLED_Clear();
	} 
	show_init();
	delay_ms(100);	
	TIM4_Int_Init(3999,719);//40ms
 	while(1)
	{	
//		adcx=Get_Adc_Average(ADC_Channel_1,3);//¶ÁÈ¡»Ò¶È´«¸ÐÆ÷adÖµ
//		adcx=(float)adcx/4096*33;
//	  printf("dddddd\r\n");
//	  printf("ggggg%f\r\n",adcx);
//		data[1]=adcx;//Êý¾Ý°ü1Î»	
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

//    printf("Yaw:%f degrees\r\n",yaw);

		
	}
	
}
 long map(long x, long in_min, long in_max, long out_min, long out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
