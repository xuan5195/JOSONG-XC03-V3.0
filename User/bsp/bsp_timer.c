#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_timer.h"
//#include "bsp_button.h"
extern void CPU_IDLE(void);

/* 
	�ڴ˶������ɸ�������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	4		/* ������ʱ���ĸ�������1��������bsp_DelayMS()ʹ�� */

SOFT_TMR g_Tmr[TMR_COUNT];

/* ȫ������ʱ�䣬��λ10ms������uIP */
__IO int32_t g_iRunTime = 0;

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

void bsp_InitTimer(void)
{
	uint8_t i;
	
	/* �������е�������ʱ�� */
	for (i = 0; i < TMR_COUNT; i++)
	{
		g_Tmr[i].count = 0;
		g_Tmr[i].flag = 0;
	}
	
	/* 
		����systic�ж�����Ϊ1ms��������systick�жϡ�
    	��������� \Libraries\CMSIS\CM3\CoreSupport\core_cm3.h 
    	
    	Systick�ж�������(\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\startup\arm\
    		startup_stm32f10x_hd.s �ļ��ж���Ϊ SysTick_Handler��
    	SysTick_Handler������ʵ����stm32f10x_it.c �ļ���
    	SysTick_Handler����������SysTick_ISR()�������ڱ��ļ�ĩβ��
    */	
//	SysTick_Config(SystemCoreClock / 16000);
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	//SysTick_Config(1600);
}

/*
*********************************************************************************************************
*	�� �� ��: SysTick_ISR
*	����˵��: SysTick�жϷ������ÿ��1ms����1��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t i;

	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&g_Tmr[i]);
	}

	g_iRunTime++;	/* ȫ������ʱ��ÿ1ms��1 */	
	if (g_iRunTime == 0x7FFFFFFF)
	{
		g_iRunTime = 0;
	}
		
	if (++s_count >= 10)
	{
		s_count = 0;



		/* 
			����Ĵ���ʵ�����а����ļ�⡣�������ÿ��10msһ�ξ����ˣ�һ����40ms���˲������Ϳ���
			��Ч���˵���е������ɵİ���������
		*/
		//bsp_KeyPro();		/* �ú����� bsp_button.c ��ʵ�� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SoftTimerDec
*	����˵��: ÿ��1ms�����ж�ʱ��������1�����뱻SysTick_ISR�����Ե��á�
*	��    �Σ�_tmr : ��ʱ������ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			/* �����ʱ����������1�����ö�ʱ�������־ */
			if (--_tmr->count == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayMS
*	����˵��: ms���ӳ٣��ӳپ���Ϊ����1ms
*	��    �Σ�n : �ӳٳ��ȣ���λ1 ms�� n Ӧ����2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t n)
{
	/* ���� n = 1 �������������� */
	if (n <= 1)
	{
		n = 2;
	}
	
	__set_PRIMASK(1);  		/* ���ж� */
	g_Tmr[0].count = n;
	g_Tmr[0].flag = 0;
	__set_PRIMASK(0);  		/* ���ж� */

	while (1)
	{
		CPU_IDLE();	/* �˴��ǿղ������û����Զ��壬��CPU����IDLE״̬���Խ��͹��ģ���ʵ��ι�� */

		/* �ȴ��ӳ�ʱ�䵽 */
		if (g_Tmr[0].flag == 1)
		{
			break;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartTimer
*	����˵��: ����һ����ʱ���������ö�ʱ���ڡ�
*	��    �Σ�	_id     : ��ʱ��ID��ֵ��1,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*						  ��ʱ��ID = 0 ������bsp_DelayMS()����
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* while(1); ���� */
		return;
	}

	__set_PRIMASK(1);  		/* ���ж� */
	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
	__set_PRIMASK(0);  		/* ���ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CheckTimer
*	����˵��: ��ⶨʱ���Ƿ�ʱ
*	��    �Σ�	_id     : ��ʱ��ID��ֵ��1,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*						  0 ����
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ���� 0 ��ʾ��ʱδ���� 1��ʾ��ʱ��
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (g_Tmr[_id].flag == 1)
	{
		g_Tmr[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetRunTime
*	����˵��: ��ȡCPU����ʱ�䣬��λ1ms
*	��    �Σ���
*	�� �� ֵ: CPU����ʱ�䣬��λ1ms
*********************************************************************************************************
*/
int32_t bsp_GetRunTime(void)
{
	int runtime; 

	__set_PRIMASK(1);  		/* ���ж� */
	
	runtime = g_iRunTime;	/* ������Systick�жϱ���д����˹��жϽ��б��� */
		
	__set_PRIMASK(0);  		/* ���ж� */

	return runtime;
}