#include "stm32f10x.h"		/* 如果要用ST的固件库，必须包含这个文件 */
#include <stdio.h>			/* 因为用到了printf函数，所以必须包含这个文件 */
#include "bsp_timer.h"		/* systick定时器模块 */
#include "bsp_FireData.h"	/**/
#include "bsp_Input.h"		/**/

#ifdef PressureOpen
  uint8_t PressureDat	= PressureOpen; 
#elif defined PressureClose
  uint8_t PressureDat	= PressureClose; 
#else
  uint8_t PressureDat	= 0; 
#endif

void Input_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//按键 采用上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

//读取数据编码
u8 Read_PressureData(void)
{
	u8 temp=0;
	if(Pressure1 == PressureDat)	{temp = 0xDD;}
	else	{temp = 0xAA;}//异常
	return temp;
}

