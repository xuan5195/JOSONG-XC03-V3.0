#include "stm32f10x.h"		/* 如果要用ST的固件库，必须包含这个文件 */
#include <stdio.h>			/* 因为用到了printf函数，所以必须包含这个文件 */
#include "bsp_timer.h"		/* systick定时器模块 */
#include "bsp_FireData.h"	/**/
#include "bsp_Input.h"		/**/
#include "bsp_Show.h"		/**/

u8 Tab[16] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
unsigned char FaulArray[15];
unsigned char TimeCableArray[5];


void Display_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 

 }
 /****************************************************************************
* 名    称：void Display_Show(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void Display_Show(u8 Display_ShowDate1,u8 Display_ShowDate2)
{
	u8 i;
	u16 temp;
	temp = Display_ShowDate1;
	for(i=0;i<8;i++)
	{
		//Led_SCLK_OFF();
		if((temp&0x80) == 0x80)	{	Led_DAT_ON(); }
		else  				{	Led_DAT_OFF();}
		Display_ShowDate1 = Display_ShowDate1 << 1;
		Led_SCLK_OFF();
		Led_SCLK_ON();
	}
	
	temp = Display_ShowDate2;
	for(i=0;i<8;i++)
	{
		//Led_SCLK_OFF();
		if((temp&0x80) == 0x80)	{	Led_DAT_ON(); }
		else  				{	Led_DAT_OFF();}
		Display_ShowDate1 = Display_ShowDate1 << 1;
		Led_SCLK_OFF();
		Led_SCLK_ON();
	}
}

////LED显示函数
void LED_Show(u16 Led_ShowDate)
{
	u8 i;
	u16 temp;
	for(i=0;i<16;i++)
	{
		temp = Led_ShowDate;
		temp &= 0x8000;
		if(temp == 0x8000){	Led_DAT_ON(); }
		else  			{	Led_DAT_OFF();}
		Led_ShowDate = Led_ShowDate << 1;
		Led_SCLK_OFF();
		Led_SCLK_ON();
	}
}

void Show(u8 FirstDat,u8 SecondDat)
{
	u16 Temp = 0x0000;
	switch (FirstDat)
	{
		case 0x00://无故障
			Temp = ( (Tab[0])|(Tab[0]<<8) );
			break;
		case 0xDD://关显示
			Temp = 0xFFFF;
			break;
		case 0x0A://点火头故障 
			Temp = ( (Tab[0x0A])|(Tab[SecondDat]<<8) );
			break;
		case 0x0C://探测器故障
			Temp = ( (Tab[0x0C])|(Tab[SecondDat]<<8) );
			break;
		case 0x0D://探测器报警
			Temp = ( (Tab[0x0D])|(Tab[SecondDat]<<8) );
			break;
		case 0x0E://装置内部故障   
			//E1、E2：分别代表总阀电点火头故障  
			//E5、E6：分别代表总阀压力检测开关故障
			Temp = ( (Tab[0x0E])|(Tab[SecondDat]<<8) );
			break;
		case 0x0F://启动输入端口
			Temp = ( (Tab[0x0F])|(Tab[SecondDat]<<8) );
			break;
		default:
			break;
	}
	LED_Show(Temp);	
}

void WriteFaulArray(u8 Detection_TempFaultData,u8 Detection_FireFaultData,u8 Detection_PressureData)
{
	u8 i;
	u8 temp;
	//探测器故障
	temp = Detection_TempFaultData;
	for(i=0;i<5;i++)//FaulArray[0] ~ FaulArray[4]
	{
		if( (temp&0x01) == 0x01 )
		{
			FaulArray[i] = 0xAA;
		}
		else
		{
			FaulArray[i] = 0x55;
		}
		temp = temp >> 1;
	}
	
	//点火头故障
	temp = Detection_FireFaultData;
	for(i=0;i<6;i++)//FaulArray[5] ~ FaulArray[9]、 FaulArray[10]
	{
		if( (temp&0x01) == 0x01 )
		{
			FaulArray[i+5] = 0xAA;
		}
		else
		{
			FaulArray[i+5] = 0x55;
		}
		temp = temp >> 1;
	}
	//主  压力异常
	if(Detection_PressureData == 0xAA)	{FaulArray[11] = 0xAA;}
	else 	{FaulArray[11] = 0x55;}
}
void WriteTimeCableNo(u8 Detection_TimeCableData)
{
	u8 i;
	u16 temp;
	//探测器报警
	temp = Detection_TimeCableData;
	for(i=0;i<5;i++)//TimeCableArray[0] ~ TimeCableArray[5]
	{
		if( (temp&0x01) == 0x01 )
		{
			TimeCableArray[i] = 0xAA;//异常
		}
		else
		{
			TimeCableArray[i] = 0x55;
		}
		temp = temp >> 1;
	}
}



