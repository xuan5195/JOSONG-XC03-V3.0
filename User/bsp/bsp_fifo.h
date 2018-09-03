#ifndef __BSP_FIFO_H
#define __BSP_FIFO_H	 

#include "stm32f10x.h"

typedef enum
{
	CQ_NONE = 0,	//0 表示FIFO无数据
	CQ_OK,			//数据正常
}CQ_ENUM;

#define CQ_FIFO_SIZE	10	//FIFO空间
typedef struct
{
	uint8_t Buf[CQ_FIFO_SIZE][16];	//键值缓冲区 
	uint8_t Read;					//缓冲区读指针
	uint8_t Write;					//缓冲区写指针 
	uint8_t Read2;					//缓冲区读指针2
	//uint8_t ** entries;				//数据缓冲区 
}CQ_FIFO_T;

void bsp_InitCQVar(void);
void bsp_PutCQ(uint8_t *_Date);
uint8_t bsp_GetCQ(uint8_t *_Date);
uint8_t bsp_GetCQ2(uint8_t *_Date);

#endif

