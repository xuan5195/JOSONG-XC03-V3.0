#include "stm32f10x.h"		
#include "bsp_cd4067.h"		

static void BspInput_CD4067_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
}

void BspInput_CD4067_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void SetInput_CD4067Switch(uint8_t uSetDat)
{
    switch (uSetDat)
    {
        case 0:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 1:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 2:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 3:
            Iutput_CD4067A_High();	Iutput_CD4067B_High(); 	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 4:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 5:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 6:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 7:
            Iutput_CD4067A_High();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 8:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 9:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 10:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 11:
            Iutput_CD4067A_High();	Iutput_CD4067B_High(); 	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 12:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 13:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 14:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 15:
            Iutput_CD4067A_High();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
       default:
            break;
    }
}

/*
*********************************************************************************************************
*	函 数 名: Read_InputDevDat
*	功能说明: 检测光耦是否导通
*	形 	  参: 选择检测通道
*	返 回 值: 0，光耦导通；1，外部无触发信号
*	备 	  注：
*			 0~3，由于是检测交流220V，一个周期内会有一半多时间光耦不导通，因此需要特殊处理
*
*		算法说明：由于前端是220v电压，因此光耦输出端波形并高电平或者低电平，而是方波，需要做过滤处理,
*				  220V电压，频率为50hz即20ms，一个周期内只有5ms时间可以导通光耦，其它时间不导通，
*				  因而需要跳过这段不导通时间
*********************************************************************************************************
*/
u8 Read_InputDevDat(uint8_t uAreaDat)  //
{
	u8 i;	
	SetInput_CD4067Switch(uAreaDat);
	BspInput_CD4067_Delay(5);
	if(uAreaDat<4) 
	{
		if(Read_InputData()==1)   //高电平，需要进一步处理,正常是低电平
		{
			for(i=0;i<10;i++)
			{
				if(Read_InputData()==0)	return 0;
				BspInput_CD4067_Delay(5);
			}	
		}
	}
	return 1;
}


/*
*********************************************************************************************************
*	函 数 名: Read_KMFB
*	功能说明: 检测是否有光耦信号
*	形 	  参: 选择检测通道
*	返 回 值: 0，光耦导通；1，外部无触发信号
*	备 	  注：
*			  本函数只用于检测光耦隔离输入信号检测，220检测由另外一个函数完成，如果使用操作系统注意上锁，
			  否则可能出现同时访问一个设备情况
*********************************************************************************************************
*/
u8 Read_Optocoupler(uint8_t uAreaDat)  //
{	
	SetInput_CD4067Switch(uAreaDat);
	BspInput_CD4067_Delay(5);
	return Read_InputData();
}


