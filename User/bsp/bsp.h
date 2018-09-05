/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*	Copyright (C), 2017-2018, �����״ϿƼ����޹�˾
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

/* ���� BSP �汾�� */
#define __STM32F1_BSP_VERSION		"1.1"

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define VERSION		    0x01		//�̼��汾��

#if 0
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif

#include "bsp_timer.h"		
#include "bsp_TimerTim3.h"
#include "bsp_Tim2.h"
#include "bsp_tm1639.h"		
#include "bsp_can.h"
#include "bsp_uart_fifo.h"
#include "bsp_crc8.h"
#include "bsp_adc.h"
#include "bsp_stmflash.h"
#include "bsp_logfile.h"
#include "bsp_fifo.h"
#include "bsp_fifo_key.h"
#include "bsp_Dht11.h"
#include "bsp_hc595.h"		
#include "bsp_cd4067.h"		
#include "bsp_cd4051.h"		
#include "bsp_cs5463.h"		

#include "bus_app.h"		
#include "check_app.h"		

#define HardWDG_ON()  GPIO_SetBits(GPIOA, GPIO_Pin_4)  	       	//
#define HardWDG_OFF()  GPIO_ResetBits(GPIOA, GPIO_Pin_4) 	   	//

#endif

