#include "stm32f10x.h"
#include <stdio.h>
#include "bsp_tm1639.h"
#include "bsp.h"

uint8_t OldShowMode=0;	//缓存数据显示模式，用于清屏
uint32_t OldShowDate=0;	//缓存数据显示模式，防止数据重复更新占用系统时间
static uint8_t table[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//extern uint8_t Logic_ADD;	//逻辑地址
//extern uint8_t gErrorShow;	//异常显示代码 在服务器未更新前显示使用，这样不会出现E000

void BspTm1639_Delay(__IO uint16_t z) 
{ 
	__IO uint8_t x;
	while(z--)
	{
		for(x=100;x>0;x--);
	}
}

void BspTm1639_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 

	BspTM1639_ClearALL();	//上电清显示
}

void BspTm1639_Writebyte(uint8_t datx)
{
 	uint8_t i;
	TM1639_STB_Low();
	for(i=0;i<8;i++)
	{
		TM1639_CLK_Low();	 
		if((datx&0x01)!=0)	TM1639_DIO_High();	 
		else				TM1639_DIO_Low();
		BspTm1639_Delay(1);
		TM1639_CLK_High();
		datx = datx >> 1;
		BspTm1639_Delay(1);
	}
}

void BspTm1639_Show(uint8_t ShowMode,uint32_t ShowDate)
{
 	uint8_t i,udat;
 	if((OldShowMode==ShowMode)&&(OldShowDate==ShowDate));	//数据、模式相同不刷新
 	else
 	{
		if(OldShowMode!=ShowMode)	BspTM1639_ClearALL();	//显示模式不同时，清显示
 		OldShowMode = ShowMode;		//保存数据  防止重复更新
		OldShowDate = ShowDate;		//		
		BspTm1639_Writebyte(MD_AUTO);	//设置为地址自动加1写显示数据
		TM1639_STB_High();
		if(ShowMode==0x02)
		{
			BspTm1639_Writebyte(DIG0);
			for(i=0;i<16;i++)	BspTm1639_Writebyte(0x00);
		}
		else if(ShowMode==0x01)	//上电初始化显示值
		{
			BspTm1639_Writebyte(DIG0);
			BspTm1639_Writebyte(0x0F);	BspTm1639_Writebyte(0x00);
			for(i=0;i<2;i++)
			{
				BspTm1639_Writebyte(0x09);	BspTm1639_Writebyte(0x00);
			}
			BspTm1639_Writebyte(0x09);	BspTm1639_Writebyte(0x03);		
		}
		else if(ShowMode==0x06)	//显示代码，用于安装检测使用
		{
			BspTm1639_Writebyte(DIG0);
			for(i=0;i<3;i++)
			{
				BspTm1639_Writebyte(0x00);	BspTm1639_Writebyte(0x04);
			}
			//VERSION 显示固件版本号
			udat = VERSION&0x0F;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			udat = (VERSION&0xF0)>>4;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			for(i=0;i<3;i++)
			{
				BspTm1639_Writebyte(0x00);	BspTm1639_Writebyte(0x04);
			}
		}
		else if(ShowMode==0x03)
		{
			BspTm1639_Writebyte(DIG0);
			BspTm1639_Writebyte(table[ShowDate%10]);		BspTm1639_Writebyte(table[ShowDate%10]>>4);
			BspTm1639_Writebyte(table[ShowDate/10%10]);		BspTm1639_Writebyte(table[ShowDate/10%10]>>4);
			BspTm1639_Writebyte(table[ShowDate/100]);		BspTm1639_Writebyte(table[ShowDate/100]>>4);
			BspTm1639_Writebyte(table[0]);					BspTm1639_Writebyte(table[0]>>4|0x08);
			BspTm1639_Writebyte(0x00);						BspTm1639_Writebyte(0x00);//DIG4
			BspTm1639_Writebyte(0x00);						BspTm1639_Writebyte(0x00);//DIG5
			BspTm1639_Writebyte(0x00);						BspTm1639_Writebyte(0x00);//DIG4
			BspTm1639_Writebyte(0x00);						BspTm1639_Writebyte(0x00);//DIG5
		}
		else if(ShowMode==0x04)
		{
			BspTm1639_Writebyte(DIG0);
			ShowDate = ShowDate & 0x00FFFFFF;	//清高位，高位为版本号不是金额
			if(ShowDate<1000000)
			{
			    udat = ShowDate%10;			BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			    udat = ShowDate/10%10;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			    udat = ShowDate/100%10;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			    udat = ShowDate/1000%10;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4|0x08);//个位
			    udat = ShowDate/10000%10;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);//十位
			    udat = ShowDate/100000%10;	BspTm1639_Writebyte(table[udat]);   BspTm1639_Writebyte(table[udat]>>4);//百位       
			}
			else
			{
			    udat = ShowDate/10%10;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			    udat = ShowDate/100%10;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			    udat = ShowDate/1000%10;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4|0x08);//个位
			    udat = ShowDate/10000%10;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);//十位
			    udat = ShowDate/100000%10;	BspTm1639_Writebyte(table[udat]);   BspTm1639_Writebyte(table[udat]>>4);//百位       
			    udat = ShowDate/1000000%10;	BspTm1639_Writebyte(table[udat]);   BspTm1639_Writebyte(table[udat]>>4);//千位       
			}
			BspTm1639_Writebyte(0x00);	BspTm1639_Writebyte(0x00); //DIG6
			BspTm1639_Writebyte(0x00);	BspTm1639_Writebyte(0x00); //DIG7
		}
		else if(ShowMode==0x05)
		{
			BspTm1639_Writebyte(DIG0);
			udat = 0x00;				BspTm1639_Writebyte(udat);			BspTm1639_Writebyte(udat);
			udat = 0x00;				BspTm1639_Writebyte(udat);			BspTm1639_Writebyte(udat);
			udat = 0x00;				BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			udat = 0x00;				BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			udat = 0x00;				BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			udat = 0x0E;				BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
			udat = 0x00;				BspTm1639_Writebyte(udat);			BspTm1639_Writebyte(udat);//DIG6
			udat = 0x00;				BspTm1639_Writebyte(udat);			BspTm1639_Writebyte(udat);//DIG7
		}
		TM1639_STB_High();
		BspTm1639_Writebyte(LEVEL_USE);
		TM1639_STB_High();
 	}

}

void BspTm1639_ShowSNDat(uint8_t ShowMode,uint8_t *ShowDate)
{
 	uint8_t udat;
	if(OldShowMode!=ShowMode)	BspTM1639_ClearALL();	//显示模式不同时，清显示
	BspTm1639_Writebyte(MD_AUTO);	//设置为地址自动加1写显示数据
	TM1639_STB_High();
	if(ShowMode==0x11)		//显示卡机SN
	{
		BspTm1639_Writebyte(DIG0);
		udat = (*(ShowDate+3))&0x0F;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = ((*(ShowDate+3))&0xF0)>>4;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = (*(ShowDate+2))&0x0F;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = ((*(ShowDate+2))&0xF0)>>4;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = (*(ShowDate+1))&0x0F;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = ((*(ShowDate+1))&0xF0)>>4;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = (*(ShowDate+0))&0x0F;		BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
		udat = ((*(ShowDate+0))&0xF0)>>4;	BspTm1639_Writebyte(table[udat]);	BspTm1639_Writebyte(table[udat]>>4);
	}
	TM1639_STB_High();
	BspTm1639_Writebyte(LEVEL_USE);
	TM1639_STB_High();
	OldShowMode = ShowMode;		//保存数据  防止重复更新
}


void BspTM1639_ClearALL(void)
{
	u8 i;
	TM1639_STB_High();
	TM1639_CLK_High();
	TM1639_DIO_High();
	BspTm1639_Writebyte(0x40);	//数据命令设置
	TM1639_STB_High();
	BspTm1639_Writebyte(DIG0);	//显示地址 起始地址
	for(i=0;i<16;i++)	BspTm1639_Writebyte(0x00);
	TM1639_STB_High();
	BspTm1639_Writebyte(LEVEL_OFF);	//显示控制命令
	TM1639_STB_High();
}

