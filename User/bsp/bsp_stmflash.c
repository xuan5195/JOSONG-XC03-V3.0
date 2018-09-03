#include "bsp_stmflash.h"
#include "bsp_crc8.h"
#include "bsp.h"

#define FLASH_SAVE_ADD		0x0800FC00		//Flash存储起始地址 为Flash最后一页，大小为1K

//uint8_t Physical_ADD_Temp[4]={0x20,0x18,0x10,0x03};//物理地址 0x03
uint8_t Physical_ADD[4]={0x00,0x00,0x00,0x00};//物理地址 0x00
uint8_t FM1702_Key[7]={0xFF,0xFF,0xFF,0xFF,0xFF,0xF1,0x29};
uint8_t WaterCost=50,CostNum=29;	//WaterCost=水费 最小扣款金额  //脉冲数
uint8_t g_IAP_Flag=0x00;	//在线升级标志
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}


static void STMFLASH_Write(u32 WriteAddr,u8 *pBuffer,u16 NumToWrite)	
{
 	u16 i,DateTemp=0;    
	FLASH_Unlock();		//解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(FLASH_SAVE_ADD);	
	for(i=0;i<NumToWrite;(i=i+2))
	{
		DateTemp = ((u16)pBuffer[i+1])<<8 ;
		DateTemp = DateTemp|pBuffer[i];
		FLASH_ProgramHalfWord(WriteAddr,DateTemp);
	    WriteAddr+=2;//地址增加2.
	}  
	FLASH_Lock();//上锁
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u8 *pBuffer,u16 NumToRead)   	
{
	u16 i,DateTemp=0;
	for(i=0;i<NumToRead;(i=i+2))
	{
		DateTemp = STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		pBuffer[i]	=(u8)(DateTemp);
		pBuffer[i+1]=(u8)(DateTemp>>8);
		ReadAddr+=2;//偏移2个字节.	
	}
}



void Read_Flash_Dat(void)
{
	u8 datatemp[16]={0};
	STMFLASH_Read(FLASH_SAVE_ADD,datatemp,16);
	printf("STMFLASH_Read: %02X%02X%02X%02X; %02X%02X%02X%02X; %02X%02X%02X%02X; %02X%02X%02X%02X;\r\n",\
	datatemp[ 0],datatemp[ 1],datatemp[ 2],datatemp[ 3],datatemp[ 4],datatemp[ 5],datatemp[ 6],datatemp[ 7],\
	datatemp[ 8],datatemp[ 9],datatemp[10],datatemp[11],datatemp[12],datatemp[12],datatemp[14],datatemp[15]	);
	if(datatemp[4] != 0xAA)	//新板子未能电过。
	{
		printf("New Board...\r\n");
		if((datatemp[0]!=0x00)&&(datatemp[0]!=0xFF)&&(datatemp[1]!=0x00)&&(datatemp[1]!=0xFF))	//检测SN是否写入
		{	//SN存在-->写入RFID信息及核验数据
			printf("SN check OK,Write RFID_Dat,CRC_Dat...\r\n");
			datatemp[ 4] = 0xAA;	//UseFlag
			datatemp[ 5] = FM1702_Key[0];	//RFID_key0
			datatemp[ 6] = FM1702_Key[1];	//RFID_key1
			datatemp[ 7] = FM1702_Key[2];	//RFID_key2
			datatemp[ 8] = FM1702_Key[3];	//RFID_key3
			datatemp[ 9] = FM1702_Key[4];	//RFID_key4
			datatemp[10] = FM1702_Key[5];	//RFID_key5
			datatemp[11] = FM1702_Key[6];	//块地址
			datatemp[12] = WaterCost;		//水费 最小扣款金额
			datatemp[13] = CostNum;			//脉冲数
			datatemp[14] = g_IAP_Flag;		//程序升级标志
			datatemp[15] = CRC8_Table(datatemp+4,11);
			STMFLASH_Write(FLASH_SAVE_ADD,datatemp,16);			
		}
	}
	else	//使用过的，直接读取SN数据，及校验数据
	{
		printf("Read RFID_Dat,CRC_Dat...\r\n");
		if( CRC8_Table(datatemp+4,11) == datatemp[15] )
		{
			Physical_ADD[0] = datatemp[0];	//物理地址0
			Physical_ADD[1] = datatemp[1];	//物理地址1
			Physical_ADD[2] = datatemp[2];	//物理地址2
			Physical_ADD[3] = datatemp[3];	//物理地址3
			
			FM1702_Key[0] 	= datatemp[5];	//RFID_key0
			FM1702_Key[1] 	= datatemp[6];	//RFID_key1
			FM1702_Key[2] 	= datatemp[7];	//RFID_key2
			FM1702_Key[3] 	= datatemp[8];	//RFID_key3
			FM1702_Key[4] 	= datatemp[9];	//RFID_key4
			FM1702_Key[5] 	= datatemp[10];	//RFID_key5
			FM1702_Key[6] 	= datatemp[11];	//块地址
			WaterCost 		= datatemp[12];	//水费 最小扣款金额
			CostNum 		= datatemp[13];	//脉冲数
			g_IAP_Flag 		= datatemp[14];	//程序升级标志
		}
	}
}

void Write_Flash_Dat(void)
{
	u8 datatemp1[16]={0},datatemp2[16]={0};
	STMFLASH_Read(FLASH_SAVE_ADD,datatemp1,16);	//读出的值 对比 以下要写入的值
	datatemp2[ 0] = datatemp1[0];//物理地址0
	datatemp2[ 1] = datatemp1[1];//物理地址1
	datatemp2[ 2] = datatemp1[2];//物理地址2
	datatemp2[ 3] = datatemp1[3];//物理地址3
	datatemp2[ 4] = 0xAA;			//标志位
	datatemp2[ 5] = FM1702_Key[0];	//RFID_key0
	datatemp2[ 6] = FM1702_Key[1];	//RFID_key1
	datatemp2[ 7] = FM1702_Key[2];	//RFID_key2
	datatemp2[ 8] = FM1702_Key[3];	//RFID_key3
	datatemp2[ 9] = FM1702_Key[4];	//RFID_key4
	datatemp2[10] = FM1702_Key[5];	//RFID_key5
	datatemp2[11] = FM1702_Key[6];	//块地址
	datatemp2[12] = WaterCost;		//水费 最小扣款金额
	datatemp2[13] = CostNum;		//脉冲数
	datatemp2[14] = g_IAP_Flag;		//程度升级标志
	datatemp2[15] = CRC8_Table(datatemp2+4,11);
	if((datatemp2[15]!=datatemp1[15])||(datatemp2[13]!=datatemp1[13])||(datatemp2[12]!=datatemp1[12])||\
	   (datatemp2[11]!=datatemp1[11])||(datatemp2[10]!=datatemp1[10])||(datatemp2[ 9]!=datatemp1[ 9])||\
	   (datatemp2[ 8]!=datatemp1[ 8])||(datatemp2[ 7]!=datatemp1[ 7])||(datatemp2[ 6]!=datatemp1[ 6])||\
	   (datatemp2[ 5]!=datatemp1[ 5]))	//数据不一样才写Flash
		{STMFLASH_Write(FLASH_SAVE_ADD,datatemp2,16);}
}
void Clear_Flash_Dat(void)
{
	u8 datatemp[16]={0};
	STMFLASH_Write(FLASH_SAVE_ADD,datatemp,16);
}


