#include "stm32f10x.h"		
#include "bsp_cd4051.h"		

static void BspInput_CD4051_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
}

void BspInput_CD4051_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SetInput_CSCD4051Switch(uint8_t uSetDat)	//三相电压采集选择口 在CS5463中使用
{
    switch (uSetDat)
    {
        case IV_C:
            Iutput_CSCD4051A_Low();	Iutput_CSCD4051B_Low();	
            break;
        case IV_B:
            Iutput_CSCD4051A_High();	Iutput_CSCD4051B_Low();	
            break;
        case IV_A:
            Iutput_CSCD4051A_Low();	Iutput_CSCD4051B_High();	
            break;
       default:
            break;
    }
}
void SetInput_IICD4051Switch(uint8_t uSetDat)	//三相电流采集选择口 在CS5463中使用
{
    switch (uSetDat)
    {
        case II_A:
            Iutput_IICSCD4051A_Low();	Iutput_IICSCD4051B_High();	
            break;
        case II_B:
            Iutput_IICSCD4051A_High();	Iutput_IICSCD4051B_Low();	
            break;
        case II_C:
            Iutput_IICSCD4051A_Low();	Iutput_IICSCD4051B_Low();	
            break;
       default:
            break;
    }
}

static void SetInput_KPCD4051Switch(uint8_t uSetDat)
{
    switch (uSetDat)
    {
		 case 0:
			 Iutput_KPCD4051A_Low();	 Iutput_CSCD4051B_Low();	 Iutput_KPCD4051C_Low();
			 break;
		 case 1:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_Low();
			 break;
		 case 2:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_Low(); 
			 break;
		 case 3:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_Low();	 
			 break;
		 case 4:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 5:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 6:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 7:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_High();	 
			 break;
		default:

            break;
    }
}


/*
*********************************************************************************************************
*	函 数 名: Read_Input_KPDevDat
*	功能说明: 检测接触器前导点是否有电压（即主板上继电器开关点是否有220v）
*	形 	  参: 选择检测通道
*	返 回 值: 0，光耦有触发；1，外部无触发信号
*	备 	  注：
*					检测信号与控制点对应关系如下
*
*					02		A泵KM1前导点电压信号
*					01		A泵KM2前导点电压信号
*					00		A泵KM3前导点电压信号
*					03		B泵KM1前导点电压信号
*					04		B泵KM2前导点电压信号
*					05		B泵KM3前导点电压信号
*
*		算法说明：由于前端是220v电压，因此光耦输出端波形并高电平或者低电平，而是方波，需要做过滤处理,
*				  220V电压，频率为50hz即20ms，一个周期内只有5ms时间可以导通光耦，其它时间不导通，
*				  因而需要跳过这段不导通时间
*********************************************************************************************************
*/
u8 Read_Input_KPDevDat(uint8_t uAreaDat)  
{
	u8 i;

	SetInput_KPCD4051Switch(uAreaDat);
	BspInput_CD4051_Delay(20);
	if(uAreaDat<6)
	{
		if(Read_InputData_KP()==1)   //高电平，需要进一步处理，检测一个周期，查看周期内是否有检测到信号
		{
			for(i=0;i<10;i++)
			{
				
				if(Read_InputData_KP()==0)	return 0;
				BspInput_CD4051_Delay(50);
			}	
		}
	}
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: CD4051Read_Optocoupler
*	功能说明: 检测是否有光耦信号
*	形 	  参: 选择检测通道
*	返 回 值: 0，光耦导通；1，外部无触发信号
*	备 	  注：
*			  本函数只用于检测光耦隔离输入信号检测，220检测由另外一个函数完成，如果使用操作系统注意上锁，
			  否则可能出现同时访问一个设备情况
*********************************************************************************************************
*/
u8 CD4051Read_Optocoupler(uint8_t uAreaDat)  
{	
	SetInput_KPCD4051Switch(uAreaDat);
	BspInput_CD4051_Delay(20);
	return Read_InputData_KP();
}

