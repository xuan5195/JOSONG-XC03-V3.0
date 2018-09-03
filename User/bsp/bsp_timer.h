#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint32_t count;	/* 计数器 */
	volatile uint8_t flag;		/* 定时到达标志  */
}SOFT_TMR;

/* 供外部调用的函数声明 */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

#endif
