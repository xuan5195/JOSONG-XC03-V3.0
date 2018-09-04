#ifndef __BSP_HC595_H
#define __BSP_HC595_H

#include "stm32f10x.h"

#define Led_DAT_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_12)   	   		//Lcd_DATA
#define Led_DAT_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_12)  	   	//

#define Led_SRCLK_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_11)   	   		//Lcd_SCLK
#define Led_SRCLK_Low()  	GPIO_ResetBits(GPIOC, GPIO_Pin_11) 	   		//

#define Led_RCLK_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_10)   	   		//Lcd_SCLK
#define Led_RCLK_Low()  	GPIO_ResetBits(GPIOC, GPIO_Pin_10) 	   		//

#define HC595_E1_OFF()   	GPIO_SetBits(GPIOA, GPIO_Pin_2)   	   		//E1
#define HC595_E1_ON()  		GPIO_ResetBits(GPIOA, GPIO_Pin_2) 	   		//

#define HC595_E2_OFF()   	GPIO_SetBits(GPIOA, GPIO_Pin_3)   	   		//E2
#define HC595_E2_ON()  		GPIO_ResetBits(GPIOA, GPIO_Pin_3) 	   		//

#define AKM1RUN		0x0001
#define AKM2RUN		0x0002
#define AKM3RUN		0x0004
#define BKM1RUN		0x0008
#define BKM2RUN		0x0010
#define BKM3RUN		0x0020
#define AHUMIDITY	0x0800    	//除湿
//#define BHUMIDITY	0x1000		//备用除湿
#define ALARMOUT1	0x0100		//无源应答
#define ALARMOUT2	0x0200		//有源应答
#define ALARMOUT3   0x0400		//三音报警器输出
#define ALARMALL    0x0FFF		


void bsp_HC595_Config(void);
void KMON_Show(u16 ShowDat);
void KMOFF_Show(u16 ShowDat);
void KMOutUpdat(void);
		 				    
#endif
