#include "stm32f10x.h"		
#include "bsp_cd4067.h"		
#include "bsp.h"

void BspInput_CD4067_Delay_us(u16 z) 
{ 
    delay_us(z);
}
void BspInput_CD4067_Delay_ms(u16 z) 
{ 
    delay_ms(z);
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
u8 Count[4]={3,3,3,3},OldState[4]={0};
u8 Read_InputDevDat(uint8_t _no)  
{
	if(_no<4) 
	{
        SetInput_CD4067Switch(_no);
        BspInput_CD4067_Delay_us(20);
        if(Read_InputData()==0)
        {
            Count[_no] = 3; OldState[_no] = 0;
            return OldState[_no];
        }
		else   //高电平，需要进一步处理,正常是低电平
		{
            if(Count[_no]>0)
            {
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 3;OldState[_no] =0;return OldState[_no]; }
                else                    {   Count[_no]--;    return OldState[_no];            }
            }
            else
            {
                Count[_no] = 3; OldState[_no] = 1;
                return OldState[_no];
            }
		}       
	}
	return 0xEF;
}

u8 Read_50HzDat(uint8_t _no)  
{
    SetInput_CD4067Switch(_no);
    BspInput_CD4067_Delay_us(1);   
    return Read_InputData();
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
    u8 _Temp=1;
	SetInput_CD4067Switch(uAreaDat);
	BspInput_CD4067_Delay_us(100);
    if(Read_InputData()==0)
    {
        BspInput_CD4067_Delay_us(100);  //延时一下，重复检测
        if(Read_InputData()==0)
        {
            _Temp=0;
        }
    }
	return _Temp;
}



