#include "stm32f10x.h"		/* 如果要用ST的固件库，必须包含这个文件 */
#include <stdio.h>			/* 因为用到了printf函数，所以必须包含这个文件 */
#include "bsp_key.h"		/**/
#include "bsp_timer.h"

KEY_ENUM KEY_ID;
/*
*********************************************************************************************************
* 函数名称：BspKey_Delay
* 功能说明: 按键延时处理
* 形  	参：无
* 返 回 值：无
*********************************************************************************************************
*/
void BspKey_Delay(u8 z) 
{ 
	bsp_DelayMS(z);
}

/*
*********************************************************************************************************
* 函数名称：BspKey_Config
* 功能说明: 按键GPIO口初始化 独立按键
* 形  	参：无
* 返 回 值：无
*********************************************************************************************************
*/
void BspKey_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//按键 采用上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
* 函数名称：BspKey_Read
* 功能说明: 扫描读按键值
* 形  	参：无
* 返 回 值：按键值
*********************************************************************************************************
*/
u8 BspKey_Read(void)
{	 
	static u8 key_up=1;//按键按松开标志
	if(1)key_up=1;  //支持连按		  
	if(key_up&&(Key1_Dat()==0||Key2_Dat()==0||Key3_Dat()==0||Key4_Dat()==0))
	{
		BspKey_Delay(10);//去抖动 
		key_up=0;
		if(Key1_Dat()==0)		return KEY_1;
		else if(Key2_Dat()==0)	return KEY_2;
		else if(Key3_Dat()==0)	return KEY_3;
		else if(Key4_Dat()==0)	return KEY_4;
	}
	else if((Key1_Dat()==1)&&(Key2_Dat()==1)&&(Key3_Dat()==1)&&(Key4_Dat()==1))	key_up=1; 	    
 	return KEY_NONE;// 无按键按下
}



