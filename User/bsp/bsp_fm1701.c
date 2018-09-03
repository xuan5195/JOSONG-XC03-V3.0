#include "stm32f10x.h"
#include "bsp_fm1701.h"
#include "bsp_crc8.h"
#include "bsp.h"

unsigned char FM1702_Buf[16];
unsigned char UID[5];			//卡号，最后一字节为校验数据
extern uint8_t FM1702_Key[7];
extern uint8_t gErrorShow;	//异常显示代码 在服务器未更新前显示使用，这样不会出现E000

u8 GetFanma(u8 dat1)
{
	u8 i,dat2=0;
	for(i=0;i<8;i++)
	{
	  	dat2<<=1;
		if((dat1&0x80)==0x00)	dat2 = dat2+1;
		dat1<<=1;
	}
	return dat2;
}

void BspFM1701_Delay(unsigned int dlength)
{ 
	unsigned int  i;
	unsigned char j;
	for (i=0;i<dlength;i++)
	{
		for (j=0;j<100;j++);
	}
}

void BspFM1701_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 

	Init_FM1702();			//FM1701初始化
}

void SPI_Send(unsigned char Data)          //摸拟SPI的发送
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		BspFM1701_Delay(1);
		if((Data&0x80)==0x80)	MOSI_SET;
		else					MOSI_CLR;
		BspFM1701_Delay(1);
		SCK_SET;
		Data<<=1;
		BspFM1701_Delay(1);
		SCK_CLR;
	}
}

unsigned char  SPI_Receve(void)         //模拟SPI有接收
{
	unsigned char i,Data=0;
	for(i=0;i<8;i++)   
	{
		BspFM1701_Delay(1);
		Data<<=1;
		if(MISO)	Data+=1;
		SCK_SET;
		BspFM1701_Delay(1);
		SCK_CLR;
		BspFM1701_Delay(1);
	}
	return Data;
}

unsigned char Read_REG(unsigned char SpiAddr)           //读寄存器
{
	unsigned char Data;
	SpiAddr<<=1;
	SpiAddr|=0x80;
	NSS_CLR;
	BspFM1701_Delay(1);
	SPI_Send(SpiAddr);
	Data=SPI_Receve();
	NSS_SET;
	BspFM1701_Delay(1);
	return Data;
}

void Write_REG(unsigned char SpiAddr,unsigned char Data)  //写寄存器
{
	SpiAddr<<=1;
	SpiAddr&=0x7f;
	NSS_CLR;
	BspFM1701_Delay(1);
	SPI_Send(SpiAddr);
	SPI_Send(Data);
	BspFM1701_Delay(1);
	NSS_SET;
	BspFM1701_Delay(1);
}
/***************************************************************
名称: Clear_FIFO                                             
功能: 该函数实现清縁FIFO的数据                                	       			                          
输入:   N/A                                                                                                                  
输出:   TRUE, FIFO被清空                                       
	FALSE, FIFO未被清空  	                                  
****************************************************************/
unsigned char Clear_FIFO(void)
{
	unsigned char i,temp;
	temp = Read_REG(Control);
	temp|=0x01;
	Write_REG(Control,temp);
	for(i=0;i<0xFF;i++)
	{
		if(Read_REG(FIFO_Length)==0)
		{
			return TRUE;
		}   
	}
	return FALSE;
}

/*********************************************************************
名称: Write_FIFO 
功能: 该函数实现向RC531的FIFO中写入x bytes数据
输入: count, 待写入字节的长度
      buff, 指向待写入数据的指针
输出：无
**********************************************************************/
void Write_FIFO(unsigned char count,unsigned char *Buff)
{
	unsigned char i;
	for (i=0;i<count;i++)
	{
		Write_REG(FIFO,*(Buff +i));
	}
}

/********************************************************************
名称: Read_FIFO   
功能: 该函数实现从RC531的FIFO中读出x bytes数据
输入: buff, 指向读出数据的指针
输出: 接收到的字节数
*******************************************************************/
unsigned char Read_FIFO(unsigned char *Buff)
{
	unsigned char i,temp;
	temp=Read_REG(FIFO_Length);
	if(temp==0)
	{
		return 0;
	}
	if(temp>=24)
	{
		temp=24;
	}
	for(i=0;i<temp;i++)
	{
		*(Buff +i) = Read_REG(FIFO);
	}
	return temp; 
}

/********************************************************************
名称: Judge_Req 
功能: 该函数实现对卡片复位应答信号的判断 
输入: *buff, 指向应答数据的指针  
输出: TRUE, 卡片应答信号正确                                 
      FALSE, 卡片应答信号错误
********************************************************************/
unsigned char Judge_Req(uchar *Buff)
{
	uchar temp1,temp2;
	temp1 = *Buff;
	temp2 = *(Buff+1);
	if((temp1 !=0x00)&&(temp2 ==0x00))
	{
		return TRUE;
	}
	return FALSE;
}

/*******************************************************************
名称: Command_Send 
功能: 该函数实现向FM1702发送命令集的功能
输入: count, 待发送命令集的长度                              
      buff, 指向待发送数据的指针                             
      Comm_Set, 命令码  
输出: TRUE, 命令被正确执行
      FALSE, 命令执行错误
******************************************************************/
uchar Command_Send(uchar count,uchar *Buff,uchar Comm_Set)
{
    uchar i;
    Write_REG(Command,Idle);   //清空当前指令。
    Clear_FIFO();
    if(count)
    {
      Write_FIFO(count,Buff);
    }
    Read_REG(FIFO_Length);
    Write_REG(Command,Comm_Set);
    for(i=0;i<0x7F;i++)	//20171020 将i<250改为i<0x7F
    {
		if(Read_REG(Command)==0)
		{
			return TRUE;
		}
    }
    return FALSE;
}

unsigned char Request(unsigned char mode)
{
    uchar a;
    Write_REG(TxControl,0x58);
    for(a=0;a<100;a++);
    Write_REG(TxControl,0x5b);
    Write_REG(CRCPresetLSB,0x63);
    Write_REG(CWConductance,0x3f);
    Write_REG(0x19,0x72);
    FM1702_Buf[0]=mode;
    Write_REG(Bit_Frame,0x07);
    Write_REG(ChannelRedundancy,3);//x03);
    Write_REG(TxControl,0x5b);
    Write_REG(Control,0x01);          //关阀加密
    
    if(Command_Send(1,FM1702_Buf,Transceive)==FALSE)
    {
		return FM1702_NOTAGERR;
    }
    Read_FIFO(FM1702_Buf);
    if(Judge_Req(FM1702_Buf)== TRUE)
    {
		return FM1702_OK;
    }
    return FM1702_REQERR;
}

/*******************************************************************
名称：KeyConvert  密钥转换格式
作用：净6位密钥转换成12位密钥格式
输入：* Uncode指向需要转换的密钥地址
    　*　coded 转拒换好的密钥存放地址。
输出：无
******************************************************************/
/*
void KeyConvert(uchar *Coding,uchar *Coded)
{
  uchar i,temp;
  for(i=0;i<6;i++)
  {
    temp=(*(Coding+i)&0x0f);
    *(Coded+i*2)=(~temp<<4)|temp;
    temp=(*(Coding+i)>>4);
    *(Coded+i*2+1)=(~temp<<4)|temp;
  }
}*/
void KeyConvert(uchar *Coding,uchar *Coded)
{
	uchar i,temp;
	for(i=0;i<6;i++)
	{
		temp=(*(Coding+i)&0x0f);
		*(Coded+i*2+1)=(~temp<<4)|temp;
		temp=(*(Coding+i)>>4);
		*(Coded+i*2)=(~temp<<4)|temp;
	}
}


/*********************************************************************
名称: Load_keyE2 
功能: 该函数实现将密码存入FM1702的keybuf中
输入：KeyCode :指向６字节的指针
输出：True: 密钥装载成功                                      
      False: 密钥装载失败 
*********************************************************************/
uchar Load_Key(uchar *Keybuf)
{
	KeyConvert(Keybuf,FM1702_Buf);
	Command_Send(12,FM1702_Buf,LoadKey);  //导入密码
	if(Read_REG(ErrorFlag)&0x40)
	{
		return FALSE;
	}
	return TRUE;
}

uchar Check_UID(void)
{
	uchar	temp;
	uchar	i;

	temp = 0x00;
	for(i = 0; i < 5; i++)
	{
		temp = temp ^ UID[i];
	}

	if(temp == 0)
	{
		return TRUE;
	}

	return FALSE;
}

void Save_UID(uchar row, uchar col, uchar length)
{
	uchar	i;
	uchar	temp;
	uchar	temp1;

	if((row == 0x00) && (col == 0x00))
	{
		for(i = 0; i < length; i++)
		{
			UID[i] = FM1702_Buf[i];
		}
	}
	else
	{
		temp = FM1702_Buf[0];
		temp1 = UID[row - 1];
		switch(col)
		{
			case 0:		temp1 = 0x00; row = row + 1; break;
			case 1:		temp = temp & 0xFE; temp1 = temp1 & 0x01; break;
			case 2:		temp = temp & 0xFC; temp1 = temp1 & 0x03; break;
			case 3:		temp = temp & 0xF8; temp1 = temp1 & 0x07; break;
			case 4:		temp = temp & 0xF0; temp1 = temp1 & 0x0F; break;
			case 5:		temp = temp & 0xE0; temp1 = temp1 & 0x1F; break;
			case 6:		temp = temp & 0xC0; temp1 = temp1 & 0x3F; break;
			case 7:		temp = temp & 0x80; temp1 = temp1 & 0x7F; break;
			default:	        break;
		}
		FM1702_Buf[0] = temp;
		UID[row - 1] = temp1 | temp;
		for(i = 1; i < length; i++)
		{
			UID[row - 1 + i] = FM1702_Buf[i];
		}
	}
}

void Set_BitFraming(uchar row, uchar col)
{
	switch(row)
	{
		case 0:		FM1702_Buf[1] = 0x20; break;
		case 1:		FM1702_Buf[1] = 0x30; break;
		case 2:		FM1702_Buf[1] = 0x40; break;
		case 3:		FM1702_Buf[1] = 0x50; break;
		case 4:		FM1702_Buf[1] = 0x60; break;
		default:	        break;
	}

	switch(col)
	{
		case 0:		Write_REG(0x0F,0x00);  break;
		case 1:		Write_REG(0x0F,0x11); FM1702_Buf[1] = (FM1702_Buf[1] | 0x01); break;
		case 2:		Write_REG(0x0F,0x22); FM1702_Buf[1] = (FM1702_Buf[1] | 0x02); break;
		case 3:		Write_REG(0x0F,0x33); FM1702_Buf[1] = (FM1702_Buf[1] | 0x03); break;
		case 4:		Write_REG(0x0F,0x44); FM1702_Buf[1] = (FM1702_Buf[1] | 0x04); break;
		case 5:		Write_REG(0x0F,0x55); FM1702_Buf[1] = (FM1702_Buf[1] | 0x05); break;
		case 6:		Write_REG(0x0F,0x66); FM1702_Buf[1] = (FM1702_Buf[1] | 0x06); break;
		case 7:		Write_REG(0x0F,0x77); FM1702_Buf[1] = (FM1702_Buf[1] | 0x07); break;
		default:	        break;
	}
}

uchar AntiColl(void)
{
	uchar	temp;
	uchar	i;
	uchar	row, col;
	uchar	pre_row;

	row = 0;
	col = 0;
	pre_row = 0;
	Write_REG(0x23,0x63);
	Write_REG(0x12,0x3f);
	Write_REG(0x13,0x3f);
	FM1702_Buf[0] = RF_CMD_ANTICOL;
	FM1702_Buf[1] = 0x20;
	Write_REG(0x22,0x03);	
	temp = Command_Send(2, FM1702_Buf, Transceive);
	while(1)
	{
		if(temp == FALSE)
		{
			return(FM1702_NOTAGERR);
		}

		temp = Read_REG(0x04);
		if(temp == 0)
		{
			return FM1702_BYTECOUNTERR;
		}

		Read_FIFO(FM1702_Buf);
		Save_UID(row, col, temp);	

		temp = Read_REG(0x0A);		
		temp = temp & 0x01;
		if(temp == 0x00)
		{
			temp = Check_UID();			
			if(temp == FALSE)
			{
				return(FM1702_SERNRERR);
			}
			return(FM1702_OK);
		}
		else
		{
			temp = Read_REG(0x0B);           
			row = temp / 8;
			col = temp % 8;
			FM1702_Buf[0] = RF_CMD_ANTICOL;
			Set_BitFraming(row + pre_row, col);	
			pre_row = pre_row + row;
			for(i = 0; i < pre_row + 1; i++)
			{
				FM1702_Buf[i + 2] = UID[i];
			}
			if(col != 0x00)
			{
				row = pre_row + 1;
			}
			else
			{
				row = pre_row;
			}
			temp = Command_Send(row + 2, FM1702_Buf, Transceive);
		}
	}
}

uchar Select_Card(void)
{
	uchar	temp, i;

	Write_REG(0x23,0x63);
	Write_REG(0x12,0x3f);
	FM1702_Buf[0] = RF_CMD_SELECT;
	FM1702_Buf[1] = 0x70;
	for(i = 0; i < 5; i++)
	{
		FM1702_Buf[i + 2] = UID[i];
	}

	Write_REG(0x22,0x0f);	
	temp = Command_Send(7, FM1702_Buf, Transceive);
	if(temp == FALSE)
	{
		return(FM1702_NOTAGERR);
	}
	else
	{
		temp = Read_REG(0x0A);
		if((temp & 0x02) == 0x02) return(FM1702_PARITYERR);
		if((temp & 0x04) == 0x04) return(FM1702_FRAMINGERR);
		if((temp & 0x08) == 0x08) return(FM1702_CRCERR);
		temp = Read_REG(0x04);
		if(temp != 1) 
			return(FM1702_BYTECOUNTERR);
		Read_FIFO(FM1702_Buf);
		temp = *FM1702_Buf;

		if((temp == 0x08) || (temp == 0x88) || (temp == 0x53) ||(temp == 0x18)) 
			return(FM1702_OK);
		else
			return(FM1702_SELERR);
	}
}

/****************************************************************/
/*名称: Authentication                                          */
/*功能: 该函数实现密码认证的过程                                */
/*输入: UID: 卡片序列号地址                                     */
/* SecNR: 扇区号                                                */
/* mode: 模式                                                   */
/*输出: FM1702_NOTAGERR: 无卡                                   */
/* FM1702_PARITYERR: 奇偶校验错                                 */
/* FM1702_CRCERR: CRC校验错                                     */
/* FM1702_OK: 应答正确                                          */
/* FM1702_AUTHERR: 权威认证有错                                 */
/****************************************************************/
uchar Authentication(uchar *UID, uchar SecNR, uchar mode)
{
	uchar i;
	uchar temp, temp1;

	Write_REG(0x23,0x63);
	Write_REG(0x12,0x3f);
	Write_REG(0x13,0x3f);
	temp1 = Read_REG(0x09);
	temp1 = temp1 & 0xf7;
	Write_REG(0x09,temp1);
	if(mode == RF_CMD_AUTH_LB)	FM1702_Buf[0] = RF_CMD_AUTH_LB;
	else						FM1702_Buf[0] = RF_CMD_AUTH_LA;
	FM1702_Buf[1] = SecNR * 4 + 3;
	for(i = 0; i < 4; i++)
	{
		FM1702_Buf[2 + i] = UID[i];
	}

	Write_REG(0x22,0x0f);	
	temp = Command_Send(6, FM1702_Buf, Authent1);
	if(temp == FALSE)			return FM1702_NOTAGERR;/* 无卡 */

	temp = Read_REG(0x0A);
	if((temp & 0x02) == 0x02) 	return FM1702_PARITYERR;/* 卡片奇偶校验错误 */
	if((temp & 0x04) == 0x04) 	return FM1702_FRAMINGERR;/* FM1702帧错误 */
	if((temp & 0x08) == 0x08) 	return FM1702_CRCERR;/* 卡片CRC校验错误 */
	temp = Command_Send(0, FM1702_Buf, Authent2);
	if(temp == FALSE)	return FM1702_NOTAGERR;/* 无卡 */

	temp = Read_REG(0x0A);
	if((temp & 0x02) == 0x02) return FM1702_PARITYERR;
	if((temp & 0x04) == 0x04) return FM1702_FRAMINGERR;
	if((temp & 0x08) == 0x08) return FM1702_CRCERR;
	temp1 = Read_REG(0x09);
	temp1 = temp1 & 0x08;	
	if(temp1 == 0x08)	return FM1702_OK;

	return FM1702_AUTHERR;
}

uchar MIF_READ(uchar *buff, uchar Block_Adr)
{
	uchar temp;

	Write_REG(0x23,0x63);
	Write_REG(0x12,0x3f);
	Write_REG(0x13,0x3f);
	Write_REG(0x22,0x0f);

	buff[0] = RF_CMD_READ;
	buff[1] = Block_Adr;
	temp = Command_Send(2, buff, Transceive);
	if(temp == 0)
	{
		return FM1702_NOTAGERR;
	}

	temp = Read_REG(0x0A);
	if((temp & 0x02) == 0x02) return FM1702_PARITYERR;
	if((temp & 0x04) == 0x04) return FM1702_FRAMINGERR;
	if((temp & 0x08) == 0x08) return FM1702_CRCERR;
	temp = Read_REG(0x04);
	if(temp == 0x10)
	{
		Read_FIFO(buff);
		return FM1702_OK;
	}
	else if(temp == 0x04)
	{
		Read_FIFO(buff);
		return FM1702_OK;
	}
	else
	{
		return FM1702_BYTECOUNTERR;
	}
}

uchar MIF_Write(uchar *buff, uchar Block_Adr)
{
	uchar temp;
	uchar *F_buff;

	Write_REG(0x23,0x63);
	Write_REG(0x12,0x3f);
	F_buff = buff + 0x10;
	Write_REG(0x22,0x07);
	*F_buff = RF_CMD_WRITE;
	*(F_buff + 1) = Block_Adr;
	temp = Command_Send(2, F_buff, Transceive);
	if(temp == FALSE)
	{
		return(FM1702_NOTAGERR);
	}

	temp = Read_REG(0x04);
	if(temp == 0)
	{
		return(FM1702_BYTECOUNTERR);
	}

	Read_FIFO(F_buff);
	temp = *F_buff;
	switch(temp)
	{
		case 0x00:	return(FM1702_NOTAUTHERR);
		case 0x04:	return(FM1702_EMPTY);
		case 0x0a:	break;
		case 0x01:	return(FM1702_CRCERR);
		case 0x05:	return(FM1702_PARITYERR);
		default:	return(FM1702_WRITEERR);
	}

	temp = Command_Send(16, buff, Transceive);
	if(temp == TRUE)	{	return(FM1702_OK);	}
	else
	{
		temp = Read_REG(0x0A);
		if((temp & 0x02) == 0x02)		return(FM1702_PARITYERR);
		else if((temp & 0x04) == 0x04)	return(FM1702_FRAMINGERR);
		else if((temp & 0x08) == 0x08)	return(FM1702_CRCERR);
		else							return(FM1702_WRITEERR);
	}
}

/*********************************************************************
各称：初始化FM1702SL
功能：实现初始货操作

*********************************************************************/
void Init_FM1702(void)
{
    uchar i;
    Reset_On;
    for(i=0;i<0xff;i++)    {      ;    }
    Reset_Off; 
    for(i=0;i<0xff;i++)    {      ;    }
    
    SCK_CLR;
    NSS_SET;
    {   
		while(Read_REG(Command));
		Write_REG(Page_Sel,0x80);
		while(Read_REG(Command));
		Write_REG(Page_Sel,0);
    }
        
    for(i=0;i<6;i++)
    {
      FM1702_Buf[i]=0xff;
    }
  
}

void delay_10ms(unsigned int _10ms)
{
	unsigned int i;

	while (_10ms--)
	{
		for(i=0;i<1000;i++);
	}
}

//读取块内金额
u32 ReadOneBlockMoney(uchar Block_Adr)
{
    u32 u32Dat=0x0;
	u8 ReadMoney_BufBack[16]={0};
	u8 statusBack=0;
	statusBack = MIF_READ(ReadMoney_BufBack,Block_Adr+1);	//读取金额
//	printf(">>读取%2d块内数据：%02d, %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X , ",Block_Adr+1,statusBack\
	,ReadMoney_BufBack[ 0],ReadMoney_BufBack[ 1],ReadMoney_BufBack[ 2],ReadMoney_BufBack[ 3],ReadMoney_BufBack[ 4],ReadMoney_BufBack[ 5],ReadMoney_BufBack[ 6],ReadMoney_BufBack[ 7]\
	,ReadMoney_BufBack[ 8],ReadMoney_BufBack[ 9],ReadMoney_BufBack[10],ReadMoney_BufBack[11],ReadMoney_BufBack[12],ReadMoney_BufBack[13],ReadMoney_BufBack[14],ReadMoney_BufBack[15]);
	if(statusBack == FM1702_OK)
	{
		if((CRC8_Table(ReadMoney_BufBack,15)==ReadMoney_BufBack[15]))
		{
			if(ReadMoney_BufBack[5]==0x00)	//正常卡
			{
				if(ReadMoney_BufBack[7]==0x01)	//反码标志
				{
					if((ReadMoney_BufBack[0]==GetFanma(ReadMoney_BufBack[ 8]))&&(ReadMoney_BufBack[1]==GetFanma(ReadMoney_BufBack[ 9]))\
					 &&(ReadMoney_BufBack[2]==GetFanma(ReadMoney_BufBack[10]))&&(ReadMoney_BufBack[3]==GetFanma(ReadMoney_BufBack[11])) )
					{
						u32Dat = ((u32)ReadMoney_BufBack[0]<<24);			//服务器通信使用
						u32Dat = u32Dat|((u32)ReadMoney_BufBack[1]<<16);	//金额1 高位
						u32Dat = u32Dat|((u32)ReadMoney_BufBack[2]<<8);		//金额2 
						u32Dat = u32Dat|((u32)ReadMoney_BufBack[3]);		//金额3							
					}
					else	u32Dat = ErrorMoney;	//错误
				}
				else
				{
					u32Dat = ((u32)ReadMoney_BufBack[0]<<24);			//服务器通信使用
					u32Dat = u32Dat|((u32)ReadMoney_BufBack[1]<<16);	//金额1 高位
					u32Dat = u32Dat|((u32)ReadMoney_BufBack[2]<<8);		//金额2 
					u32Dat = u32Dat|((u32)ReadMoney_BufBack[3]);		//金额3							
				}
			}
			else if(ReadMoney_BufBack[5]==0x02)	{ u32Dat = ErrorMoney+7; gErrorShow = ReadMoney_BufBack[12]; }	//特殊异常 E700//复制卡
			else 								{ u32Dat = ErrorMoney+5; gErrorShow = ReadMoney_BufBack[12]; }	//用户异常 E000		
		}			
		else
		{
			u32Dat = ErrorMoney;
		}
	}
	else
	{
		u32Dat = ErrorMoney;
	}
//	printf("金额:%08X;\r\n",u32Dat);//
//	printf("\r\n");
	return 	u32Dat;
}
/*********************************************************************
名称: ReadMoney 
功能: 读块内取卡内金额
输入：Block_Adr:块地址，块编号 
输出：卡内金额，最大为FFFFFF=16777215；实际为16777.215                                      
*********************************************************************/
u8 ReadMoney(u8 Block_Adr,u8 *_Money,u8 *_err)
{
    u32 u32Dat=0x0;
	int CMPDat=0;
	u32 u32DatBack=0x0,u32DatTemp=0x0;
	u8 ReadMoney_Buf[16]={0},ReadMoney_Buf1[16]={0};
	u8 status=0;
	u8 reDat;
	status = MIF_READ(ReadMoney_Buf,Block_Adr);	//读取金额
//	printf(">>读取%2d块内数据：%02d, %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X .\r\n",Block_Adr,status\
	,ReadMoney_Buf[ 0],ReadMoney_Buf[ 1],ReadMoney_Buf[ 2],ReadMoney_Buf[ 3],ReadMoney_Buf[ 4],ReadMoney_Buf[ 5],ReadMoney_Buf[ 6],ReadMoney_Buf[ 7]\
	,ReadMoney_Buf[ 8],ReadMoney_Buf[ 9],ReadMoney_Buf[10],ReadMoney_Buf[11],ReadMoney_Buf[12],ReadMoney_Buf[13],ReadMoney_Buf[14],ReadMoney_Buf[15]);
	if(status== FM1702_OK)
    {
		if((CRC8_Table(ReadMoney_Buf,6)==ReadMoney_Buf[6])&&(ReadMoney_Buf[4]==RFID_OPENFLAG))	//CRC  、 开卡标志
		{
			if(ReadMoney_Buf[7]==0x01)	//反码标志
			{
				if((ReadMoney_Buf[0]==GetFanma(ReadMoney_Buf[ 8]))&&(ReadMoney_Buf[1]==GetFanma(ReadMoney_Buf[ 9]))\
				 &&(ReadMoney_Buf[2]==GetFanma(ReadMoney_Buf[10]))&&(ReadMoney_Buf[3]==GetFanma(ReadMoney_Buf[11])) )
				{
					u32Dat = ((u32)ReadMoney_Buf[0]<<24);	        //服务器通信使用
					u32Dat = u32Dat|((u32)ReadMoney_Buf[1]<<16);	//金额1 高位
					u32Dat = u32Dat|((u32)ReadMoney_Buf[2]<<8);		//金额2 
					u32Dat = u32Dat|((u32)ReadMoney_Buf[3]);		//金额3
					memcpy(_Money,ReadMoney_Buf,0x04);				
					reDat = Money_OK;	
				}
				else	reDat = Money_Error_Check;	//反码错误 \数据校验失败
			}
			else
			{
				u32Dat = ((u32)ReadMoney_Buf[0]<<24);			//服务器通信使用
				u32Dat = u32Dat|((u32)ReadMoney_Buf[1]<<16);	//金额1 高位
				u32Dat = u32Dat|((u32)ReadMoney_Buf[2]<<8);		//金额2 
				u32Dat = u32Dat|((u32)ReadMoney_Buf[3]);		//金额3
				memcpy(_Money,ReadMoney_Buf,0x04);					
				reDat = Money_OK;	
			}
			if(ReadMoney_Buf[5]==0x00);		//正常卡
			else if(ReadMoney_Buf[5]==0x02)	//{u32Dat = ErrorMoney+7;gErrorShow = ReadMoney_Buf[12];}	//特殊异常 E700//复制卡
			{
				reDat = Money_Error_E700;
				_err[0] = ReadMoney_Buf[12];
			}
			else 							//{u32Dat = ErrorMoney+5;gErrorShow = ReadMoney_Buf[12];}	//用户异常 E000						
			{
				reDat = Money_Error_E000;
				_err[0] = ReadMoney_Buf[12];
			}
			
			if(reDat == Money_OK)
			{
				if((ReadMoney_Buf[14]==0xAA)&&(CRC8_Table(ReadMoney_Buf,15)==ReadMoney_Buf[15]))	//备份钱包标志
				{	
					status = MIF_READ(ReadMoney_Buf1,Block_Adr+1);	//读取金额
					if(status== FM1702_OK)
					{
						CMPDat = memcmp(ReadMoney_Buf,ReadMoney_Buf1,4);	//金额比较
						if(CMPDat>0)	
						{
							printf("金额与备份金额不相等");
							u32Dat = ((u32)ReadMoney_Buf[0]<<24);			//服务器通信使用
							u32Dat = u32Dat|((u32)ReadMoney_Buf[1]<<16);	//金额1 高位
							u32Dat = u32Dat|((u32)ReadMoney_Buf[2]<<8);		//金额2 
							u32Dat = u32Dat|((u32)ReadMoney_Buf[3]);		//金额3
							WriteMoney(Block_Adr,u32DatBack,0);			//重新写入金额，并加入备份标志
						}
						else if(CMPDat<0)
						{
							printf("金额与备份金额不相等");
							u32Dat = ((u32)ReadMoney_Buf1[0]<<24);			//服务器通信使用
							u32Dat = u32Dat|((u32)ReadMoney_Buf1[1]<<16);	//金额1 高位
							u32Dat = u32Dat|((u32)ReadMoney_Buf1[2]<<8);	//金额2 
							u32Dat = u32Dat|((u32)ReadMoney_Buf1[3]);		//金额3
							WriteMoney(Block_Adr,u32DatBack,0);			//重新写入金额，并加入备份标志
						}
					}
				}
				else
				{	
					u32DatTemp = u32Dat;
					WriteMoney(Block_Adr,u32DatTemp,0);	//没有开启备份标志，加入备份标志
				}
			}
		}
		else 	reDat = Money_Error_NULL;	//不是本系统卡
    }
	else    reDat = Money_Error_Read;	//读卡失败
//	printf("金额:%08X; 标志量:%d.\r\n",u32Dat,reDat);
	return 	reDat;
}


/*********************************************************************
名称: DecMoney 
功能: 递减卡内金额 
输入：Block_Adr:块地址，块编号 
	  Dec_dat:递减金额
输出：卡内最后金额，最大为50000；实际为500.00                                      
*********************************************************************/
u8 DecMoney(u8 Block_Adr,u8 Dec_dat,u8 *_Money)
{
	static u32 u32Dat=0x0;
	u8 ReadMoney_Buf[16]={0};
	u8 status=0;
	u8 u8Dat[4]={0},uErr[2]={0};
	u8 _ReadStatus;
	_ReadStatus = ReadMoney(FM1702_Key[6],u8Dat,uErr);	//读取卡内金额 使用地址块FM1702_Key[6]
	if(_ReadStatus!=Money_OK)	{	return Money_Error_Read;	}	//读卡失败
	else
	{
		u32Dat = ((u32)u8Dat[0]<<24);			//服务器通信使用
		u32Dat = u32Dat|((u32)u8Dat[1]<<16);	//金额1 高位
		u32Dat = u32Dat|((u32)u8Dat[2]<<8);		//金额2 
		u32Dat = u32Dat|((u32)u8Dat[3]);		//金额3
		printf(">>DecMoney,%d.%03d.->",(u32Dat&0x00FFFFFF)/1000,(u32Dat&0x00FFFFFF)%1000);
		u32Dat = u32Dat - Dec_dat;		//减值;
		printf(" %d.%03d.",(u32Dat&0x00FFFFFF)/1000,(u32Dat&0x00FFFFFF)%1000);
		
		ReadMoney_Buf[ 0] = u32Dat>>24;	//通信码
		ReadMoney_Buf[ 1] = u32Dat>>16;	//金额1 高位
		ReadMoney_Buf[ 2] = u32Dat>>8;	//金额2 
		ReadMoney_Buf[ 3] = u32Dat;		//金额3	
		ReadMoney_Buf[ 4] = RFID_OPENFLAG;	//开卡标志
		ReadMoney_Buf[ 5] = 0x00;			//卡类别 0x00=正常卡
		ReadMoney_Buf[ 6] = CRC8_Table(ReadMoney_Buf,6);
		ReadMoney_Buf[ 7] = 0x01;	//反码标志
		ReadMoney_Buf[ 8] = GetFanma(ReadMoney_Buf[0]);	//反码 通信码
		ReadMoney_Buf[ 9] = GetFanma(ReadMoney_Buf[1]);	//反码 金额1 高位
		ReadMoney_Buf[10] = GetFanma(ReadMoney_Buf[2]);	//反码 金额2
		ReadMoney_Buf[11] = GetFanma(ReadMoney_Buf[3]);	//反码 金额3
		ReadMoney_Buf[12] = 0x00;	//异常显示代码
		ReadMoney_Buf[13] = 0x00; 	//预留 
		ReadMoney_Buf[14] = 0xAA;	//备份钱包标志
		ReadMoney_Buf[15] = CRC8_Table(ReadMoney_Buf,15);	//CRC8
		status = MIF_Write(ReadMoney_Buf,Block_Adr);			//写入金额
		if(status != FM1702_OK)		return 	Money_Error_Write;	//写卡失败
		status = MIF_Write(ReadMoney_Buf,Block_Adr+1);			//写入金额--备份钱包
		if(status != FM1702_OK)		return 	Money_Error_Write;	//写卡失败

		//减完金额后 再读出并对比	
		_ReadStatus = ReadMoney(FM1702_Key[6],u8Dat,uErr);	//读取卡内金额 使用地址块FM1702_Key[6]	
		if(_ReadStatus!=Money_OK)	{	return Money_Error_Read;	}	//读卡失败
		else
		{
			if(memcmp(ReadMoney_Buf,u8Dat,4)==0)	//数据比较正确	
			{	
				memcpy(_Money,ReadMoney_Buf,0x04);	
				return Money_OK;			
			}
			else	//数据比较错误								
			{
				return Money_Error_Check;	
			}	
		}
	}
}

/*********************************************************************
名称: WriteMoney 
功能: 写入卡内金额 
输入：Block_Adr:块地址，块编号 
	  Write_dat:写入金额
输出：                                     
*********************************************************************/
u8 WriteMoney(u8 Block_Adr,u32 Write_dat,u8 _ErrorFlag)
{
	u8 status,_UseFlag;
	u8 ReadMoney_Buf[16]={0};
	if((_ErrorFlag/10)==7)			{_UseFlag =0x02;}	//复制卡
	else if ((_ErrorFlag/10)!=0)	{_UseFlag =0x01;}	//用户异常
	else							{_UseFlag =0x00;}	//正常卡
	ReadMoney_Buf[ 0] = (u8)(Write_dat>>24); 			//校验
	ReadMoney_Buf[ 1] = (u8)(Write_dat>>16); 			//数据高位
	ReadMoney_Buf[ 2] = (u8)(Write_dat>>8);  			//数据
	ReadMoney_Buf[ 3] = (u8)(Write_dat);   				//数据低位
	ReadMoney_Buf[ 4] = 0x01;    						//开卡标记
	ReadMoney_Buf[ 5] = _UseFlag;						//用户异常
	ReadMoney_Buf[ 6] = CRC8_Table(ReadMoney_Buf,6);	//CRC8
	ReadMoney_Buf[ 7] = 0x01;							//反码标志
	ReadMoney_Buf[ 8] = GetFanma(ReadMoney_Buf[0]);		//反码 通信码
	ReadMoney_Buf[ 9] = GetFanma(ReadMoney_Buf[1]);		//反码 金额1 高位
	ReadMoney_Buf[10] = GetFanma(ReadMoney_Buf[2]);		//反码 金额2
	ReadMoney_Buf[11] = GetFanma(ReadMoney_Buf[3]);		//反码 金额3
	ReadMoney_Buf[12] = _ErrorFlag;  					//异常显示代码
	ReadMoney_Buf[13] = 0x00; 							//预留 
	ReadMoney_Buf[14] = 0xAA;							//备份钱包标志
	ReadMoney_Buf[15] = CRC8_Table(ReadMoney_Buf,15);	//CRC8
	status = MIF_Write(ReadMoney_Buf,Block_Adr);		//写入金额
	if(status != FM1702_OK)		return status;
	status = MIF_Write(ReadMoney_Buf,Block_Adr+1);		//写入备份钱包	
	return status;
}

u8 ReadRFIDCard(void)	//读取CARD状态 ==FM1702_OK表示只用于检测是否有卡，不校验密码
{
    u8 status;
	status = Request(RF_CMD_REQUEST_ALL);		// 寻卡
	if(status != FM1702_OK)	
	{	/*delay_10ms(2);*/ 		return status;	}

	status = AntiColl();						//冲突检测
	if(status != FM1702_OK)    	return status;	
	else 						return status;

}

u8 ReadRFID(void)	//读取RFID状态 ==FM1702_OK表示读取数据成功
{
	u8 status,password[6] = {0x0};
	u8 SectionNum = 0;
	password[0] = FM1702_Key[0];	password[1] = FM1702_Key[1];	password[2] = FM1702_Key[2];
	password[3] = FM1702_Key[3];	password[4] = FM1702_Key[4];	password[5] = FM1702_Key[5];
	SectionNum = FM1702_Key[6]/4;	//取出扇区号
	status = Request(RF_CMD_REQUEST_ALL);		// 寻卡
	if(status != FM1702_OK) {	/*delay_10ms(2);*/	return status;	}

	status = AntiColl();						//冲突检测
	if(status != FM1702_OK)    	return status;	
	
	status=Select_Card();                       //选卡
	if(status != FM1702_OK)    	return status;

	status = Load_Key(password);                //加载密码
	if(status != TRUE)         	return status;

	status = Authentication(UID, SectionNum, RF_CMD_AUTH_LA);//校验密码
	if(status != FM1702_OK)    	return status;
	else 						return status;
}


u8 WriteRFID_Key_test(void)	//
{
	u8 status,password[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xF1};
	u8 SectionNum = 8;
	u8 RFID_Dat[16] = {0};
	status = Request(RF_CMD_REQUEST_ALL);		// 寻卡
	if(status != FM1702_OK) 
	{		/*delay_10ms(2);*/		return status;	}

	status = AntiColl();						//冲突检测
	if(status != FM1702_OK)    	return status;	

	status=Select_Card();                       //选卡
	if(status != FM1702_OK)    	return status;

	status = Load_Key(password);                //加载密码
	if(status != TRUE)         	return status;

	status = Authentication(UID, 2, RF_CMD_AUTH_LA);//校验密码
	if(status != FM1702_OK)    	return status;
	
    status=MIF_READ(RFID_Dat,SectionNum);	             //读卡
	if(status != FM1702_OK)    	return status;
	else 						return status;
	
}

void SoftReset1702(void)
{
    if(Read_REG(CRCPresetLSB)!=0x63)
    {
        Init_FM1702();  //FM1701初始化
    }
}


