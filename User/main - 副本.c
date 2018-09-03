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

#define IAP_Flag	1		//IAP标志 0表示没使用在线升级功能


static void InitBoard(void);
void Delay (uint16_t nCount);

uint8_t ShowFlag=0x00;
uint16_t ShowCount=0; 	    //用于正常待机时交替显示
uint16_t gLogNum=0;		    //日志存储条数 0-640
uint8_t gErrorShow=0;	    //异常显示代码 在服务器未更新前显示使用，这样不会出现E000
uint8_t gErrorDat[6]={0};	//异常代码存储
uint8_t Logic_ADD=0;		//逻辑地址
uint8_t g_RxMessage[8]={0};	//CAN接收数据
uint8_t g_RxMessFlag=0;		//CAN接收数据 标志
uint8_t OutFlag=0,PowerUpFlag=0;	//放水标志,上电标志
uint8_t g_MemoryBuf[5][10]={0};	    //数据缓存，[0]=0xAA表示有插卡数据，[0]=0xBB表示有拔卡数据，[1-4]卡号；[5-7]金额；[8]卡核验码；[9]通信码
uint8_t FlagBit = 0x00;		//通信标志位，每插入卡一次数据加1，数值达到199时，清0
uint16_t Beforeupdate = 0;	//远程更新前的扣费金额;
uint16_t OvertimeCount=0;	//超时计数
uint8_t Time20msCount=0;	//
uint8_t OvertimeMinutes=0;	//超时分钟，超时标志 0xAA表示超时
uint8_t BeforeFlag = 0xAA;	//更新标志 0xAA表示未更新; 
uint8_t InPutCount=0;		//输入脉冲计数
uint8_t re_RxMessage[16]={0};
uint32_t RFID_Money=0,OldRFID_Money = 0,u32TempDat=0,RFID_MoneyTemp=0;	//卡内金额
uint8_t CardInFlag=0;
uint8_t Flash_UpdateFlag=0x00;	//Flash有数据更新标志，0xAA表示有数据要更新
uint8_t DelayCount=0;			//上电随机延时
uint8_t g_LoseContact=0;		//失联计数，大于200时，表示失联，自动复位
uint32_t g_RunningTime=0;		//运行时间
uint16_t DecSum=0;				//DecSum递减和
extern uint8_t UID[5];
extern uint8_t FM1702_Buf[16];
extern uint8_t Physical_ADD[4];     //物理地址
extern uint8_t FM1702_Key[7];
extern uint8_t WaterCost,CostNum;	//WaterCost=水费 最小扣款金额  //脉冲数
extern uint8_t g_IAP_Flag;			//在线升级标志

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

void RCC_Configuration(void)
{
//    RCC_DeInit();
//    RCC_HSEConfig(RCC_HSE_OFF);
//    RCC_HSICmd(ENABLE);
    RCC_HSEConfig(RCC_HSE_ON);
}
void PutOutMemoryBuf(void)	//清第一个缓存
{
	uint8_t i,j;
	for(i=0;i<4;i++)
		for(j=0;j<10;j++)
			g_MemoryBuf[i][j] = g_MemoryBuf[i+1][j];
	for(j=0;j<10;j++)
		g_MemoryBuf[4][j] = 0x00;
}
u8 SeekMemoryDat(u8 *Buf)
{
	uint8_t i;
	for(i=0;i<5;i++)
		if((g_MemoryBuf[i][0]==Buf[0])&&(g_MemoryBuf[i][1]==Buf[1])\
          &&(g_MemoryBuf[i][2]==Buf[2])&&(g_MemoryBuf[i][3]==Buf[3])\
            &&(g_MemoryBuf[i][4]==Buf[4])&&(g_MemoryBuf[i][5]==Buf[5])\
              &&(g_MemoryBuf[i][6]==Buf[6])&&(g_MemoryBuf[i][7]==Buf[7])\
               &&(g_MemoryBuf[i][8]==Buf[8]) )	return i;
    return 0xF1;    //Error_Flag
}
void PutInMemoryBuf(u8 *Buf)	//末尾加入第一个缓存
{
	uint8_t i,j;
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
	
	Write_LogDat(gLogNum,BufDat);		//写入日志
	if(gLogNum >= 639)  gLogNum = 0;
    else                gLogNum++;

	if((Buf[ 9]&0x80)!=0x80)	//连续消费不提交服务器，只作日志
	{
		if(SeekMemoryDat((u8 *)Buf)==0xF1)  //查询是否在缓存列表中
		{
			if(g_MemoryBuf[4][0]!=0)	//目前已经存满，丢第一个；
			{
				for(i=0;i<4;i++)
					for(j=0;j<10;j++)
						g_MemoryBuf[i][j] = g_MemoryBuf[i+1][j];
				if(FlagBit==200)FlagBit=0;else ++FlagBit;	//通信码
				for(j=0;j<9;j++)	g_MemoryBuf[4][j] = Buf[j];//存入末尾
				g_MemoryBuf[4][9] = FlagBit;
			}
			else
			{
				for(i=0;i<5;i++)
				{
					if(g_MemoryBuf[i][0]!=0)	continue;
					else
					{
						if(FlagBit==200)FlagBit=0;else ++FlagBit;	//通信码
						for(j=0;j<9;j++)	g_MemoryBuf[i][j] = Buf[j];//存入末尾
						g_MemoryBuf[i][9] = FlagBit;
						break;
					}
				}
			}
		}			
    }
}
void Write_FlagStatusLogDat(void)	//写入复位状态
{
	uint8_t Temp_dat=0;
    uint8_t BufDat[16]={0};

	printf("RCC_FLAG_PINRST:%d; RCC_FLAG_PORRST:%d; RCC_FLAG_SFTRST:%d;\r\n",RCC_GetFlagStatus(RCC_FLAG_PINRST),RCC_GetFlagStatus(RCC_FLAG_PORRST),RCC_GetFlagStatus(RCC_FLAG_SFTRST));
	printf("RCC_FLAG_IWDGRST:%d; RCC_FLAG_WWDGRST:%d; RCC_FLAG_LPWRRST:%d;\r\n",RCC_GetFlagStatus(RCC_FLAG_IWDGRST),RCC_GetFlagStatus(RCC_FLAG_WWDGRST),RCC_GetFlagStatus(RCC_FLAG_LPWRRST));
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST)==SET)    {   Temp_dat = 0x80;             }//引脚复位
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST)==SET)    {   Temp_dat = Temp_dat|0x40;    }//POR/PDR复位
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST)==SET)    {   Temp_dat = Temp_dat|0x20;    }//软件复位
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)==SET)   {   Temp_dat = Temp_dat|0x10;    }//独立看门狗复位
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST)==SET)   {   Temp_dat = Temp_dat|0x04;    }//窗口看门狗复位
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST)==SET)   {   Temp_dat = Temp_dat|0x03;    }//低电量复位
    Delay(0xFF); 		//简单延时一下  
    RCC_ClearFlag();    //	printf("RCC_ClearFlag();\r\n");

	//日志存储
    BufDat[ 0] = (uint8_t)(g_RunningTime>>24); //时钟
    BufDat[ 1] = (uint8_t)(g_RunningTime>>16); //时钟
    BufDat[ 2] = (uint8_t)(g_RunningTime>>8);  //时钟
    BufDat[ 3] = (uint8_t)(g_RunningTime);     //时钟
    BufDat[ 4] = 0x00;     	//卡号0；
    BufDat[ 5] = 0x00;     	//卡号1；
    BufDat[ 6] = 0x00;     	//卡号2；
    BufDat[ 7] = 0x00;     	//卡号3；
    BufDat[ 8] = 0x00;		//校验	
    BufDat[ 9] = 0x00;     	//金额1高位
    BufDat[10] = 0x00;     	//金额2
    BufDat[11] = 0x00;     	//金额3
    BufDat[12] = 0x00;	    //DecSum高位
    BufDat[13] = 0x00;     	//DecSum低位
	BufDat[14] = Temp_dat;	//插卡(Bit7=1)取卡(Bit7=0) 连续消息(Bit6=1)	
    BufDat[15] = CRC8_Table(BufDat,15); 	//CRC
	
	Write_LogDat(gLogNum,BufDat);		//写入日志
	if(gLogNum >= 639)  gLogNum = 0;
    else                gLogNum++;
}

void Init_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
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
	uint8_t PUSendCount = 0x00;	    //上电发送随机数次数；
	uint8_t CardUnReadFlag=0xAA;	//读卡标志，用于延时执行取卡动作
	uint8_t OldeUID[5]={0};			//卡号，OldeUID[4]为校验数据
	uint8_t MemoryBuffer[12]={0};	//数据缓存，[0]=0xAA表示有插卡数据，[0]=0xBB表示有拔卡数据，[1-4]卡号；[5-7]金额；[8]卡核验码；[9-10]DecSum递减和,[11]预留
	uint8_t RFID_Count=0;			//卡开启水阀延时时间;3秒
	uint8_t ErrorTemp[5]={0};
	uint8_t UseErrFlag = 0x00;		//用户或卡异常标志,接收到服务器返回数据后才显示异常代码值 否则显示‘E000’
	uint8_t PowerUp_Count=5;
	uint8_t i=0;
	uint8_t OldCardInFlag = 0;		//用于标记插卡、拔卡动作
	uint32_t Premoney=0;			//Premoney插卡时金额
	uint8_t Time_250ms=0;
	uint8_t _MoneyTemp[4]={0},_ErrTemp[2]={0};
	uint8_t f=0,CQ_Start=0;
	uint8_t CQ_Buf1[16],CQ_Buf2[16];
	u8 ReadStatus=0;	//RFID卡读取返回值 
	SystemInit();
	
	#if IAP_Flag
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x5000); //设置中断向量表的位置在 0x5000，并且将Target的IROM1起始改为0x08005000.
	#endif
	
	InitBoard();		//硬件初始化
	Delay(0xFFFF); 		//上电简单延时一下  
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN初始化正常模式,波特率60Kbps  //则波特率为:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	printf("\r\nStarting Up...\r\nYCKJ-KJ01 V3.1...\r\n");
	printf("VersionNo: %02X...\r\n",VERSION);
	printf("CAN_Baud: 60Kbps...\r\n");
	Read_Flash_Dat();	//读取Flash数据
	printf("KJ_SN:%02X%02X%02X%02X;\r\n",Physical_ADD[0],Physical_ADD[1],Physical_ADD[2],Physical_ADD[3]);
	printf("FM1702_Key:%02X%02X%02X%02X%02X%02X; %02d;\r\n",FM1702_Key[0],FM1702_Key[1],FM1702_Key[2],FM1702_Key[3],FM1702_Key[4],FM1702_Key[5],FM1702_Key[6]);
	printf("WaterCost:0.%03d; CostNum:%02d; g_IAP_Flag:0x%02X;\r\n",WaterCost,CostNum,g_IAP_Flag);
    if(g_IAP_Flag == 0xAA)	//更新标志
	{
		g_IAP_Flag = 0x00;	//清更新标志
		Write_Flash_Dat();
		printf("g_IAP_Flag:0x%02X;\r\n",g_IAP_Flag);
	}
	BspTm1639_Show(0x01,0x00);
	ShowFlag = 0xAA;	//交替显示标志,0xAA为交替显示
	gLogNum = Search_LogNum();	printf("gLogNum:%d;\r\n",gLogNum);
    Write_FlagStatusLogDat();	//写入复位状态
//	gLogNum = 573;
//	Show_LogDat(gLogNum); //显示最后一页日志
//	Logic_ADD = 2;	    //测试使用
//	PowerUpFlag = 0xAA;	//测试使用
	srand((Physical_ADD[2]<<8)|(Physical_ADD[3]));	//使用物理地址后二位作为种子
	while(Logic_ADD==0)	//逻辑地址为0时，表示该设备未注册，进入等待注册过程
	{
		if((PowerUpFlag==0xAA)||(PowerUp_Count>0))	//收到注册广播 或 上电计数10次
		{
			if(DelayCount==0)
			{				
				DelayCount = rand()% 100;  //产生0-99的随机数
				if(Logic_ADD==0)
 				{
					if(PowerUp_Count>0)		//上电注册
					{
						Package_Send(0xB3,(u8 *)Physical_ADD);	
						PowerUp_Count--;
					}
					else
					{
						PowerUp_Count = 0;
						if(PUSendCount<30)	{	Package_Send(0xB3,(u8 *)Physical_ADD);	PUSendCount++;	Delay(0xFF);}	
						else				{	PUSendCount = 200;	}
					}
				}
			}				
		}

		if( g_RxMessFlag == 0xAA )//接收到有数据
		{
			if((g_RxMessage[0]==0xA1)&&(g_RxMessage[1]==0xA1))	{	PowerUpFlag=0xAA;	}//进入随机延时未注册回复
			g_RxMessFlag = 0x00;
		}
		
		if( ReadRFIDCard() == FM1702_OK )						//只用于检测是否有卡，不校验密码
		{	BspTm1639_Show(0x06,0x01);	Delay(1500);	}		//显示代码表示到卡 ---XX--- //软件版本号
		else
		{
			if(ShowCount<148)			BspTm1639_ShowSNDat(0x11,(u8 *)Physical_ADD);
			else if(ShowCount<150)		BspTm1639_Show(0x00,0x00);		//关显示
			else if(ShowCount<298)		BspTm1639_Show(0x01,0x00);		//显示方框
			else						BspTm1639_Show(0x00,0x00);		//关显示			
		}
		if(Flash_UpdateFlag == 0xAA)
		{
			Write_Flash_Dat();
			Flash_UpdateFlag = 0;
			if(g_IAP_Flag == 0xAA)		SoftReset();//更新标志 软件复位
		}
		if(g_LoseContact>200)		SoftReset();//失联 软件复位 10秒钟累加一；在定时器累加
	}
	CAN_DeInit(CAN1);
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN初始化正常模式,波特率60Kbps  //则波特率为:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	ShowCount = 0;g_RxMessFlag = 0x00;	
	BspTm1639_Show(0x03,WaterCost);	//显示每升水金额值
    printf("--------------------------Logic_ADD:0x%02X------------------------------\r\n",Logic_ADD);
	bsp_StartTimer(1, 200);			//定时器1周期 200毫秒
	
	for(f=0;f<15;f++)
	{
		CQ_Buf1[ 0]=f;CQ_Buf1[ 1]=f;CQ_Buf1[ 2]=f;CQ_Buf1[ 3]=f;CQ_Buf1[ 4]=f;CQ_Buf1[ 5]=f;CQ_Buf1[ 6]=f;CQ_Buf1[ 7]=f;
		CQ_Buf1[ 8]=f;CQ_Buf1[ 9]=f;CQ_Buf1[10]=f;CQ_Buf1[11]=f;CQ_Buf1[12]=f;CQ_Buf1[13]=f;CQ_Buf1[14]=f;CQ_Buf1[15]=f;
		bsp_PutCQ((uint8_t *)CQ_Buf1);
		printf(">>bsp_PutCQ:%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X.\r\n",
		CQ_Buf1[ 0],CQ_Buf1[ 1],CQ_Buf1[ 2],CQ_Buf1[ 3],CQ_Buf1[ 4],CQ_Buf1[ 5],CQ_Buf1[ 6],CQ_Buf1[ 7],
		CQ_Buf1[ 8],CQ_Buf1[ 9],CQ_Buf1[10],CQ_Buf1[11],CQ_Buf1[12],CQ_Buf1[13],CQ_Buf1[14],CQ_Buf1[15]);
		if(f>6)
		{
			CQ_Start = bsp_GetCQ((uint8_t *)CQ_Buf2);
			if(CQ_Start==CQ_OK)
			{
				printf("<<bsp_GetCQ:%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X.\r\n",
				CQ_Buf2[ 0],CQ_Buf2[ 1],CQ_Buf2[ 2],CQ_Buf2[ 3],CQ_Buf2[ 4],CQ_Buf2[ 5],CQ_Buf2[ 6],CQ_Buf2[ 7],
				CQ_Buf2[ 8],CQ_Buf2[ 9],CQ_Buf2[10],CQ_Buf2[11],CQ_Buf2[12],CQ_Buf2[13],CQ_Buf2[14],CQ_Buf2[15]);
			}
		}
	}
	for(f=0;f<10;f++)
	{
		CQ_Start = bsp_GetCQ((uint8_t *)CQ_Buf2);
		if(CQ_Start==CQ_OK)
		{
			printf("<<f=%2d;bsp_GetCQ:%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X.\r\n",f,
			CQ_Buf2[ 0],CQ_Buf2[ 1],CQ_Buf2[ 2],CQ_Buf2[ 3],CQ_Buf2[ 4],CQ_Buf2[ 5],CQ_Buf2[ 6],CQ_Buf2[ 7],
			CQ_Buf2[ 8],CQ_Buf2[ 9],CQ_Buf2[10],CQ_Buf2[11],CQ_Buf2[12],CQ_Buf2[13],CQ_Buf2[14],CQ_Buf2[15]);
		}
	}
	
	while (1)
	{
		CPU_IDLE();
		if ( bsp_CheckTimer(1) )	//软定时器
		{
			bsp_StartTimer(1, 200);	//启动下个定时周期/
			if(Time_250ms<9)	Time_250ms++;
			else	
			{	
				Time_250ms=0;	
                if(g_RunningTime<100)   g_RunningTime = 0;
                else                    g_RunningTime++;	
			}
			if((gErrorDat[0]==0x30)||(gErrorDat[0]==0x31)||(gErrorDat[0]==0x32)||(gErrorDat[0]==0x33))	//异常情况
			{//01.显示异常代码
				ShowFlag = 0x00;	//关交替显示标志,只显示异常代码
				ErrorTemp[0] = ((gErrorDat[1]-0x30)<<4)|(gErrorDat[2]-0x30);
				ErrorTemp[1] = gErrorDat[0]-0x30;
				BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);
				InPutCount = 0;
				OutFlag = 0x00;	OutPut_OFF();	//关闭水阀，放水
			}
			else
			{
				if( ReadRFID() == FM1702_OK )   {CardUnReadFlag = 0;    /*printf("CardUnReadFlag=%02X.MI_OK-0!\r\n",CardUnReadFlag);	*/}//清零 未读到卡计数
				else if(CardUnReadFlag<100)	    {CardUnReadFlag++;      /*printf("CardUnReadFlag=%02X.\r\n",CardUnReadFlag);	        */}//累计 未读到卡计数
				if( (CardUnReadFlag<3) )	//读到卡
				{
					if((OldeUID[0]!=UID[0])||(OldeUID[1]!=UID[1])||(OldeUID[2]!=UID[2])||(OldeUID[3]!=UID[3]))
					{	//卡号不一致
						if(0x00 != CardInFlag)	//刚拔卡
						{
							MemoryBuffer[0] = 0x55;				//卡刚拔出
							MemoryBuffer[1] = OldeUID[0];		//卡号0；
							MemoryBuffer[2] = OldeUID[1];		//卡号1；
							MemoryBuffer[3] = OldeUID[2];		//卡号2；
							MemoryBuffer[4] = OldeUID[3];		//卡号3；
							MemoryBuffer[5] = RFID_Money>>16;	//数据域5 金额1高位
							MemoryBuffer[6] = RFID_Money>>8;	//数据域6 金额2
							MemoryBuffer[7] = RFID_Money;		//数据域7 金额3
							MemoryBuffer[8] = RFID_Money>>24;	//数据域8 校验
							MemoryBuffer[9] = DecSum>>8;		//DecSum递减和 高位
							MemoryBuffer[10] = DecSum;			//DecSum递减和 低位
							printf(">>Main_Lin315,拔卡动作.插入:%08X(%d.%03d),扣款:%d*%02d,拔卡:%08X(%d.%03d).\r\n\r\n--------------\r\n",\
                            Premoney,Premoney/1000,Premoney%1000,DecSum,WaterCost,RFID_Money,(RFID_Money&0x00FFFFFF)/1000,(RFID_Money&0x00FFFFFF)%1000);
							PutInMemoryBuf((u8 *)MemoryBuffer);	
							MemoryBuffer[0] = 0x00;
							gErrorDat[0]=0;	gErrorDat[1]=0;	gErrorDat[2]=0;	gErrorDat[3]=0;	gErrorDat[4]=0;	gErrorDat[5]=0;
						}
						OldCardInFlag = 0x00;	CardInFlag = 0x00;
						OldeUID[0]=UID[0]; OldeUID[1]=UID[1];	OldeUID[2]=UID[2];	OldeUID[3]=UID[3];
						Premoney = 0;	DecSum = 0;
						CardUnReadFlag=0xAA;	//标志改为未读到卡
						OutFlag = 0x00;			//放水标志 0x00->停止放水
						RFID_Count=0; 	InPutCount = 0;
						continue;
					}
					if(RFID_Count<1)//1*200=200ms
					{
						printf("\r\n*************************************************************\r\n");
						ReadStatus = ReadMoney(FM1702_Key[6],_MoneyTemp,_ErrTemp);	//读取卡内金额 使用地址块FM1702_Key[6]						
						RFID_MoneyTemp = ((u32)_MoneyTemp[0]<<24);					//服务器通信使用
						RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[1]<<16);	//金额1 高位
						RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[2]<<8);	//金额2 
						RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[3]);		//金额3
						printf(">>Main_Lin340,插卡动作,插卡金额:%08X(%d.%03d).CardUnReadFlag=%02X.\r\n",\
                        RFID_MoneyTemp,(RFID_MoneyTemp&0x00FFFFFF)/1000,(RFID_MoneyTemp&0x00FFFFFF)%1000,CardUnReadFlag);
						RFID_Money = RFID_MoneyTemp;
						if(ReadStatus == Money_OK);				//{RFID_Money = RFID_MoneyTemp;		}	//读卡正确
						else if(ReadStatus == Money_Error_E700)	{gErrorShow = _ErrTemp[0];			}	//E700
						else if(ReadStatus == Money_Error_E000)	{gErrorShow = _ErrTemp[0];			}
						else	{  RFID_Count=0; continue; 	}	//读卡错误 退出重新读卡
						OldRFID_Money = RFID_Money;				
						CardInFlag = 0xAA;	//读卡成功
						ShowFlag = 0x00;	//关交替显示标志,只显示异常代码						
						BspTm1639_Show(0x00,0x00);		//关显示
					}
					else if(RFID_Count<6)	//6*200=1200ms
					{	//显示原卡内金额, 卡插入0.4秒时，提交插卡动作
						if(ReadStatus == Money_Error_E700)	//读出数据大于金额值为用户异常代码 E700
						{	//E700不提交插卡动作
							InPutCount = 0;		OutPut_OFF();		//关闭水阀，停止放水
							RFID_Count=200;		ErrorTemp[1] = 0x07;	ErrorTemp[0] = 0x00;
							BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);//显示异常代码						
						}
						else if(ReadStatus == Money_Error_E000)	//用户异常代码 E000
						{
							InPutCount = 0;	OutPut_OFF();		//关闭水阀，停止放水
							if(UseErrFlag==0x00)
							{
								ErrorTemp[0] = gErrorShow%10;	ErrorTemp[1] = gErrorShow/10;	//百位，个位
								BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);//显示异常代码
							}
							else
							{
								ErrorTemp[0] = ((gErrorDat[1]-0x30)<<4)|(gErrorDat[2]-0x30);	//十位，个位
								ErrorTemp[1] = gErrorDat[0]-0x30;	        //百位
								BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);  //显示异常代码
							}
							CardInFlag = 0xAA;	//读卡成功	
							if(OldCardInFlag != CardInFlag)	//刚插入
							{
								MemoryBuffer[0] = 0xAA;		//卡刚插入
								MemoryBuffer[1] = UID[0];	//卡号0；
								MemoryBuffer[2] = UID[1];	//卡号1；
								MemoryBuffer[3] = UID[2];	//卡号2；
								MemoryBuffer[4] = UID[3];	//卡号3；
								MemoryBuffer[5] = RFID_Money>>16;	//数据域5 金额1高位
								MemoryBuffer[6] = RFID_Money>>8;	//数据域6 金额2
								MemoryBuffer[7] = RFID_Money;		//数据域7 金额3
								MemoryBuffer[8] = RFID_Money>>24;	//数据域8 校验
								MemoryBuffer[9] = DecSum>>8;		//DecSum递减和 高位
								MemoryBuffer[10] = DecSum;			//DecSum递减和 低位
								PutInMemoryBuf((u8 *)MemoryBuffer);
								Premoney = RFID_Money;	//保存插卡金额
								MemoryBuffer[0] = 0x00;	
								OldCardInFlag = CardInFlag;
							}							
						}
						else	//读出数据为正常金额,提交插卡动作
						{
							CardInFlag = 0xAA;	//读卡成功								
							BspTm1639_Show(0x04,RFID_Money);	//计费扣款模式
							if(OldCardInFlag != CardInFlag)	//刚插入
							{
								MemoryBuffer[0] = 0xAA;		//卡刚插入
								MemoryBuffer[1] = UID[0];	//卡号0；
								MemoryBuffer[2] = UID[1];	//卡号1；
								MemoryBuffer[3] = UID[2];	//卡号2；
								MemoryBuffer[4] = UID[3];	//卡号3；
								MemoryBuffer[5] = RFID_Money>>16;	//数据域5 金额1高位
								MemoryBuffer[6] = RFID_Money>>8;	//数据域6 金额2
								MemoryBuffer[7] = RFID_Money;		//数据域7 金额3
								MemoryBuffer[8] = RFID_Money>>24;	//数据域8 校验
								MemoryBuffer[9] = DecSum>>8;		//DecSum递减和 高位
								MemoryBuffer[10] = DecSum;			//DecSum递减和 低位
								PutInMemoryBuf((u8 *)MemoryBuffer);
								Premoney = RFID_Money;	//保存插卡金额
								MemoryBuffer[0] = 0x00;	
								OldCardInFlag = CardInFlag;
							}							
						}						
						
					}
					else if(RFID_Count<12)//12*200=2400ms
					{	//延时放水
						if( ReadStatus== Money_OK)	
						{
							BspTm1639_Show(0x04,RFID_Money);	//计费扣款模式	
						}
						else	//用户异常代码		
						{
							InPutCount = 0;	OutPut_OFF();		//关闭水阀，放水
							if(UseErrFlag==0x00)
							{
								ErrorTemp[0] = gErrorShow%10;	ErrorTemp[1] = gErrorShow/10;	//个位//百位
								BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);//01.显示异常代码
							}							
							else
							{
								ErrorTemp[0] = ((gErrorDat[1]-0x30)<<4)|(gErrorDat[2]-0x30);
								ErrorTemp[1] = gErrorDat[0]-0x30;
								BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);//01.显示异常代码
							}
						}
					}
					

					if(ReadStatus!=Money_OK)	//异常代码
					{
						if(gErrorDat[0]!=0x00)
						{
							InPutCount = 0;	OutPut_OFF();		//关闭水阀，放水
							ErrorTemp[0] = gErrorDat[4]%10;
							ErrorTemp[1] = gErrorDat[4]/10;
							BspTm1639_ShowSNDat(0x12,(u8 *)ErrorTemp);//01.显示异常代码
						}
						if(OldCardInFlag != CardInFlag)	
						{
							MemoryBuffer[0] = 0xAA;		//卡刚插入
							MemoryBuffer[1] = UID[0];	//卡号0；
							MemoryBuffer[2] = UID[1];	//卡号1；
							MemoryBuffer[3] = UID[2];	//卡号2；
							MemoryBuffer[4] = UID[3];	//卡号3；
							MemoryBuffer[5] = RFID_Money>>16;	//数据域5 金额1高位 
							MemoryBuffer[6] = RFID_Money>>8;	//数据域6 金额2	
							MemoryBuffer[7] = RFID_Money;		//数据域7 金额3	
							MemoryBuffer[8] = RFID_Money>>24;	//数据域8 校验
							MemoryBuffer[9] = DecSum>>8;		//DecSum递减和 高位
							MemoryBuffer[10] = DecSum;			//DecSum递减和 低位
							PutInMemoryBuf((u8 *)MemoryBuffer);
							MemoryBuffer[0] = 0x00;
							OldCardInFlag = CardInFlag;
						}
					}
					else if((RFID_Count>12)&&((RFID_Money&0x00FFFFFF)>=WaterCost))	//卡内金额大于最小扣款基数
					{
						if(OutFlag == 0x00)	
						{
							OutFlag = 0xAA;	//放水标志
							ReadStatus = DecMoney(FM1702_Key[6],WaterCost,_MoneyTemp);
							RFID_MoneyTemp = ((u32)_MoneyTemp[0]<<24);					//服务器通信使用
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[1]<<16);	//金额1 高位
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[2]<<8);	//金额2 
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[3]);		//金额3
							if(ReadStatus == Money_OK)	{	RFID_Money = RFID_MoneyTemp;	}	//读卡正确
							else	{ RFID_Money = OldRFID_Money; RFID_Count=0; continue; 	}	//读卡错误 退出重新读卡
							InPutCount = 0; DecSum = 1;	//递减和=1; 开始放水时扣款
							printf(">>Main_Lin460,放水标志,DecSum=%02d.Money=%d.%03d\r\n",DecSum,(RFID_Money&0x00FFFFFF)/1000,(RFID_Money&0x00FFFFFF)%1000);
							OldRFID_Money = RFID_Money; 
							Beforeupdate = WaterCost;	//更新前扣费 初次扣费
							OvertimeCount=0;OvertimeMinutes=0;//有扣费，清超时标志
						}
						if( InPutCount >= CostNum )
						{
                            printf("nPutCount=InPutCount-CostNum: %d-%d ",InPutCount,CostNum);
							InPutCount = InPutCount - CostNum;
                            printf("= %d.\r\n",InPutCount);
                            if( InPutCount > CostNum )  InPutCount = 0;
							OvertimeCount=0;	OvertimeMinutes=0;//有扣费，清超时标志
							ReadStatus = DecMoney(FM1702_Key[6],WaterCost,_MoneyTemp);
							RFID_MoneyTemp = ((u32)_MoneyTemp[0]<<24);					//服务器通信使用
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[1]<<16);	//金额1 高位
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[2]<<8);	//金额2 
							RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[3]);		//金额3
							if(ReadStatus == Money_OK)		{RFID_Money = RFID_MoneyTemp;		}	//读卡正确
							else	{ RFID_Money = OldRFID_Money; RFID_Count=0; continue; 	}	//读卡错误 退出重新读卡
							OldRFID_Money = RFID_Money;
							DecSum++;	//递减和+1;
							printf(">>432,DecSum=%02d.\r\n",DecSum);
							if( (DecSum%DRCCYCLE) == 0 )		//20个计费周期时，提交一次拔卡动作
							{
								MemoryBuffer[0] = 0x55;					//卡刚拔出
								MemoryBuffer[1] = OldeUID[0];			//卡号0；
								MemoryBuffer[2] = OldeUID[1];			//卡号1；
								MemoryBuffer[3] = OldeUID[2];			//卡号2；
								MemoryBuffer[4] = OldeUID[3];			//卡号3；
								MemoryBuffer[5] = RFID_Money>>16;		//数据域5 金额1高位
								MemoryBuffer[6] = RFID_Money>>8;		//数据域6 金额2
								MemoryBuffer[7] = RFID_Money;			//数据域7 金额3
								MemoryBuffer[8] = RFID_Money>>24;		//数据域8 校验
								MemoryBuffer[9] = DecSum>>8;			//DecSum递减和 高位
								MemoryBuffer[9] = MemoryBuffer[9]|0x80;	// 加入连续消息标记
								MemoryBuffer[10] = DecSum;				//DecSum递减和 低位
								printf(">>Main_Lin489,DRCCYCLE个计费周期,插卡金额:%08X,扣款次数:%d*%02d,金额:%08d(%d).\r\n",Premoney,DecSum,WaterCost,RFID_Money,(RFID_Money&0x00FFFFFF));
								PutInMemoryBuf((u8 *)MemoryBuffer);
							}
							if( (RFID_Money&0x00FFFFFF) != ((Premoney&0x00FFFFFF)-DecSum*WaterCost) )
							{	RFID_Count=0;	WriteMoney(FM1702_Key[6],Premoney,0);	continue;	}	//扣费出错，将返回插卡金额并重新进入扣费操作
							if(ErrorMoney==RFID_Money)	
							{	RFID_Count=0; 	WriteMoney(FM1702_Key[6],Premoney,0);	continue;	}	//金额出错，将返回插卡金额并重新进入扣费操作
							if(RFID_Money != 0x00)	//防止这个时间段拔出卡，造成数据为0
							{
								OldRFID_Money = RFID_Money;
								if(BeforeFlag == 0xAA)	Beforeupdate = Beforeupdate + WaterCost;	//更新前扣费
								if(Beforeupdate>(10*WaterCost))	Beforeupdate = 10*WaterCost;	
							}
							else	{	RFID_Money = OldRFID_Money;	}
						}
						BspTm1639_Show(0x04,RFID_Money);			//计费扣款模式
						if((OvertimeMinutes == 0xAA)||(DecSum>=200))
                            OutPut_OFF();	//关闭水阀，停止放水
						else						
                            OutPut_ON();	//打开水阀，放水
					}
					else if((RFID_Money&0x00FFFFFF)<WaterCost)
					{
						OutPut_OFF();	//关闭水阀，停止放水
					}
					
					if(RFID_Count<100)	RFID_Count++;	//100*200=10,000ms
					
					if((re_RxMessage[4]!=0)&&(re_RxMessage[4]==re_RxMessage[9])&&(re_RxMessage[10]==CRC8_Table(re_RxMessage,10)))	//CRC校验
					{
						if((re_RxMessage[0]==0)&&(re_RxMessage[1]==0)&&(re_RxMessage[2]==0)&&(re_RxMessage[3]==0))
						{
							printf(">>Main_Lin582,服务器数据包返回卡号:00000000.\r\n");
							for(i=0;i<16;i++)	re_RxMessage[i] = 0x00;
						}
						else
						{
							if((re_RxMessage[0] == UID[0])&&(re_RxMessage[1] == UID[1])&&\
							   (re_RxMessage[2] == UID[2])&&(re_RxMessage[3] == UID[3])&&\
							   (re_RxMessage[4] == FlagBit))
							{
								u32TempDat = (((u32)re_RxMessage[5]<<16)|((u32)re_RxMessage[6]<<8)|re_RxMessage[7]);	//读取云端发回数据
								Premoney = u32TempDat;	//保存插卡金额
								if((u32TempDat<=WaterCost)||(Beforeupdate>=u32TempDat))	{	u32TempDat = 0x00;	RFID_Count=0;		}//金额为0，直接写入0；
								else													{	u32TempDat = u32TempDat-Beforeupdate;	}
								u32TempDat = u32TempDat | ((u32)re_RxMessage[8]<<24);
								printf(">>Main_Lin595,服务器数据包.金额:%08X(%d.%03d).\r\n",u32TempDat,(u32TempDat&0x00FFFFFF)/1000,(u32TempDat&0x00FFFFFF)%1000);
								if( WriteMoney(FM1702_Key[6],u32TempDat,0) == FM1702_OK )	//写入金额 成功
								{
									Beforeupdate = 0x00;	//清空更新数据前的扣费金额	
									BeforeFlag = 0x00;		//数据更新完成
									Delay(0xFF);			//延时
									ReadStatus = ReadMoney(FM1702_Key[6],_MoneyTemp,_ErrTemp);	//读取卡内金额 使用地址块FM1702_Key[6]						
									RFID_MoneyTemp = ((u32)_MoneyTemp[0]<<24);					//服务器通信使用
									RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[1]<<16);	//金额1 高位
									RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[2]<<8);	//金额2 
									RFID_MoneyTemp = RFID_MoneyTemp|((u32)_MoneyTemp[3]);		//金额3
									if(ReadStatus == Money_OK)	{RFID_Money = RFID_MoneyTemp;		}	//读卡正确
									else	{ RFID_Money = OldRFID_Money; RFID_Count=0; continue; 	}	//读卡错误 退出重新读卡
									if(RFID_Money<WaterCost)	RFID_Count=0;	//返回金额小于最小扣款金额
									BspTm1639_Show(0x00,0x00);		Delay(0x2FFF); 	//关显示，延时闪烁一下
									BspTm1639_Show(0x04,RFID_Money);		//计费扣款模式	
									OldRFID_Money = RFID_Money;
								}
							}
							for(i=0;i<16;i++)	re_RxMessage[i] = 0x00;
						}

					}
					if((gErrorDat[0]==UID[0])&&(gErrorDat[1]==UID[1])&&(gErrorDat[2]==UID[2])&&(gErrorDat[3]==UID[3])&&(gErrorDat[5]==FlagBit))	//异常情况
					{
						WriteMoney(FM1702_Key[6],Premoney,gErrorDat[4]);	//写入异常标志量
						gErrorDat[5] = 0xAA;//防止再次进入，重新写卡数据
					}
					
				}
				else	//没有读到卡
				{
					UID[0]=0;	UID[1]=0;	UID[2]=0;	UID[3]=0;//清卡号
					CardUnReadFlag=0xAA;RFID_Count=0;
					if((gErrorDat[4]>=40))	//用于卡相关或用户相关，在卡取走后，清异常代码防止异常代码闪烁
					{gErrorDat[0]=0;	gErrorDat[1]=0;	gErrorDat[2]=0;	gErrorDat[3]=0;	gErrorDat[4]=0;	gErrorDat[5]=0;}	
					ShowFlag = 0xAA;	//交替显示标志,0xAA为交替显示
					UseErrFlag = 0x00;	//清用户或卡异常标志
					CardInFlag = 0x00;
					if(OldCardInFlag != CardInFlag)	//刚拔卡
					{
						MemoryBuffer[0] = 0x55;				//卡刚拔出
						MemoryBuffer[1] = OldeUID[0];		//卡号0；
						MemoryBuffer[2] = OldeUID[1];		//卡号1；
						MemoryBuffer[3] = OldeUID[2];		//卡号2；
						MemoryBuffer[4] = OldeUID[3];		//卡号3；
						MemoryBuffer[5] = RFID_Money>>16;	//数据域5 金额1高位
						MemoryBuffer[6] = RFID_Money>>8;	//数据域6 金额2
						MemoryBuffer[7] = RFID_Money;		//数据域7 金额3
						MemoryBuffer[8] = RFID_Money>>24;	//数据域8 校验
						MemoryBuffer[9] = DecSum>>8;		//DecSum递减和 高位
						MemoryBuffer[10] = DecSum;			//DecSum递减和 低位
						printf(">>Main_Lin562,拔卡动作.插入金额:%08X,扣款次数:%d*%02d,拔卡金额:%08X,(%d).\r\n\r\n--------------\r\n",\
						Premoney,DecSum,WaterCost,RFID_Money,RFID_Money&0x00FFFFFF);
						if((RFID_Money&0x00FFFFFF) > 0x00EA0000);	//错误异常时，不提交拔卡动作
						else 	{	PutInMemoryBuf((u8 *)MemoryBuffer);	Premoney=0;	DecSum=0;	}
						MemoryBuffer[0] = 0x00;		
						OldCardInFlag = CardInFlag;
					}
					Beforeupdate = 0x00;	BeforeFlag = 0xAA;
					InPutCount = 0;
					OvertimeCount=0;OvertimeMinutes=0;  //有扣费，清超时标志
					OutFlag = 0x00;		OutPut_OFF();	//关闭水阀，放水
					if(ShowCount<148)			BspTm1639_Show(0x03,WaterCost);	//显示每升水金额值
					else if(ShowCount<150)		BspTm1639_Show(0x00,0x00);		//关显示
					else if(ShowCount<298)		BspTm1639_ShowSNDat(0x11,(u8 *)Physical_ADD);
					else						BspTm1639_Show(0x00,0x00);		//关显示	
					if(ShowCount==200)			{	SoftReset1702();	}		//检测RFID模块
				}
			
			}

			if(Flash_UpdateFlag == 0xAA)
			{
				Write_Flash_Dat();
				Flash_UpdateFlag = 0;
				if(g_IAP_Flag == 0xAA)		SoftReset();//更新标志 软件复位
			}
			if((g_LoseContact>200)&&(gErrorDat[0]==0x00))
			{				
				SoftReset();//失联 软件复位 10秒钟累加一；在定时器累加
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
	//NVIC_Configuration();
	
	Init_GPIO();		//输出初始化
	#if IAP_Flag
	#else
		bsp_InitUart(); 	    //初始化串口,因为IAP中初始过
	#endif
	BspTm1639_Config();	        //TM1639初始化
	BspFM1701_Config();	        //FM1701 GPIO初始化
	TIM3_Int_Init(1999,720-1);  //以100khz的频率计数，0.01ms中断，计数到2000 为20ms 
	TIM2_Cap_Init(0xFFFF,72-1);	//以1Mhz的频率计数 
 	Adc_Init();		  		    //ADC初始化
	bsp_InitCQVar();
}


//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //TIM3更新中断 20ms中断
	{
		if( ShowFlag == 0xAA )	//更新显示
		{
			if( ShowCount < 300 )  	ShowCount++;	//300*20=6000，6秒一个显示周期
			else					ShowCount = 0;
			
			if( DelayCount > 0 )	{	if( (ShowCount%2)==0 )	DelayCount--;	}	//20ms中断	20*2=40ms
		}
		else    ShowCount = 0;
		
		//定时时间 10min内无流量计费关阀门
		if(OvertimeCount<3000)	OvertimeCount++;	//超时计数，60s=60,000ms=3000*20ms
		else
		{
			OvertimeCount=0;
			if(OvertimeMinutes<10)	OvertimeMinutes++;	//超时10分钟
			else				  	OvertimeMinutes = 0xAA;//超时标志
		}
		if(Time20msCount<10)	Time20msCount++;		//0.2秒 10 *20ms = 200ms
		else	
		{	
			Time20msCount=0;
			if(g_LoseContact<100)	g_LoseContact++;	//失联计数，0.2秒*100 = 20Sec；
			else					g_LoseContact=255;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 					
	}
}

void Delay(__IO uint16_t nCount)
{
	while (nCount != 0)
	{
		nCount--;
	}
}



