#ifndef __BSP_CD4067_H
#define __BSP_CD4067_H	

#include "stm32f10x.h"		

#define Iutput_CD4067A_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_9) 	   		
#define Iutput_CD4067A_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_9) 	   		

#define Iutput_CD4067B_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_8) 	   		
#define Iutput_CD4067B_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_8)	   		

#define Iutput_CD4067C_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_7)  	   		
#define Iutput_CD4067C_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_7) 	   		

#define Iutput_CD4067D_High()   	GPIO_SetBits(GPIOC, GPIO_Pin_6) 	   		
#define Iutput_CD4067D_Low()  		GPIO_ResetBits(GPIOC, GPIO_Pin_6)   		

#define Read_InputData()			GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)

#define RDKMA1  10
#define RDKMA2  11
#define RDKMA3  12
#define RDKMB1  13
#define RDKMB2  14
#define RDKMB3  15

void BspInput_CD4067_Config(void); 
uint8_t Read_InputDevDat(uint8_t uAreaDat); 
uint8_t Read_Optocoupler(uint8_t uAreaDat);
		 				    
#endif