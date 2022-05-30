#ifndef __IIC_H
#define __IIC_H
#include "sys.h"

//IO操作函数	 
#define IIC_SCL    PBout(3) //SCL
#define IIC_SDA    PBout(4) //SDA	 
#define READ_SDA   PBin(4)  //输入SDA 

#define Slave_Addr 0x36
#define Write_Bit 0
#define Read_Bit 1
#define Angle_Hight_Register_Addr 0x0C
#define Angle_Low_Register_Addr 0x0D

//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<21;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<21;}
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void AS5600_Init(void);
u16 AS5600_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
float Get_Angle(void);
#endif


