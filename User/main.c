//更新说明：


#include "bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void InitBoard(void);
void Delay (uint16_t nCount);

uint8_t g_RxMessage[8]={0};	//CAN接收数据
uint8_t g_RxMessFlag=0;		//CAN接收数据 标志
uint8_t OutFlag=0;	        //放水标志,上电标志
uint8_t InPutCount=0;		    //输入脉冲计数
uint8_t g_ShowUpDateFlag=0;		//显示更新标志
uint32_t g_RunningTime=0;		//运行时间
uint16_t g_CANShowLED;          //用于存储CAN板子上LED显示
uint16_t g_ShowDat[6]={0};
uint8_t StartTimerFlag = 0x00;  //用于自动启动切换定时标志量
uint8_t User_StartT_Flag = 0x00;  //用于自动启动切换定时标志量
uint8_t g_TM1639Flag;
uint8_t g_KeyNoneCount;
uint8_t g_ChangeFlag=0x00;
//uint8_t g_StartMode = HH33;
extern CQ_FIFO_T s_gCQ;			//消息FIFO变量,结构体
extern uint8_t RS485_Count;
extern uint8_t RS485Dat[10];
extern MotorChar gAp,gBp;  //A\B泵相关信息
extern uint8_t Menu;
extern uint8_t g_ShowMode;

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
    BufDat[ 4] = Buf[1];     	
    BufDat[ 5] = Buf[2];     	
    BufDat[ 6] = Buf[3];     	
    BufDat[ 7] = Buf[4];     	
    BufDat[ 8] = Buf[8];		
    BufDat[ 9] = Buf[5];     
    BufDat[10] = Buf[6];     
    BufDat[11] = Buf[7];  
    BufDat[12] = Buf[9];
    BufDat[13] = Buf[10];
	BufDat[14] = 0x02;
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
    uint8_t RunMode = 0;    //0手动模式(默认)，1主一备二，2主二备一;
    uint8_t RunDate = 0x11; //运行数据，低四位为A泵,高四位为B泵 0x1停止，0x2低速，0x4高速;
    uint8_t OldRunMode=0,OldStartFlag=0;
    uint8_t OlineSrart=0;   //远程启动标志
    uint8_t HumiFlag=0;     //除湿标志，0x00表示停止，0x05表示开启；
	uint8_t canbuf[8]={0};  //CAN数据缓存
	SystemInit();
	
	InitBoard();		//硬件初始化
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN初始化正常模式,波特率60Kbps  //则波特率为:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	//printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//定时器1周期 200毫秒
    RS485Dat_LED1_ON();	//远程通信指示灯
    RS485Dat_LED2_ON(); //主板通信指示灯	
    RS485Dat_LED12_ON();//A泵停止指示灯
    RS485Dat_LED18_ON();//B泵停止指示灯
    RS485Dat_LED16_ON();//手动模式指示灯
    RS485Dat_LED6_ON(); //主三相电
    delay_ms(10);
	KMOFF_Show(ALARMALL);KMOutUpdat();
	HC595_E1_ON();HC595_E2_ON();
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
                if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0)
                {   g_ShowDat[2] = DHT_Dat[2]; g_ShowDat[3] = DHT_Dat[0];   }
                Get_InputValue();
			}
            if(Time_250ms%10==0)        RS485Dat_LED2_ON();     //主板通信指示灯
            else if(Time_250ms%10==5)   RS485Dat_LED2_OFF();    //主板通信指示灯
            if(Time_250ms%2==0)         CanSendDat(g_ShowMode); //CAN数据发送
            ReadInputDat(gParamDat[Menu_StartMode]);     //读取A/B泵全部状态，存放在结构体中
            DisplaySendDat();   //RS485数据发送
			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//接收到数据
			{
				RS485_Count = 0;
                RS485Dat_Key[0] = RS485Dat[4];  RS485Dat_Key[1] = RS485Dat[5];  //获取按键值
                if((RS485Dat_Key[0]!=0x00)||(RS485Dat_Key[1]!=0x00))
                {
                    //printf("RS485_ReceiveDat(),%02X,%02X.\r\n",RS485Dat_Key[0],RS485Dat_Key[1]);
                    if((RS485Dat_Key[1]&0x01)==0x01){NVIC_SystemReset();}       //系统复位
                    else if((RS485Dat_Key[1]&0x08)==0x08){RunMode=1;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //主一备二
                    else if((RS485Dat_Key[1]&0x10)==0x10){RunMode=0;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //手动模式
                    else if((RS485Dat_Key[1]&0x20)==0x20){RunMode=2;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //主二备一
                }
			}
            if(RunMode==0)      //手动模式,直接操作继电器输出
            {
                RS485Dat_LED15_OFF();RS485Dat_LED16_ON();RS485Dat_LED17_OFF();
				if(gParamDat[Menu_StartMode]==HHFJ)	//风机模式，低速按键有效
				{
					if((RS485Dat_Key[1]&0x02)==0x02)//A泵停止        
					{
						RunDate = (RunDate&0xF0)|0x01;
						printf("手动操作,A泵，停止.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[0]&0x01)==0x01)   //A泵低速
					{   
						RunDate = (RunDate&0xF0)|0x02;
						printf("手动操作,A泵，低速.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x04)==0x04)   //A泵高速
					{   
						RunDate = (RunDate&0xF0)|0x04;
						printf("手动操作,A泵，高速.0x%02X.\r\n",RunDate);
					}				
					else if((RS485Dat_Key[1]&0x40)==0x40)   //B泵停止
					{   
						RunDate = (RunDate&0x0F)|0x10;
						printf("手动操作,B泵，停止.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[0]&0x02)==0x02)   //B泵低速
					{   
						RunDate = (RunDate&0x0F)|0x20;
						printf("手动操作,B泵，低速.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x80)==0x80)   //B泵高速 
					{   
						RunDate = (RunDate&0x0F)|0x40;
						printf("手动操作,B泵，高速.0x%02X.\r\n",RunDate);
					}
				}
				else	
				{
					if((RS485Dat_Key[1]&0x02)==0x02)//A泵停止        
					{
						RunDate = (RunDate&0xF0)|0x01;
						printf("手动操作,A泵，停止.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x04)==0x04)   //A泵高速
					{   
						RunDate = (RunDate&0xF0)|0x02;
						printf("手动操作,A泵，启动.0x%02X.\r\n",RunDate);
					}				
					else if((RS485Dat_Key[1]&0x40)==0x40)   //B泵停止
					{   
						RunDate = (RunDate&0x0F)|0x10;
						printf("手动操作,B泵，停止.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x80)==0x80)   //B泵高速 
					{   
						RunDate = (RunDate&0x0F)|0x20;
						printf("手动操作,B泵，启动.0x%02X.\r\n",RunDate);
					}
				}					
            }
            else if(RunMode==1)  //主一备二
            {
                RS485Dat_LED15_ON();RS485Dat_LED16_OFF();RS485Dat_LED17_OFF();
            }
            else if(RunMode==2)  //主二备一
            {
                RS485Dat_LED15_OFF();RS485Dat_LED16_OFF();RS485Dat_LED17_ON();
            }
            if(RunMode!=OldRunMode) //运行模式切换，把运行模式全部转为停止
            {
                OldRunMode = RunMode;
                RunDate = 0x11;
                printf(">>运行模式切换,全部转为停止.0x%02X.  ",RunDate);
                if(RunMode==0)      {   printf("手动模式.\r\n");    }
                else if(RunMode==1) {   printf("主一备二模式.\r\n");}
                else if(RunMode==2) {   printf("主二备一模式.\r\n");}
            }
            if(RunMode!=0)  //自动模式
            {
                if((Read_Optocoupler(8)==0)||(Read_Optocoupler(9)==0)||(OlineSrart == 0xAA)) //远控1/远控2检测
                {
                    if(StartTimerFlag==0x00)
                    {
                        StartTimerFlag=0xAA;        //标记进入启动，开启定时功能
						if(RunMode==1)  	{RunDate=0x12;printf("自动模式.启动A泵.");}//主一备二
						else if(RunMode==2) {RunDate=0x21;printf("自动模式.启动B泵.");}//主二备一
                    }
                    else if(StartTimerFlag==0xBB)   //进入第二阶段，定时时间到，该定时在定时器中实现
                    {
                        StartTimerFlag=0xCC;        //标记进入高速运行
						if(RunMode==1)  	{RunDate=0x14;printf("切换高速.\r\n");}//主一备二
						else if(RunMode==2) {RunDate=0x41;printf("切换高速.\r\n");}//主二备一
                    }
                }
                else
                {
                    if(StartTimerFlag!=0x00)
                    {
						RunDate = 0x11;
						StartTimerFlag = 0x00;  
						g_ChangeFlag = 0x00;
					}
                }
            }
			else
			{
				if(gParamDat[Menu_StartMode]!=HHFJ)	//不是 风机模式，低速-高速自动跳转
				{
					if((RunDate&0x0F)!=0x01)	
					{
						if(gAp.StartT_Flag==0x00)
						{
							gAp.StartT_Flag=0xAA;
							RunDate = (RunDate&0xF0)|0x02;
                            printf("手动模式.启动A泵.  ");
						}
						else if(gAp.StartT_Flag==0xBB)
						{
							gAp.StartT_Flag=0xCC;
							RunDate = (RunDate&0xF0)|0x04;
                            printf("切换高速.\r\n");
						}
						
					}
					else
					{
						gAp.StartT_Flag=0x00;
						RunDate = (RunDate&0xF0)|0x01;
					}
					
					if((RunDate&0xF0)!=0x10)	
					{
						if(gBp.StartT_Flag==0x00)
						{
							gBp.StartT_Flag=0xAA;
							RunDate = (RunDate&0x0F)|0x20;
                            printf("手动模式.启动B泵.  ");
						}
						else if(gBp.StartT_Flag==0xBB)
						{
							gBp.StartT_Flag=0xCC;
							RunDate = (RunDate&0x0F)|0x40;
                            printf("切换高速.\r\n");
						}
						
					}
					else
					{
						gBp.StartT_Flag=0x00;
						RunDate = (RunDate&0x0F)|0x10;
					}				
				}
			}
            RunDate = KMStart_Pro(RunMode,RunDate,gParamDat[Menu_StartMode]);
            if(gParamDat[Menu_Ha]>gParamDat[Menu_Hb])   //湿度上限>下限值 开启自动除湿功能
            {
                if((g_ShowDat[3]>gParamDat[Menu_Ha])&&(HumiFlag==0x00))
                {
                    HumiFlag = 0x05;
                    KMON_Show(AHUMIDITY);   //打开除湿
                    printf("湿度%2d(Limit:%2d-%2d),启动除湿设备!\r\n",g_ShowDat[3],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                }
                if(((g_ShowDat[3])<gParamDat[Menu_Hb])&&(HumiFlag!=0x00))
                {
                    HumiFlag = 0x00;
                    KMOFF_Show(AHUMIDITY);   //关闭除湿
                    printf("湿度%2d(Limit:%2d-%2d),关闭除湿设备.\r\n",g_ShowDat[3],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                }
            }
            KMOutAnswer();  //无源/有源继电器输出控制
        }
        KMOutUpdat();   //统一更新继电器输出
        SetParam();     //设置修改参数
        if(Menu==Menu_Idle)
        {
            if(g_ShowMode == Show_UV)       { BspTm1639_Show(g_ShowUpDateFlag+0,g_ShowDat[g_ShowUpDateFlag+0]);}   //循环显示 电压、电流
            else if(g_ShowMode == Show_CF)  { BspTm1639_Show(g_ShowUpDateFlag+2,g_ShowDat[g_ShowUpDateFlag+2]);}   //循环显示 温度、湿度
            else if(g_ShowMode == Show_PL)  { BspTm1639_Show(g_ShowUpDateFlag+4,g_ShowDat[g_ShowUpDateFlag+4]);}   //循环显示 压力、流量
        }
        else                BspTm1639_ShowParam(g_TM1639Flag,Menu,gParamDat[Menu]);
        if(bsp_GetCQ((uint8_t *)canbuf)!=CQ_NONE)//接收到有数据
        {
			//printf(">>CanRxMsg:%02X %02X %02X%02X%02X%02X%02X%02X.\r\n",canbuf[0],canbuf[1],canbuf[2],canbuf[3],canbuf[4],canbuf[5],canbuf[6],canbuf[7]);
            if     (canbuf[0]==0x01){RunMode=1;}  //主一备二
            else if(canbuf[0]==0x02){RunMode=0;}  //手动模式
            else if(canbuf[0]==0x03){RunMode=2;}  //主二备一
            if((OldStartFlag==0x00)&&(canbuf[1]==0x55))
            {   //远程板,急启按键按下
                OldStartFlag=0xAA;  printf("远程板,急启按键按下!\r\n");
                OlineSrart = 0xAA;  RS485Dat_LED3_ON();
            }  
            else if((OldStartFlag==0xAA)&&(canbuf[1]!=0x55))
            {
                OldStartFlag=0x00;  printf("远程板,急启按键松开!\r\n");
                OlineSrart = 0x00;  RS485Dat_LED3_OFF();
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
	delay_ms(200);
	Init_GPIO();				//输出初始化
    CPU_IDLE();delay_ms(200);
	bsp_InitKey();
	bsp_InitUart(); 	    	//初始化串口+RS485
	printf("\r\nStarting Up...\r\nJOSONG-XS03 V3.0...\r\n");
	printf("VersionNo: %02X...\r\n",VERSION);
	printf("SystemCoreClock: %d...\r\n",SystemCoreClock);
	delay_ms(100);
	BspTm1639_Config();	        //TM1639初始化
	BspTm1639_Show(0xA1,0x00);	//上电初始化显示
	BspDht11_Config();			//温湿度传感器初始化
	bsp_HC595_Config();			//HC595初始化 用于继电器输出
	BspInput_CD4067_Config();	//CD4067初始化 用于信号采集
	BspInput_CD4051_Config();	//CD4051*3
	Bsp_CS5463_Config();
	CS546x_Init(0);
    Read_Flash_Dat();   //读出Flash数据
    CPU_IDLE();
    delay_ms(200);
	TIM3_Int_Init(99,720-1);  	//以100khz的频率计数，0.01ms中断，计数到100 *0.01ms 为1ms 
//	TIM2_Cap_Init(0xFFFF,72-1);	//以1Mhz的频率计数 
 	Adc_Init();		  		    //ADC初始化
	bsp_InitCQVar();
}

extern void SysTick_ISR(void);	/* 声明调用外部的函数 */
uint16_t Time_count=0,KMTime_count=0;
uint16_t gApTime_Count=0,gBpTime_Count=0;
uint8_t KMTime_Sec=0,gApTime_Sec=0,gBpTime_Sec=0;
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //TIM3更新中断 1ms中断
	{
		if(Time_count>3000) //3秒周期产生
        {
            Time_count = 0;
            g_ShowUpDateFlag = (1+g_ShowUpDateFlag)%2;  //0-1-0
        }
		Time_count++;
        if((Time_count%100)==1) //100mS中断
        {
            if(gAp.DelayCheck_Count>0)  gAp.DelayCheck_Count--;//延时检测时间到，进入检测
            else                        gAp.DelayCheck_Count=0;
            if(gBp.DelayCheck_Count>0)  gBp.DelayCheck_Count--;//延时检测时间到，进入检测
            else                        gBp.DelayCheck_Count=0;
            if((gAp.SlowTimer_Count>0)&&(gAp.SlowTimer_Count<200))   gAp.SlowTimer_Count--;//低速运行时间
            if((gBp.SlowTimer_Count>0)&&(gAp.SlowTimer_Count<200))   gBp.SlowTimer_Count--;//低速运行时间
        }
        if((Time_count%1000==700)&&(g_TM1639Flag==0))    g_TM1639Flag = 1;
        if((Time_count%1000==  1)&&(g_TM1639Flag==1))    g_TM1639Flag = 0;
        if(Time_count%1000==1)  {  if(g_KeyNoneCount>0)  g_KeyNoneCount--;  }
        if(StartTimerFlag==0xAA)    
        {
            KMTime_count++;
            if(KMTime_count>1000) //1秒周期定时
            {
                KMTime_count = 0;
                KMTime_Sec++;
                if(KMTime_Sec>6)    StartTimerFlag = 0xBB;  //定时时间到
            }
        }
        else if(StartTimerFlag==0x00)
        {
            KMTime_count = 0;   KMTime_Sec = 0;
        }

		if(gAp.StartT_Flag==0xAA)
		{
			gApTime_Count++;
			if(gApTime_Count>1000)
			{
				gApTime_Count=0;
				gApTime_Sec++;
				if(gApTime_Sec>6)	gAp.StartT_Flag = 0xBB;
			}
		}
		else if(gBp.StartT_Flag==0x00)
		{
			gApTime_Count=0;gApTime_Sec=0;
		}
		if(gBp.StartT_Flag==0xAA)
		{
			gBpTime_Count++;
			if(gBpTime_Count>1000)
			{
				gBpTime_Count=0;
				gBpTime_Sec++;
				if(gBpTime_Sec>6)	gBp.StartT_Flag = 0xBB;
			}
		}
		else if(gBp.StartT_Flag==0x00)
		{
			gBpTime_Count=0;gBpTime_Sec=0;
		}		
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



