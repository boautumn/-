#ifndef _PWM_H_
#define _PWM_H_

#define SERVO1 PCout(10)			//控制爪子的开合
#define SERVO2 PCout(11)			//上下爪子的舵机
#define SERVO3 PCout(12)
#define SERVO4 PDout(2)
#define SERVO5 PBout(5)
#define SERVO6 PBout(8)

#define keypwm3 PBin(10)
#define keypwm4 PBin(11)

typedef unsigned char	u8, U8, uint8, UINT8, BYTE;
typedef unsigned short	u16, U16, uint16, UINT16, WORD;
typedef unsigned char	bool, BOOL;

#define TRUE   1
#define FALSE  0
#define NULL   0

extern uint16 ServoPwmDutySet[];

extern char ServoPwmDutyHaveChange;
extern u32 syst;

void ServoSetPluseAndTime(uint8 id,uint16 p,uint16 time);
void ServoPwmDutyCompare(void);//脉宽变化比较及速度控制
void InitPWM(void);

#endif

