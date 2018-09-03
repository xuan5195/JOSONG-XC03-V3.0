#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_led.h"

void BspLed_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//JTAG-DP Disabled and SW-DP Enabled，PB3、PB4才能使用

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	LEDALL_ON();
}

void LEDALL_ON(void)
{
	LED1_ON();	LED2_ON();	LED3_ON();	LED4_ON();	LED5_ON();
	LED6_ON();	LED7_ON();	LED8_ON();	LED9_ON();	LED10_ON();
	LED11_ON();	LED12_ON();	LED13_ON();	LED14_ON();
}
void LEDALL_OFF(void)
{
	LED1_OFF();	LED2_OFF();	LED3_OFF();	LED4_OFF();	LED5_OFF();
	LED6_OFF();	LED7_OFF();	LED8_OFF();	LED9_OFF();	LED10_OFF();
	LED11_OFF();LED12_OFF();LED13_OFF();LED14_OFF();
}

void LEDShow(uint8_t _uDat1,uint8_t _uDat2)
{
	if( (_uDat1&0x80) == 0x80 )	LED5_ON();	//主一故障
	else						LED5_OFF();
	if( (_uDat1&0x40) == 0x40 )	LED6_ON();	//主二故障
	else						LED6_OFF();
	if( (_uDat1&0x20) == 0x20 )	LED7_ON();	//主一动力电
	else						LED7_OFF();
	if( (_uDat1&0x10) == 0x10 )	LED8_ON();	//主二动力电
	else						LED8_OFF();

	if( (_uDat1&0x08) == 0x08 )	LED4_ON();	//主一泵动作
	else						LED4_OFF();
	if( (_uDat1&0x04) == 0x04 )	LED9_ON();	//主二泵动作
	else						LED9_OFF();
	if( (_uDat1&0x02) == 0x02 )	LED2_ON();	//远控一启
	else						LED2_OFF();
	if( (_uDat1&0x01) == 0x01 )	LED3_ON();	//远控二启
	else						LED3_OFF();

	if( (_uDat2&0x04) == 0x04 )	LED12_ON();	//手动
	else						LED12_OFF();
	if( (_uDat2&0x02) == 0x02 )	LED11_ON();	//主一备二
	else						LED11_OFF();
	if( (_uDat2&0x01) == 0x01 )	LED13_ON();	//主二备一
	else						LED13_OFF();
	
}

