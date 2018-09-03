#include "bsp_logfile.h"
#include "bsp_stmflash.h"
#include "bsp_crc8.h"
#include "bsp.h"
extern uint32_t g_RunningTime;		//运行时间
//extern uint16_t gLogNum;		    //日志存储条数 0-640

static void STMFLASH_Write(u32 WriteAddr,u8 *pBuffer,u16 NumToWrite)	
{
 	u16 i,DateTemp=0;    
	FLASH_Unlock();		//解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	//FLASH_ErasePage(FLASH_SAVE_ADD);	
	for(i=0;i<NumToWrite;(i=i+2))
	{
		DateTemp = ((u16)pBuffer[i+1])<<8 ;
		DateTemp = DateTemp|pBuffer[i];
		FLASH_ProgramHalfWord(WriteAddr,DateTemp);
	    WriteAddr+=2;//地址增加2.
	}  
	FLASH_Lock();//上锁
}


//上电查找目前起始地址
//返回值：当面记录在条数 0-799；
uint16_t Search_LogNum(void)
{
	uint16_t _Num=0;
	uint8_t i,j;
	uint8_t _Buffer[16];
	for(i=0;i<10;i++)
	{
		for(j=0;j<LOGFILE_PAGE_COUNT;j++)
		{
			_Num = i*LOGFILE_PAGE_COUNT+j;
			Read_LogDat(_Num,_Buffer);
			if((_Buffer[0]==0xFF)&&(_Buffer[1]==0xFF)&&(_Buffer[2]==0xFF)&&(_Buffer[3]==0xFF))
			{
				return _Num;
			}
		}
	}
	return 0xEF00;	//错误值
}

//_no:写入条数；
//pBuffer：写入数据缓存
void Write_LogDat(uint16_t _no,uint8_t *pBuffer)
{
	u32 UseADD=0;
	if(_no!=0)
	{
		UseADD += (_no/LOGFILE_PAGE_COUNT)*STM_SECTOR_SAVE;		//加入页地址 1024
		UseADD += (_no%LOGFILE_PAGE_COUNT)*LOGFILE_BYTE_COUNT;	//加入页内偏移地址 12-->16Byte
	}
	UseADD += LOGFILE_SAVE_ADD;								//加入起始地址 0x0800D000
	printf(">>写日志,ADD:0x%08X | ",UseADD);
	STMFLASH_Write(UseADD,pBuffer,LOGFILE_BYTE_COUNT);		//数据写入
	printf("%02X%02X%02X%02X,%02X%02X%02X%02X,%02X%02X%02X%02X,%02X%02X%02X%02X."\
	,pBuffer[ 0],pBuffer[ 1],pBuffer[ 2],pBuffer[ 3],pBuffer[ 4],pBuffer[ 5],pBuffer[ 6],pBuffer[ 7]\
	,pBuffer[ 8],pBuffer[ 9],pBuffer[10],pBuffer[11],pBuffer[12],pBuffer[13],pBuffer[14],pBuffer[15]);
	if( (_no%LOGFILE_PAGE_COUNT) == (LOGFILE_PAGE_COUNT-1) )	//到最后一个，把下页擦除；
	{
		printf("\r\nThe last one,Next Page Erase. ");
		UseADD = (_no/LOGFILE_PAGE_COUNT)*STM_SECTOR_SAVE;		//加入页地址 1024
		UseADD = UseADD + LOGFILE_SAVE_ADD;								//加入起始地址 0x0800BC00
		FLASH_Unlock();		        //解锁Flash
        UseADD = UseADD + 0x400;	//加入偏移地址 0x400
		if(UseADD==0x0800F800)	    //当前为最后一页面，擦除第一页；
		{
			UseADD = LOGFILE_SAVE_ADD;	//第一页 起始地址
		}
		printf("StartADD=%08X,Flash_Page=%02d/64.",UseADD,(UseADD&0x0000FFFF)>>10);
		FLASH_ErasePage(UseADD);	//下页起始地址
		FLASH_Lock();	//上锁
	}
	printf("\r\n");
}

//_no:读取条数；
//pBuffer：读出数据缓存
void Read_LogDat(uint16_t _no,uint8_t *pBuffer)
{
	u32 UseADD=0;
	UseADD += (_no/LOGFILE_PAGE_COUNT)*STM_SECTOR_SAVE;		//当面页数
	UseADD += (_no%LOGFILE_PAGE_COUNT)*LOGFILE_BYTE_COUNT;	//当面条数
	UseADD += LOGFILE_SAVE_ADD;								//第一页 起始地址
//	printf("ADD=0x%08X, ",UseADD);
	STMFLASH_Read(UseADD,pBuffer,LOGFILE_BYTE_COUNT);
//    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X;\r\n",pBuffer[0],pBuffer[1],pBuffer[2],pBuffer[3],\
    pBuffer[4],pBuffer[5],pBuffer[6],pBuffer[7],pBuffer[8],pBuffer[9],pBuffer[10],pBuffer[11],pBuffer[12],pBuffer[13],pBuffer[14],pBuffer[15]);
}

//显示页存储数据
void Show_LogDat(uint16_t _no)
{
	uint16_t _Num=0,i=0;
	uint8_t _Buffer[16];	//数据缓存
	_Num = (_no/LOGFILE_PAGE_COUNT)*LOGFILE_PAGE_COUNT;	//取出最后页的起始条数;
	printf("Show_LogDat: %03d\r\n",_Num);
	for(i=_Num;i<_no;i++)
	{
		Read_LogDat(i,_Buffer);
		printf("_Num=%03d, ",i);
		printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X;\r\n",_Buffer[0],_Buffer[1],_Buffer[2],_Buffer[3],\
        _Buffer[4],_Buffer[5],_Buffer[6],_Buffer[7],_Buffer[8],_Buffer[9],_Buffer[10],_Buffer[11],_Buffer[12],_Buffer[13],_Buffer[14],_Buffer[15]);
	}
}

