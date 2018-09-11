#include "stm32f10x.h"
#include <stdio.h>
#include "bsp_tm1639.h"
#include "bsp.h"

uint8_t OldShowMode=0;	//缓存数据显示模式，用于清屏
uint32_t OldShowDate=0;	//缓存数据显示模式，防止数据重复更新占用系统时间
const uint8_t table[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
const uint8_t Table_S[] = {0x77,0x7C,0x39,0x58,0x5E,0x79,0x71,0x3D,	//AbCcdEFG
                           0x76,0x74,0x0F,0x0E,0x75,0x38,0x37,0x54,	//HhIJKLMn
                           0x5C,0x73,0x67,0x31,0x49,0x78,		    //oPqrSt
                           0x3E,0x1C,0x7E,0x64,0x6E,0x5A};    		//UvWXYZ

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

void BspTm1639_Show(uint8_t ShowMode,uint16_t ShowDate)
{
 	uint8_t i;
 	if((OldShowMode==ShowMode)&&(OldShowDate==ShowDate));	//数据、模式相同不刷新
 	else
 	{
		if(OldShowMode!=ShowMode)	BspTM1639_ClearALL();	//显示模式不同时，清显示
 		OldShowMode = ShowMode;		//保存数据  防止重复更新
		OldShowDate = ShowDate;		//		
		BspTm1639_Writebyte(MD_AUTO);	//设置为地址自动加1写显示数据
		TM1639_STB_High();
        if(ShowMode==0xA1)	//上电初始化显示值
		{
			BspTm1639_Writebyte(DIG0);
			BspTm1639_Writebyte(0x0F);	BspTm1639_Writebyte(0x00);
			for(i=0;i<2;i++)
			{
				BspTm1639_Writebyte(0x09);	BspTm1639_Writebyte(0x00);
			}
			BspTm1639_Writebyte(0x09);	BspTm1639_Writebyte(0x03);		
		}
		else    //显示代码
		{
			BspTm1639_Writebyte(DIG0);
			BspTm1639_Writebyte(table[ShowDate%10]);		BspTm1639_Writebyte(table[ShowDate%10]>>4);
			BspTm1639_Writebyte(table[ShowDate/10%10]);		BspTm1639_Writebyte(table[ShowDate/10%10]>>4);
			BspTm1639_Writebyte(table[ShowDate/100]);		BspTm1639_Writebyte(table[ShowDate/100]>>4);
            if(ShowMode==0x00)      {BspTm1639_Writebyte(Table_S[22]); BspTm1639_Writebyte(Table_S[22]>>4); }  //电压U
            else if(ShowMode==0x01) {BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4); }  //电流A
            else if(ShowMode==0x02) {BspTm1639_Writebyte(Table_S[ 2]); BspTm1639_Writebyte(Table_S[ 2]>>4); }  //温度C
            else if(ShowMode==0x03) {BspTm1639_Writebyte(Table_S[ 8]); BspTm1639_Writebyte(Table_S[ 8]>>4); }  //湿度H
            else if(ShowMode==0x04) {BspTm1639_Writebyte(Table_S[17]); BspTm1639_Writebyte(Table_S[17]>>4); }  //压力P
            else if(ShowMode==0x05) {BspTm1639_Writebyte(Table_S[13]); BspTm1639_Writebyte(Table_S[13]>>4); }  //流量L
		}
		TM1639_STB_High();
		BspTm1639_Writebyte(LEVEL_USE);
		TM1639_STB_High();
 	}

}

void BspTm1639_ShowParam(uint8_t ShowFlag,uint8_t ShowMode,uint16_t ShowDate)
{
    if(ShowFlag==0) //显示
    {
        if((OldShowMode==ShowMode)&&(OldShowDate==ShowDate));	//数据、模式相同不刷新
        else
        {
            if(OldShowMode!=ShowMode)	BspTM1639_ClearALL();	//显示模式不同时，清显示
            OldShowMode = ShowMode;		//保存数据  防止重复更新
            OldShowDate = ShowDate;		//		
            BspTm1639_Writebyte(MD_AUTO);	//设置为地址自动加1写显示数据
            TM1639_STB_High();
            BspTm1639_Writebyte(DIG0);
            BspTm1639_Writebyte(table[ShowDate%10]);		BspTm1639_Writebyte(table[ShowDate%10]>>4);
            BspTm1639_Writebyte(table[ShowDate/10%10]);		BspTm1639_Writebyte(table[ShowDate/10%10]>>4);
            if((ShowMode==Menu_U)||(ShowMode==Menu_A))
            {
                BspTm1639_Writebyte(table[ShowDate/100]);		BspTm1639_Writebyte(table[ShowDate/100]>>4);
            }
ShowBack:
            switch (ShowMode)
            {
                case Menu_U:
                    BspTm1639_Writebyte(Table_S[22]); BspTm1639_Writebyte(Table_S[22]>>4);//U
                    break;
                case Menu_A:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    break;
                case Menu_Ua:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    BspTm1639_Writebyte(Table_S[22]); BspTm1639_Writebyte(Table_S[22]>>4);//U
                    break;
                case Menu_Ub:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[22]); BspTm1639_Writebyte(Table_S[22]>>4);//U
                    break;
                case Menu_Aa:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    break;
                case Menu_Ab:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    break;
                case Menu_Ac:
                    BspTm1639_Writebyte(Table_S[ 3]); BspTm1639_Writebyte(Table_S[ 3]>>4);//C
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    break;
                case Menu_Ca:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//A
                    BspTm1639_Writebyte(Table_S[ 2]); BspTm1639_Writebyte(Table_S[ 2]>>4);//C
                    break;
                case Menu_Cb:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[ 2]); BspTm1639_Writebyte(Table_S[ 2]>>4);//C
                    break;
                case Menu_Ha:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//a
                    BspTm1639_Writebyte(Table_S[ 8]); BspTm1639_Writebyte(Table_S[ 8]>>4);//H
                    break;
                case Menu_Hb:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[ 8]); BspTm1639_Writebyte(Table_S[ 8]>>4);//H
                    break;
                case Menu_Pa:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//a
                    BspTm1639_Writebyte(Table_S[17]); BspTm1639_Writebyte(Table_S[17]>>4);//P
                    break;
                case Menu_Pb:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[17]); BspTm1639_Writebyte(Table_S[17]>>4);//P
                    break;
                case Menu_Fa:
                    BspTm1639_Writebyte(Table_S[ 0]); BspTm1639_Writebyte(Table_S[ 0]>>4);//a
                    BspTm1639_Writebyte(Table_S[ 6]); BspTm1639_Writebyte(Table_S[ 6]>>4);//F
                    break;
                case Menu_Fb:
                    BspTm1639_Writebyte(Table_S[ 1]); BspTm1639_Writebyte(Table_S[ 1]>>4);//b
                    BspTm1639_Writebyte(Table_S[ 6]); BspTm1639_Writebyte(Table_S[ 6]>>4);//F
                    break;            
                default:
                    break;
            }
            TM1639_STB_High();
            BspTm1639_Writebyte(LEVEL_USE);
            TM1639_STB_High();
        }
    }
    else
    {
        OldShowMode = 0xFF;
        TM1639_STB_High();
        TM1639_CLK_High();
        TM1639_DIO_High();
        BspTm1639_Writebyte(0x40);	//数据命令设置
        TM1639_STB_High();
        BspTm1639_Writebyte(DIG0);	//显示地址 起始地址
        BspTm1639_Writebyte(0x00);BspTm1639_Writebyte(0x00);
        BspTm1639_Writebyte(0x00);BspTm1639_Writebyte(0x00);
		if((ShowMode==Menu_U)||(ShowMode==Menu_A))
		{
			BspTm1639_Writebyte(0x00);BspTm1639_Writebyte(0x00);
		}
		goto ShowBack;
    }
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

