#ifndef __BSP_CD4051_H
#define __BSP_CD4051_H	

#include "stm32f10x.h"		

//#define IV_A	2
//#define IV_B	1
//#define IV_C	0
//#define II_A	0
//#define II_B	1
//#define II_C	2

enum
{
	IV_A=0,		
	IV_B,		
	IV_C		
};
enum
{
	II_A=0,		
	II_B,		
	II_C		
};//三相电流输入选择
#define Iutput_CSCD4051A_High()   	GPIO_SetBits(GPIOB, GPIO_Pin_6) 	   		
#define Iutput_CSCD4051A_Low()  	GPIO_ResetBits(GPIOB, GPIO_Pin_6) 	   		

#define Iutput_CSCD4051B_High()   	GPIO_SetBits(GPIOB, GPIO_Pin_7) 	   		
#define Iutput_CSCD4051B_Low()  	GPIO_ResetBits(GPIOB, GPIO_Pin_7)	

//三相电压输入选择
#define Iutput_IICSCD4051A_High()   GPIO_SetBits(GPIOA, GPIO_Pin_15) 	   		
#define Iutput_IICSCD4051A_Low()  	GPIO_ResetBits(GPIOA, GPIO_Pin_15) 	   		

#define Iutput_IICSCD4051B_High()   GPIO_SetBits(GPIOD, GPIO_Pin_2) 	   		
#define Iutput_IICSCD4051B_Low()  	GPIO_ResetBits(GPIOD, GPIO_Pin_2)	

//用于220V数据输入检测
#define Iutput_KPCD4051A_High()   	GPIO_SetBits(GPIOB, GPIO_Pin_12)		
#define Iutput_KPCD4051A_Low()  	GPIO_ResetBits(GPIOB, GPIO_Pin_12) 	   		

#define Iutput_KPCD4051B_High()   	GPIO_SetBits(GPIOB, GPIO_Pin_13) 	   		
#define Iutput_KPCD4051B_Low()  	GPIO_ResetBits(GPIOB, GPIO_Pin_13)	

#define Iutput_KPCD4051C_High()   	GPIO_SetBits(GPIOB, GPIO_Pin_14) 	   		
#define Iutput_KPCD4051C_Low()  	GPIO_ResetBits(GPIOB, GPIO_Pin_14)

#define Read_InputData_KP()			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) 

enum 
{
	Channal_A =1,
	Channal_B,
	Channal_C,
	Channal_Max,
};

void BspInput_CD4051_Config(void);
void SetInput_CSCD4051Switch(uint8_t uSetDat);//三相电压采集选择口 在CS5463中使用
void SetInput_IICD4051Switch(uint8_t uSetDat);//三相电流采集选择口 在CS5463中使用
u8 Read_Input_KPDevDat(uint8_t uAreaDat) ;
u8 CD4051Read_Optocoupler(uint8_t uAreaDat);
		 				    
#endif
