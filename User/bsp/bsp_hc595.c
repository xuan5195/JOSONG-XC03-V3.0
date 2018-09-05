#include "stm32f10x.h"	
#include "bsp_hc595.h"		
#include "bsp.h"

static uint16_t g_LedShowDat=0,Old_LedShowDat=0;
static void HC595_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=15;x>0;x--);
	}
}
void bsp_HC595_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	HC595_E1_OFF();HC595_E2_OFF();	//防止继电器误动作
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	HC595_E1_OFF();HC595_E2_OFF();
//	HC595_E1_ON();HC595_E2_ON();
}

static void BspLed_Show(u16 ShowDat)//0100= 0000 0100 //0200//0400
{
	u8 i;
	u16 j = ShowDat;
	Led_RCLK_Low();
	Led_SRCLK_Low();
	HC595_Delay(1);
	for(i=0;i<16;i++)
	{  
		if((j&0x8000) == 0x8000){	Led_DAT_High();	}
		else					{	Led_DAT_Low();	}
		HC595_Delay(1);
		Led_SRCLK_High();	//上升沿，数据移入
		HC595_Delay(1);		//修改延时,调整上升沿顺序，解决输出误动作问题
		Led_SRCLK_Low();
		j<<=1;
	}
	Led_RCLK_High(); 	//产生一个上升沿，将数据更新并口输出
	HC595_Delay(1);
	Led_RCLK_Low();
}

void KMON_Show(u16 ShowDat)
{
	g_LedShowDat = g_LedShowDat&(~ShowDat);
//	printf("g_LedShowDat: %04X...\r\n",g_LedShowDat);
//    BspLed_Show(g_LedShowDat);
}
void KMOFF_Show(u16 ShowDat)
{
    g_LedShowDat = g_LedShowDat|ShowDat;
//	printf("g_LedShowDat: %04X...\r\n",g_LedShowDat);
//    BspLed_Show(g_LedShowDat);
}

void KMOutUpdat(void)
{
    if(Old_LedShowDat!=g_LedShowDat)
    {
        BspLed_Show(0x0FFF);
        Old_LedShowDat=g_LedShowDat;
//        printf("g_LedShowDat: %04X...\r\n",g_LedShowDat);
        delay_ms(100);
        BspLed_Show(g_LedShowDat);
    }
}

