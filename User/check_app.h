#ifndef __CHECK_APP_H
#define __CHECK_APP_H	

#include "stm32f10x.h"		

typedef struct 
{
	uint8_t Statue;         //运行状态
	uint8_t Power_Statue;   //动力电状态，220V脉冲检测
	uint8_t Work_Check;     //工作检测，220V脉冲检测
	uint8_t Phase_Check;    //断相/过载检测，开关信号
	uint8_t KM1_Point;      //KM1触点状态
	uint8_t KM2_Point;      //KM2触点状态
	uint8_t KM3_Point;      //KM3触点状态
	uint8_t KM1_Coil;       //KM1线圈状态
	uint8_t KM2_Coil;       //KM2线圈状态
	uint8_t KM3_Coil;       //KM3线圈状态
	uint16_t Timer_Count;	//水泵低速延时时间计数 递减方式	
}MotorChar; //电机特性结构体

#ifdef GLOBAL_APP
	#define APP_EXT
#else
	#define APP_EXT extern 
#endif
APP_EXT MotorChar gAp,gBp;  //A/B泵相关信息


void ReadInputDat(uint8_t _ApDat,uint8_t _BpDat);
void KMAutoRUN(uint8_t _Mode,uint8_t _Step);//自动启动控制


#endif
