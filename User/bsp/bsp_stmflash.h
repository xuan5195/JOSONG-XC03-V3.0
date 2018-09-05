#ifndef __BSP_STMFLASH_H__
#define __BSP_STMFLASH_H__
#include "stm32f10x.h"
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
//void STMFLASH_Write(u32 WriteAddr,u8 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u8 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
void Read_Flash_Dat(void);
void Write_Flash_Dat(void);

#endif

