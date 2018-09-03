#ifndef __BSP_INPUT_H
#define __BSP_INPUT_H	


#define Pressure1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)

#define KeyInput  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

void Input_GPIO_Config(void);			//≥ı ºªØ
u8 Read_PressureData(void);
		 				    
#endif
