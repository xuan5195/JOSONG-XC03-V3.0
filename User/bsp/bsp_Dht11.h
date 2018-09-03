#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "stm32f10x.h"

//IO方向设置
#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

//IO操作函数											   
#define	DHT11_DQ_High()		GPIO_SetBits(GPIOB, GPIO_Pin_9)		//数据端口	PB9
#define	DHT11_DQ_Low()		GPIO_ResetBits(GPIOB, GPIO_Pin_9)	//数据端口	PB9
#define	DHT11_DQ_IN()     	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//GPIOB->IDR & GPIO_Pin_9 			//数据端口	PB9


void BspDht11_Config(void);
uint8_t DHT11_Read_Bit(void);		 
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t *buf);    

#endif
