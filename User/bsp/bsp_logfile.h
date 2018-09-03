#ifndef __BSP_LOGFILE_H__
#define __BSP_LOGFILE_H__
#include "stm32f10x.h"

#define LOGFILE_SAVE_ADD	0x0800D000			//日志文件Flash存储起始地址,大小为10K 0x0800D000-0x0800F800
#define STM_SECTOR_SAVE		1024				//页大小 1024=0x400; 
#define LOGFILE_PAGE_COUNT	64					//页存储数量
#define LOGFILE_BYTE_COUNT	16					//单条日志大小 12Byte-->16Byte
 
uint16_t Search_LogNum(void);
void Write_LogDat(uint16_t _no,uint8_t *pBuffer);
void Read_LogDat(uint16_t _no,uint8_t *pBuffer);
void Show_LogDat(uint16_t _no);

#endif

