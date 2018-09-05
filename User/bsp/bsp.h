/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2017-2018, 厦门易聪科技有限公司
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

/* 定义 BSP 版本号 */
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

#define VERSION		    0x01		//固件版本号

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

