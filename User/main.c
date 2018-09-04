//更新说明：


#include "bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



static void InitBoard(void);
void Delay (uint16_t nCount);

uint8_t g_RxMessage[8]={0};	//CAN接收数据
uint8_t g_RxMessFlag=0;		//CAN接收数据 标志
uint8_t OutFlag=0;	//放水标志,上电标志
uint8_t InPutCount=0;		//输入脉冲计数
uint32_t g_RunningTime=0;		//运行时间
extern CQ_FIFO_T s_gCQ;				//消息FIFO变量,结构体 */
extern uint8_t RS485_Count;
extern uint8_t RS485Dat[10];

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

void RCC_Configuration(void)
{
//    RCC_DeInit();
//    RCC_HSEConfig(RCC_HSE_OFF);
//    RCC_HSICmd(ENABLE);
//    RCC_HSEConfig(RCC_HSE_ON);
}
void CPU_IDLE(void)
{
	static uint8_t g_FlagDWG=0;
	if(++g_FlagDWG%2==0)	HardWDG_ON();
	else 					HardWDG_OFF();
}


uint8_t PutOutMemoryBuf(uint8_t *_Date)	//清第一个缓存
{
	if (s_gCQ.Read == s_gCQ.Write)
	{
		return CQ_NONE;
	}
	else
	{
		memcpy(_Date,s_gCQ.Buf[s_gCQ.Read],16);
		if (++s_gCQ.Read >= CQ_FIFO_SIZE)
		{
			s_gCQ.Read = 0;
		}
		return CQ_OK;
	}
}

void PutInMemoryBuf(u8 *Buf)	//末尾加入第一个缓存
{
    uint8_t BufDat[16]={0};
	//日志存储
    BufDat[ 0] = (uint8_t)(g_RunningTime>>24); //时钟
    BufDat[ 1] = (uint8_t)(g_RunningTime>>16); //时钟
    BufDat[ 2] = (uint8_t)(g_RunningTime>>8);  //时钟
    BufDat[ 3] = (uint8_t)(g_RunningTime);     //时钟
    BufDat[ 4] = Buf[1];     	//卡号0；
    BufDat[ 5] = Buf[2];     	//卡号1；
    BufDat[ 6] = Buf[3];     	//卡号2；
    BufDat[ 7] = Buf[4];     	//卡号3；
    BufDat[ 8] = Buf[8];		//校验	
    BufDat[ 9] = Buf[ 5];     	//金额1高位
    BufDat[10] = Buf[ 6];     	//金额2
    BufDat[11] = Buf[ 7];     	//金额3
    BufDat[12] = Buf[ 9]&0x7F;	//DecSum高位
    BufDat[13] = Buf[10];     	//DecSum低位
	
	if((Buf[ 9]&0x80)==0x80)    BufDat[14] = 0x03;	 //连续消息标记; 
	else
    {
        if( Buf[0] == 0xAA )	BufDat[14] = 0x01;  //插卡；
        else					BufDat[14] = 0x02;  //取卡； 
	}
    BufDat[15] = CRC8_Table(BufDat,15); 	//CRC
	

	memcpy(s_gCQ.Buf[s_gCQ.Write],Buf,10);

	if (++s_gCQ.Write  >= CQ_FIFO_SIZE)
	{
		s_gCQ.Write = 0;
	}
}

void Init_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SoftReset(void)
{
	__set_FAULTMASK(1);		// 关闭所有中端
	NVIC_SystemReset();		// 复位
}

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t DHT_Dat[5]={0},RS485Dat_Key[2]={0};
	uint8_t Time_250ms=0;
	uint8_t KeyDat=0;
    uint8_t RunMode = 0;    //0手动模式(默认)，1主一备二，2主二备一;
    KMDat ApRun,BpRun;      //A/B泵 运行状态    
	SystemInit();
	
	InitBoard();		//硬件初始化
	BspTm1639_Show(0x01,0x00);	//上电初始化显示
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN初始化正常模式,波特率60Kbps  //则波特率为:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");
	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//定时器1周期 200毫秒
	KMOFF_Show(ALARMALL);
	HC595_E1_ON();HC595_E2_ON();
    RS485Dat_LED1_ON();	//远程通信指示灯
    RS485Dat_LED2_ON(); //主板通信指示灯	
    RS485Dat_LED12_ON();//A泵停止指示灯
    RS485Dat_LED18_ON();//B泵停止指示灯
    RS485Dat_LED16_ON();//手动模式指示灯
    while (1)
	{
		CPU_IDLE();
		if ( bsp_CheckTimer(1) )	//软定时器
		{
			bsp_StartTimer(1, 200);	//启动下个定时周期
			if(Time_250ms<20)	Time_250ms++;
			else	
			{	
				Time_250ms=0;
                if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0){;}
                Get_InputValue();
			}
            //ReadDat_CD4067();
            ReadInputDat();
            DisplaySendDat();
			KeyDat = bsp_GetKey();
			if(KeyDat!=KEY_NONE)	//按键检测及数据处理
			{
				switch (KeyDat)
				{
					case KEY_NONE:	//无按键按下
						break;
					case KEY_1_DOWN:	//						
						printf("  KEY_1_DOWN!\r\n");
						break;
					case KEY_1_UP:		//
						printf("  KEY_1_UP!\r\n");
						break;
					case KEY_2_DOWN:	//
						printf("  KEY_2_DOWN!\r\n");						
						break;
					case KEY_2_UP:		//
						printf("  KEY_2_UP!\r\n");
						break;
					case KEY_3_DOWN:	//
                        RS485Dat_LED1_ON();
						printf("  KEY_3_DOWN!\r\n");
						break;
					case KEY_3_UP:		//
                        RS485Dat_LED1_OFF();
						printf("  KEY_3_UP!\r\n");
						break;
					case KEY_4_DOWN:	//
                        RS485Dat_LED2_ON();
						printf("  KEY_4_DOWN!\r\n");						
						break;
					case KEY_4_UP:		//
                        RS485Dat_LED2_OFF();
						printf("  KEY_4_UP!\r\n");
						break;
					default:
						break;
				}
			}

			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//接收到数据
			{
				RS485_Count = 0;
                RS485Dat_Key[0] = RS485Dat[4];  RS485Dat_Key[1] = RS485Dat[5];  //获取按键值
                if((RS485Dat_Key[0]!=0x00)||(RS485Dat_Key[1]!=0x00))
                printf("RS485_ReceiveDat(),%02X,%02X.\r\n",RS485Dat_Key[0],RS485Dat_Key[1]);
                if(RS485Dat_Key[1]&0x01==0x01){;}       //系统复位
                else if((RS485Dat_Key[1]&0x08)==0x08){RunMode=1;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //主一备二
                else if((RS485Dat_Key[1]&0x10)==0x10){RunMode=0;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //手动模式
                else if((RS485Dat_Key[1]&0x20)==0x20){RunMode=2;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //主二备一
			}
            if(RunMode==0)  //手动模式
            {
                if((RS485Dat_Key[1]&0x02)==0x02)//A泵停止        
                {   
                    ApRun.Statue=Stop;
                    printf("手动模式,A泵停止.\r\n");
                    KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
                }  
                else if((RS485Dat_Key[0]&0x01)==0x01)   //A泵低速
                {   
                    ApRun.Statue=Slow;     
                    printf("手动模式,A泵低速.\r\n");
                    KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
                }  
                else if((RS485Dat_Key[1]&0x04)==0x04)   //A泵高速
                {   
                    ApRun.Statue=HighSpeed;
                    printf("手动模式,A泵高速.\r\n");
                    KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
                }  
                else if((RS485Dat_Key[1]&0x40)==0x40)   //B泵停止
                {   
                    BpRun.Statue=Stop;     
                    printf("手动模式,B泵停止.\r\n");
                    KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
                }  
                else if((RS485Dat_Key[0]&0x02)==0x02)   //B泵低速
                {   
                    BpRun.Statue=Slow;     
                    printf("手动模式,B泵低速.\r\n");
                    KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
                }  
                else if((RS485Dat_Key[1]&0x80)==0x80)   //B泵高速 
                {   
                    BpRun.Statue=HighSpeed;
                    printf("手动模式,B泵高速.\r\n");
                    KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
                }                 
            }

		}

	}
}

/*
*********************************************************************************************************
*	函 数 名: InitBoard
*	功能说明: 初始化硬件设备
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitBoard(void)
{	
	RCC_Configuration();
	/* 初始化systick定时器，并启动定时中断 */
	bsp_InitTimer(); 
	delay_init();
	delay_ms(500);
	Init_GPIO();				//输出初始化
	bsp_InitKey();
	bsp_InitUart(); 	    	//初始化串口+RS485
	printf("\r\nStarting Up...\r\nJOSONG-XS03 V3.0...\r\n");
	printf("VersionNo: %02X...\r\n",VERSION);
	printf("SystemCoreClock: %d...\r\n",SystemCoreClock);
	delay_ms(500);
	BspTm1639_Config();	        //TM1639初始化
	BspDht11_Config();			//温湿度传感器初始化
	bsp_HC595_Config();			//HC595初始化 用于继电器输出
	BspInput_CD4067_Config();	//CD4067初始化 用于信号采集
	BspInput_CD4051_Config();	//CD4051*3
	Bsp_CS5463_Config();
	CS546x_Init(0);
	TIM3_Int_Init(99,720-1);  	//以100khz的频率计数，0.01ms中断，计数到100 *0.01ms 为1ms 
//	TIM2_Cap_Init(0xFFFF,72-1);	//以1Mhz的频率计数 
 	Adc_Init();		  		    //ADC初始化
	bsp_InitCQVar();
}

extern void SysTick_ISR(void);	/* 声明调用外部的函数 */
uint8_t Time_count=0;
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //TIM3更新中断 20ms中断
	{
		if(Time_count>200)	Time_count=0;
		Time_count++;
		SysTick_ISR();	//这个函数在bsp_timer.c中 
		if((Time_count%10)==0)			bsp_KeyScan();	//每10ms扫描按键一次
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  	//清除TIMx更新中断标志 					
	}
}

void Delay(__IO uint16_t nCount)
{
	while (nCount != 0)
	{
		nCount--;
	}
}



