#include "stm32f10x.h"		
#include "bsp_cs5463.h"		
#include "delay.h"
#include "bsp.h"

static UN32 CS546x_Buff;					//CS5463缓冲区
PhaseHandle Board_Phase;

/*static void BspInput_CS5463_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
}*/

void Bsp_CS5463_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: CS546x_Read_Byte
*	功能说明: 读取寄存器一个字节数据
*	形     参: 无
*	返 回 值: 获取到的一个字节数据 
*	备     注：
*********************************************************************************************************
*/
u8 CS546x_Read_Byte(void)
{	
	u8 ReceiveDat,i;	
	ReceiveDat = 0;
	CS546x_SET_SDI;
//	delay_us(10);
	for(i=0;i<7;i++) 
	{
		CS546x_RESET_CLK;
		delay_us(5);
		CS546x_SET_CLK;
		delay_us(5);
		if(CS546x_Get_SDO)	ReceiveDat |= 1;
		ReceiveDat <<= 1;	
	}	
	return ReceiveDat;
}
/*
*********************************************************************************************************
*	函 数 名: CS546x_Write_Byte
*	功能说明: 向寄存器中写一个字节数据
*	形     参: 要写的单字节数据
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS546x_Write_Byte(u8 WrDat)
{
	u8  tmp_8u,i;
	tmp_8u = WrDat;
	for(i=0;i<8;i++)
	{
		delay_us(10);
		CS546x_RESET_CLK;
		if((tmp_8u&0x80)==0)	CS546x_RESET_SDI;
		else					CS546x_SET_SDI;
		delay_us(2);
		tmp_8u <<= 1;
		CS546x_SET_CLK;
	}
	delay_us(10);
	CS546x_RESET_CLK;
}
/*
*********************************************************************************************************
*	函 数 名: CS546x_RedReg
*	功能说明: 向读取寄存器3个字节数据
*	形     参: RegAddr,寄存器地址;DustBuf,读出数据的缓存;
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS546x_RedReg(u8 RegAddr,u8 *DustBuf)
{
	u8 i;	
	CS546x_RESET_CS;
	delay_us(10);
	CS546x_Write_Byte(RegAddr);
	for(i=0;i<3;i++)
	{
		*(DustBuf+i) = CS546x_Read_Byte();
	}
	CS546x_SET_CS;

}

/*
*********************************************************************************************************
*	函 数 名: CS546x_WriteReg
*	功能说明: 向寄存器中写3字节数据
*	形     参: RegAddr,寄存器地址;DustBuf，要写的单字节数据;
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS546x_WriteReg(u8 RegAddr,u8 *DustBuf)
{
	u8 i;	
	CS546x_RESET_CS;
	delay_us(10);
	CS546x_Write_Byte(RegAddr);
	for(i=0;i<3;i++)
	{
		CS546x_Write_Byte(*(DustBuf+i));
	}
	CS546x_SET_CS;

}

/*
*********************************************************************************************************
*	函 数 名: CS546x_ResetStaReg
*	功能说明: 清除状态寄存器标志
*	形     参: 无
*	返 回 值: 无 
*	备     注：写1有效，写0无效，不做任何处理
*********************************************************************************************************
*/
void CS546x_ResetStaReg()
{
	CS546x_Buff.u8[0] = 0xFF;
	CS546x_Buff.u8[1] = 0xFF;
	CS546x_Buff.u8[2] = 0xFF;

	CS546x_WriteReg(Status | REG_WR,CS546x_Buff.u8);
}



#define READ_MASK		0xBF	//读寄存器时的屏蔽码，与（写）地址相与
#define CMD_SYNC0    	0XFE  	//结束串口重新初始化 
#define CMD_SYNC1    	0XFF  	//开始串口重新初始化
#define REG_CONFR   	0x40   	//配置 
#define REG_CYCCONT 	0x4A   	//一个计算周期的A/D转换数 
#define REG_STATUSR 	0x5E 	//状态 
#define REG_MODER   	0x64  	//操作模式 
#define REG_MASKR   	0x74  	//中断屏蔽 
#define REG_CTRLR   	0x78  	//控制 
#define CMD_STARTC   	0XE8  	//执行连续计算周期

#define REG_VRMSR   	0X18  	//VRMS
#define REG_IRMSR   	0X16  	//IRMS
#define REG_Pactive   	0X14  	//Pactive


static u8 RX_Buff[4];					//CS5463读写缓冲区
u8 sta;									//芯片状态

/*
*********************************************************************************************************
*	函 数 名: CS5463CMD
*	功能说明: 发送命令到芯片
*	形     参: 无
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
static void CS5463CMD(u8 cmd)
{
    u8 i;
    CS546x_SET_CLK;		//SCLK = 1;
    CS546x_RESET_CS;	//CS = 0;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(cmd&0x80)    CS546x_SET_SDI;		//MOSI = 1;
        else		    CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;			 		//在时钟上升沿，数据被写入CS5463
        cmd <<= 1;
        i++;
    }
    delay_us(50);		//uDelay(50);
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
}


/*
*********************************************************************************************************
*	函 数 名: CS5463WriteReg
*	功能说明: 更改寄存器数据
*	形     参: addr->寄存器地址;*p->数据内容;
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS5463WriteReg(u8 addr,u8 *p)
{	
    u8 i,j;
    u8 dat;
    CS546x_RESET_CS;	//CS = 0;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(addr&0x80)CS546x_SET_SDI;	//MOSI = 1;
        else		 CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;	//在时钟上升沿，数据被写入CS5463
        addr <<= 1;
        i++;
    }
    j = 0;
    delay_us(50);		//uDelay(50);
    while(j<3)
    {
        dat = *(p+j);
        i = 0;
        while(i<8)
        {
            delay_us(50);		//uDelay(50);
            CS546x_RESET_CLK;	//SCLK = 0;
            if(dat&0x80)CS546x_SET_SDI;		//MOSI = 1;
            else		CS546x_RESET_SDI;	//MOSI = 0;
            delay_us(50);		//uDelay(50);
            CS546x_SET_CLK;		//SCLK = 1;   //在时钟上升沿，数据被写入CS5463
            dat <<= 1;
            i++;
        }
        delay_us(50);		//uDelay(50);
        j++;
    }
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
    delay_us(50);		//uDelay(50);	
}

/*
*********************************************************************************************************
*	函 数 名: CS5463ReadReg
*	功能说明: 获取寄存器数值
*	形     参: addr->寄存器地址;*p->数据内容;
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS5463ReadReg(u8 addr,u8 *p)
{	
    u8 i,j;
    u8 dat;
    CS546x_RESET_CS;	//CS = 0;
    addr &= READ_MASK;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(addr&0x80)CS546x_SET_SDI;	//MOSI = 1;
        else		 CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;
        addr <<= 1;	//在时钟上升沿，数据被写入CS5463
        i++;
    }

    CS546x_SET_SDI;		//MOSI = 1;
    j = 0;
    while(j<3)
    {
        i = 0;
        dat = 0;
        while(i<8)
        {
            delay_us(50);		//uDelay(50);
            CS546x_RESET_CLK;	//SCLK = 0;
            if(i==7)CS546x_RESET_SDI;	//MOSI = 0;
            else	CS546x_SET_SDI;		//MOSI = 1;
            delay_us(50);		//uDelay(50);
            dat <<= 1;			 			
            if(CS546x_Get_SDO)	dat |= 0x01;
            else				dat &= 0xFE;
            CS546x_SET_CLK;		//SCLK = 1;
            delay_us(50);		//uDelay(50);					 		
            i++;
        }
        *(p+j) = dat;
        j++;
    }
    delay_us(50);		//uDelay(50);
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
}

/*
*********************************************************************************************************
*	函 数 名: CS5463_ResetStatusReg
*	功能说明: 复位状态寄存器数值
*	形     参: 无
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
static void CS5463_ResetStatusReg(void)
{
    RX_Buff[0] = 0xFF;
    RX_Buff[1] = 0xFF;
    RX_Buff[2] = 0xFF;
    CS5463WriteReg(0x5E,RX_Buff);		//复位状态寄存器	#define REG_STATUSR 	0x5E 	//状态  
}

/*
*********************************************************************************************************
*	函 数 名: CS5463_GetStaReg_Val
*	功能说明: 获取状态寄存器数值
*	形     参: 无
*	返 回 值: 返回中断信息 
*	备     注：
*********************************************************************************************************
*/
u8 CS5463_GetStatusReg(void)
{ 
	u8 sta=0;

	CS5463ReadReg(0x1E,RX_Buff);	   //1E 是什么？   状态寄存器
	if(RX_Buff[0]&0x80)		   			//检测：电流、电压、功率测量是否完毕
	{
		//检测电流/电压是否超出范围
		//检测电流有效值/电压有效值/电能是否超出范围
		if((RX_Buff[0]&0x03)||(RX_Buff[1]&0x70))
		{
			CS5463_ResetStatusReg();		//复位状态寄存器
		}
		else
		{
			sta |= 0x01;//B0000_0001;	//这什么意思 还可以这样写吗？ PT2017-2-8   分隔符吗？ 
		}
	}

	if(RX_Buff[2]&0x80)			   	//检测：温度测量是否完毕
	{
		sta |=0x02; //B0000_0010;
	} 
	return(sta);	
}  



/*
*********************************************************************************************************
*	函 数 名: CS5463_GetStaReg_Val
*	功能说明: 获取状态寄存器数值
*	形     参: 无
*	返 回 值: 返回中断信息 
*	备     注：
*********************************************************************************************************
*/
u8 CS5463_GetStaReg_Val(void)
{
	u8 ret=0;

	CS5463ReadReg(Status | REG_RD,CS546x_Buff.u8);
	if(CS546x_Buff.u8[0] & 0x80)
	{
		//检测电流/电压是否超出范围
		//检测电流有效值/电压有效值/电能是否超出范围
		if((CS546x_Buff.u8[0]&0x03)||(CS546x_Buff.u8[1]&0x70))
		{
		 	CS546x_ResetStaReg();		//复位状态寄存器
		}else
		{
			ret |= 0x01;//B0000_0001;	//这什么意思 还可以这样写吗？ PT2017-2-8   分隔符吗？ 
		}
	}

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: CS546x_Get_Vrms
*	功能说明: 获取有效电压数据
*	形     参: DustBuf，经过处理后的数据缓存;
*	返 回 值: 无 
*	备     注：算法如下
*
*			根据一次函数关系 y=kx+b;其中y为检测电阻两端电压，x为寄存器数值/0xFFFFFF,k
*			为增益系数，b为偏移值，因此需要求出k和b值，由于寄存器每次读取都会跳动，故需要
*			多次取值求平均值以减小误差
*
*
*	检测电阻电压推导供电电压：Vres/100*5/2*640
*
*
*********************************************************************************************************
*/
u32 CS546x_Get_Vrms()
{
	float Tmp;
	UN32 ResceBuf;
	u8 DustBuf[4]={0};
	
	ResceBuf.u32 = 0;
	CS5463ReadReg(REG_RD|Vrms,DustBuf);
//	ShowHex(DustBuf,4);						//打印原始数据

	ResceBuf.u8[0] =  DustBuf[2];
	ResceBuf.u8[1] =  DustBuf[1];
	ResceBuf.u8[2] =  DustBuf[0];
	Tmp = ResceBuf.u32/16777215.0;
//	printf("result-K: %f\r\n",Tmp);		//转为小数点数值    
	Tmp = Tmp*CS5463_VScale;		//V_Coff;     //计算电压值220V*250mv/(110mv/1.414)=704.8V
//    printf("电压: %f\r\n",Tmp);		//转为小数点数值   
    Tmp *= 10;						//放大十倍
	ResceBuf.u32 = (u32)Tmp;
//	memcpy(DustBuf,ResceBuf.u8,4);

	return ResceBuf.u32;
}

/*
*********************************************************************************************************
*	函 数 名: CS546x_Get_Irms
*	功能说明: 获取有效电流数据
*	形     参: DustBuf，经过处理后的数据缓存;
*	返 回 值: 无 
*	备     注：
*
*				实测数据
*			交流电流值266ma，采样电阻电压，6.4mv，理论计算采样电阻应该是10.64mv，因此互感器存在漏磁，
*		该系数为10.64/6.4=	1.6625，该系数值需要大量测试统计调整
*
*
*			根据一次函数关系 y=kx+b;其中y为检测电阻两端电压，x为寄存器数值/0xFFFFFF,k
*			为增益系数，b为偏移8值，因此需要求出k和b值，由于寄存器每次读取都会跳动，故需要
*			多次取值求平均值以减小误差
*			
*			初步计算K值为112.74，b值为3.69，即函数为y=112.74x+3.69
*			
*			测试数据
*            Vres                寄存器数据					转换为小数
*            5.8mv				04 DC DC				0.0187
*            6.4mv				06 DC 50  				0.024022
*
*
*
*            
*********************************************************************************************************
*/
u32 CS546x_Get_Irms()
{
	float Tmp;
	UN32 ResceBuf;
    u8 DustBuf[4]={0};

    
	ResceBuf.u32 = 0;
	Tmp = 0;
	CS5463ReadReg(REG_RD|Irms,DustBuf);
	ResceBuf.u8[0] =  DustBuf[2];
	ResceBuf.u8[1] =  DustBuf[1];
	ResceBuf.u8[2] =  DustBuf[0];
	Tmp = ResceBuf.u32/16777215.0;
	Tmp = Tmp*CS5463_IScale;//I_Coff;		//计算电流值 暂时不用 
	Tmp *= 1000;
	ResceBuf.u32 = (u32)Tmp;
	
	return ResceBuf.u32;
}


/*
*********************************************************************************************************
*	函 数 名: CS546x_Init
*	功能说明: CS546x芯片初始化
*	形     参: IsReadEEP，是否使用存储器内保存的参数进行配置芯片;1，使用;0，不使用;
*	返 回 值: 无 
*	备     注：
*********************************************************************************************************
*/
void CS546x_Init(unsigned char IsReadEEP)
{
	CS546x_CLR_RST;
	delay_ms(10);
	CS546x_SET_RST;

	if(IsReadEEP)
	{			
		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//同步串口

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x0f;
		CS546x_Buff.u8[2] = 0xA0;
		CS5463WriteReg(CycleCount | REG_WR,CS546x_Buff.u8); //配置每个计算周期的A/D转换数->4000

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Mode| REG_WR,CS546x_Buff.u8);	//开启高通滤波

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x01;
		CS5463WriteReg(Cfg | REG_WR,CS546x_Buff.u8);	//配置寄存器

		CS546x_Buff.u8[0] = 0xFF;
		CS546x_Buff.u8[1] = 0xFF;
		CS546x_Buff.u8[2] = 0xFF;
		CS5463WriteReg(Status | REG_WR,CS546x_Buff.u8);//clear state reg

		CS546x_Buff.u8[0] = 0x80;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(MASK | REG_WR,CS546x_Buff.u8);	// allow int

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Ctrl | REG_WR,CS546x_Buff.u8);	//初始化控制寄存器

		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//同步串口

		CS546x_RESET_CS;
		delay_us(10);
		CS5463CMD(0xe8);  						//采用连续计算周期测量模式
		CS546x_SET_CS;
	}
	else
	{			
		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8);  			//串口重新初始化序列  ->SYNC1命令

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x01;
		CS5463WriteReg(Cfg | REG_WR,CS546x_Buff.u8);	//配置寄存器						

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x60;                       //开启电流和电压高通滤波器
		CS5463WriteReg(Mode | REG_WR,CS546x_Buff.u8);	//工作模式寄存器	
		
		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x0f;
		CS546x_Buff.u8[2] = 0xA0;
		CS5463WriteReg(CycleCount | REG_WR,CS546x_Buff.u8);	//配置每个计算周期的A/D转换数->4000

		CS546x_Buff.u8[0] = 0xFF;
		CS546x_Buff.u8[1] = 0xFF;
		CS546x_Buff.u8[2] = 0xFF;
		CS5463WriteReg(Status | REG_WR,CS546x_Buff.u8);//clear state reg

		CS546x_Buff.u8[0] = 0x80;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x80;
		CS5463WriteReg(MASK | REG_WR,CS546x_Buff.u8);	// allow int

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Ctrl | REG_WR,CS546x_Buff.u8);	//初始化控制寄存器		

		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//同步串口

		CS546x_RESET_CS;
		delay_us(10);
		CS5463CMD(0xe8);  						//采用连续计算周期测量模式,开启测量
		CS546x_SET_CS;
	}
}
//Channal 检测通道
void Get_InputValue(void)
{
	static UN32 Voltage_VA,Voltage_IA;
	u8 CS546x_Sta;
    static u8 Channal=Channal_A;
    if(Channal==Channal_A)
    {
        SetInput_CSCD4051Switch(IV_A);	//A相电压通道选择
        SetInput_IICD4051Switch(II_A);	//A相电流通道选择
        delay_us(30);					//必要延时
        CS546x_Sta	= CS5463_GetStaReg_Val();			//检测中断产生的原因
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//读有效电流值
            printf("A V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal++;
        }
    }
    else if(Channal==Channal_B)
    {
        SetInput_CSCD4051Switch(IV_B);	//B相电压通道选择
        SetInput_IICD4051Switch(II_B);	//B相电流通道选择
        delay_us(30);					//必要延时
        CS546x_Sta	= CS5463_GetStaReg_Val();			//检测中断产生的原因
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//读有效电流值
            printf("B V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal++;
        }
    }
    else if(Channal==Channal_C)
    {
        SetInput_CSCD4051Switch(IV_C);	//B相电压通道选择
        SetInput_IICD4051Switch(II_C);	//B相电流通道选择
        delay_us(30);					//必要延时
        CS546x_Sta	= CS5463_GetStaReg_Val();			//检测中断产生的原因
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//读有效电流值
            printf("C V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal=Channal_A;
        }
    }
}




