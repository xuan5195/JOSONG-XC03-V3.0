#include "bsp_stmflash.h"
#include "bsp_crc8.h"
#include "bsp.h"

#define FLASH_SAVE_ADD		0x0800FC00		//Flash存储起始地址 为Flash最后一页，大小为1K
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

/*
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
}*/

//半字写
static void STMFLASH_WriteHword(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
 	u16 i;    
	FLASH_Unlock();		//解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(FLASH_SAVE_ADD);	
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
	FLASH_Lock();//上锁
}

//半字读
static void STMFLASH_ReadHword(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]= STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.	
		ReadAddr+=2;//偏移2个字节.	
	}
}


void Read_Flash_Dat(void)
{
	u16 datatemp[20]={0};
ReReadFlash:
	STMFLASH_ReadHword(FLASH_SAVE_ADD,datatemp,20);
	printf("STMFLASH_Read: %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X;\r\n",\
	datatemp[ 0],datatemp[ 1],datatemp[ 2],datatemp[ 3],datatemp[ 4],datatemp[ 5],datatemp[ 6],datatemp[ 7],datatemp[ 8],datatemp[ 9],\
	datatemp[10],datatemp[11],datatemp[12],datatemp[12],datatemp[14],datatemp[15],datatemp[16],datatemp[17],datatemp[18],datatemp[19]);
	if(datatemp[18] != 0xAA)	//新板子未能电过。
	{
		printf("New Board...\r\n");
		Write_Flash_Dat();
		goto ReReadFlash;
	}
	else	//使用过的，直接读取SN数据，及校验数据
	{
		gParamDat[Menu_Ua] = datatemp[ 0];//电压上限
		gParamDat[Menu_Ub] = datatemp[ 1];//电压下限
		gParamDat[Menu_A ] = datatemp[ 2];//电流值
		gParamDat[Menu_Aa] = datatemp[ 3];//电流上限
		gParamDat[Menu_Ab] = datatemp[ 4];//电流下限
		gParamDat[Menu_Ac] = datatemp[ 5];//电流变比
		gParamDat[Menu_Ca] = datatemp[ 6];//温度上限
		gParamDat[Menu_Cb] = datatemp[ 7];//温度下限
		gParamDat[Menu_Ha] = datatemp[ 8];//湿度上限
		gParamDat[Menu_Hb] = datatemp[ 9];//湿度下限
		gParamDat[Menu_Pa] = datatemp[10];//压力上限
		gParamDat[Menu_Pb] = datatemp[11];//压力下限
		gParamDat[Menu_Fa] = datatemp[12];//流量上限
		gParamDat[Menu_Fb] = datatemp[13];//流量下限
        printf("电压上限:%2d.电压下限:%2d.\r\n",gParamDat[Menu_Ua],gParamDat[Menu_Ub]);
        printf("电流上限:%2d.电流下限:%2d.电流基数:%3d.电流变比:%2d.\r\n",gParamDat[Menu_Aa],gParamDat[Menu_Ab],gParamDat[Menu_A],gParamDat[Menu_Ac]);
        printf("温度上限:%2d.温度下限:%2d.湿度上限:%2d.湿度下限:%2d.\r\n",gParamDat[Menu_Ca],gParamDat[Menu_Cb],gParamDat[Menu_Ha],gParamDat[Menu_Hb]);
        printf("压力上限:%2d.压力下限:%2d.流量上限:%2d.流量下限:%2d.\r\n",gParamDat[Menu_Pa],gParamDat[Menu_Pb],gParamDat[Menu_Fa],gParamDat[Menu_Fb]);
	}
}

void Write_Flash_Dat(void)
{
	u16 datatemp[20]={0};
	datatemp[ 0] = gParamDat[Menu_Ua];//电压上限
	datatemp[ 1] = gParamDat[Menu_Ub];//电压下限
	datatemp[ 2] = gParamDat[Menu_A ];//电流值
	datatemp[ 3] = gParamDat[Menu_Aa];//电流上限
	datatemp[ 4] = gParamDat[Menu_Ab];//电流下限
	datatemp[ 5] = gParamDat[Menu_Ac];//电流变比
	datatemp[ 6] = gParamDat[Menu_Ca];//温度上限
	datatemp[ 7] = gParamDat[Menu_Cb];//温度下限
	datatemp[ 8] = gParamDat[Menu_Ha];//湿度上限
	datatemp[ 9] = gParamDat[Menu_Hb];//湿度下限
	datatemp[10] = gParamDat[Menu_Pa];//压力上限
	datatemp[11] = gParamDat[Menu_Pb];//压力下限
	datatemp[12] = gParamDat[Menu_Fa];//流量上限
	datatemp[13] = gParamDat[Menu_Fb];//流量下限
	datatemp[18] = 0xAA;	//使用标志
	STMFLASH_WriteHword(FLASH_SAVE_ADD,(u16 *)datatemp,20);
}



