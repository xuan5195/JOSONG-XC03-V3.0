#ifndef __BSP_FIFO_H
#define __BSP_FIFO_H	 

#include "stm32f10x.h"

typedef enum
{
	CQ_NONE = 0,	//0 ��ʾFIFO������
	CQ_OK,			//��������
}CQ_ENUM;

#define CQ_FIFO_SIZE	10	//FIFO�ռ�
typedef struct
{
	uint8_t Buf[CQ_FIFO_SIZE][16];	//��ֵ������ 
	uint8_t Read;					//��������ָ��
	uint8_t Write;					//������дָ�� 
	uint8_t Read2;					//��������ָ��2
	//uint8_t ** entries;				//���ݻ����� 
}CQ_FIFO_T;

void bsp_InitCQVar(void);
void bsp_PutCQ(uint8_t *_Date);
uint8_t bsp_GetCQ(uint8_t *_Date);
uint8_t bsp_GetCQ2(uint8_t *_Date);

#endif

