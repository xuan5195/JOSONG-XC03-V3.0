//更新说明：
//2018.01.24  V5.1 增加在未注册时也能够远程复位
//2018.01.26  V5.1 增加在未注册时3min自动软复位，提高带电接入卡机时上线成功率
//2018.01.27  V5.1 修复上卡金额为0、错误代码E300时，刷卡造成放水不停止、不计费及显示金额错误等问题。
//2018.02.03  V5.1 增加在刷卡过程与服务器通信上金额界面闪烁一下。
//2018.02.23  V5.2 修改CAN波特率为60kbps;失联软复位时间由原来3min改为15Sec。
//2018.03.12  V5.3 增加上电主动外发本机SN用于主动注册，发送次数为10次。
//2018.04.21  V0.1 测试版本修改及优化Flash存储问题，将原始值改为全0；
//2018.04.24  V0.2 增加插卡/拔卡数据记录，存储起始地址0x0800D000,空间10K，存储容量800条；
//2018.04.25  V0.2 增加钱包备份，并兼容老版本；
//2018.04.26  V0.3 增加本次刷卡总扣费金额，并对比异常提交"拔卡不扣款动作";金额为600,000,服务器收到该金额时，以服务器为准
//2018.04.26  V0.3 增加DRCCYCLE计费周期后，提交一次"拔卡数据包";
//2018.04.26  V0.3 增加核对SN及逻辑地址广播,不一致软件复位;
//2018.05.01  V0.4 增加时钟接收，时钟存储到运行日志，串口打印日志，CAN取日志数据;
//2018.05.09  V0.5 二张卡同时插入并在移动让卡二次重复读写过程中，提交金额与卡号交错问题;
//2018.05.10  V0.5 修复二个钱包金额不一致时造成卡错误问题;
//2018.05.10  V0.5 卡机刚上电时，用日志中的时钟递增跑-->改为时钟为0时，不跑时钟
//2018.05.16  V0.5 将日志由原12Byte改为16Byte，增加用水量值2Byte，其余留空。
//2018.05.16  V0.5 将DecSum>200时,进入关阀，等待重新插卡。
//2018.05.16  V0.6 版本号55改为56； 修复刚开卡时第一次插卡金额错误问题。
//                 修改日志问题，将插卡\拔卡\连续消费移到BufDat[14]上去，详见格式文档。
//2018.05.18  V0.6 连续消费不提交服务器，只作日志;
//2018.07.04  V0.6 修复卡高位为00时，无法服务器充值;

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
	uint8_t DHT_Dat[5]={0},Buff[4]={0xFF,0x00,0xFF,0x00};
	uint8_t Time_250ms=0;
	uint8_t KeyDat=0;
	SystemInit();
	
	InitBoard();		//硬件初始化
	BspTm1639_Show(0x01,0x00);	//上电初始化显示
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN初始化正常模式,波特率60Kbps  //则波特率为:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");
	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//定时器1周期 200毫秒
	KMOFF_Show(ALARMALL);
	HC595_E1_ON();HC595_E2_ON();	
	while (1)
	{
		CPU_IDLE();
		if ( bsp_CheckTimer(1) )	//软定时器
		{
			bsp_StartTimer(1, 200);	//启动下个定时周期
			if(Time_250ms<9)	Time_250ms++;
			else	
			{	
				Time_250ms=0;
			}
			KeyDat = bsp_GetKey();
			if(KeyDat!=KEY_NONE)	//按键检测及数据处理
			{
				switch (KeyDat)
				{
					case KEY_NONE:	//无按键按下
						break;
					case KEY_1_DOWN:	//
						if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0){;}
						printf("  KEY_1_DOWN!\r\n");
						break;
					case KEY_1_UP:		//
						printf("  KEY_1_UP!\r\n");
						break;
					case KEY_2_DOWN:	//
						printf("  KEY_2_DOWN!\r\n");
						Get_InputValue();
						break;
					case KEY_2_UP:		//
						printf("  KEY_2_UP!\r\n");
						break;
					case KEY_3_DOWN:	//
						KMON_Show(BKM1RUN);
						printf("  KEY_3_DOWN!\r\n");
						break;
					case KEY_3_UP:		//
						KMOFF_Show(BKM1RUN);
						printf("  KEY_3_UP!\r\n");
						break;
					case KEY_4_DOWN:	//
						printf("  KEY_4_DOWN!\r\n");
						Buff[0]=0xFF;Buff[1]=0xFF;
						RS485_SendDat((uint8_t *)Buff);
						break;
					case KEY_4_UP:		//
						printf("  KEY_4_UP!\r\n");
						Buff[0]=0x00;Buff[1]=0x00;
						RS485_SendDat((uint8_t *)Buff);
						break;
					default:
						break;
				}
			}


			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//接收到数据
			{
				RS485_Count = 0;
				printf("RS485:%02X%02X %02X%02X%02X %02X %02X %02X%02X.\r\n",
				RS485Dat[0],RS485Dat[1],RS485Dat[2],RS485Dat[3],RS485Dat[4],RS485Dat[5],RS485Dat[6],RS485Dat[7],RS485Dat[8]); 
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



