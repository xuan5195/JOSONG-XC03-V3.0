#ifndef __BSP_TM1639_H
#define __BSP_TM1639_H

#include "stm32f10x.h"

#define TM1639_DIO_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define TM1639_DIO_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_1) 	   		//
#define TM1639_CLK_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_2)  	   		//
#define TM1639_CLK_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_2) 	   		//
#define TM1639_STB_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_3)  	   		//
#define TM1639_STB_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_3) 	   		//

#define DIG0	0xC0
#define DIG1	0xC2
#define DIG2	0xC4
#define DIG3	0xC6
#define DIG4	0xC8
#define DIG5	0xCA
#define DIG6	0xCC
#define DIG7	0xCE

#define LEVEL_OFF	0x80
#define LEVEL_1		0x88
#define LEVEL_2		0x89
#define LEVEL_4		0x8A
#define LEVEL_10	0x8B
#define LEVEL_11	0x8C
#define LEVEL_12	0x8D
#define LEVEL_13	0x8E
#define LEVEL_14	0x8F

#define LEVEL_USE	LEVEL_4	//目前使用亮度 

#define MD_WRITE	0x40
#define MD_AUTO		MD_WRITE
#define MD_NORMAL	MD_WRITE


void BspTm1639_Config(void); 
void BspTm1639_Writebyte(uint8_t datx);
void BspTM1639_ClearALL(void);	//清显示数据 
void BspTm1639_Show(uint8_t ShowMode,uint16_t ShowDate);
void BspTm1639_ShowParam(uint8_t ShowFlag,uint8_t ShowMode,uint16_t ShowDate);

#endif
