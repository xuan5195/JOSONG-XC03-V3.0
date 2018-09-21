#ifndef __CHECK_APP_H
#define __CHECK_APP_H	

#include "stm32f10x.h"		

typedef struct 
{
	uint8_t Statue;         //运行状态 0->Stop; 1->Slow; 2->Fast; 3->Error
	uint8_t OldStatue;      //记录上一次状态
	uint8_t StartT_Flag;    //用于手动启动时延时标志
	uint8_t Power_Statue;   //动力电状态，220V脉冲检测 0->正常; 1->异常.
	uint8_t Work_Check;     //工作检测，220V脉冲检测   0->正常; 1->异常.
	uint8_t Phase_Check;    //断相/过载检测，开关信号  0->正常; 1->异常.
	uint8_t KM1_Point;      //KM1触点状态 0->正常; 1->异常.
	uint8_t KM2_Point;      //KM2触点状态 0->正常; 1->异常.
	uint8_t KM3_Point;      //KM3触点状态 0->正常; 1->异常.
	uint8_t KM1_Coil;       //KM1线圈状态 0->正常; 1->异常.
	uint8_t KM2_Coil;       //KM2线圈状态 0->正常; 1->异常.
	uint8_t KM3_Coil;       //KM3线圈状态 0->正常; 1->异常.
	uint8_t ErrorFlag;      //错误标志 0->正常; 1->异常.只用于启动后故障数据保存
	uint16_t SlowTimer_Count;	//水泵低速延时时间计数 递减方式	
	uint16_t DelayCheck_Count;	//延时异常检测时间计数 递减方式	    
}MotorChar; //电机特性结构体


typedef enum
{
	Stop = 0,   //停止
    Slow,       //低速
    Fast,       //高速
    Error,      //错误
}Run_Statue;

typedef enum
{
	Menu_Idle = 0, //待机状态 数码管循环显示 电压、电流、温度、湿度、压力、流量
    Menu_Work,     //水泵工作   水泵工作，只循环显示电压、电流
    Menu_U,        //电压值
    Menu_Ua,       //电压上限
    Menu_Ub,       //电压下限
    Menu_A,        //电流值
    Menu_Aa,       //电流上限
    Menu_Ab,       //电流下限
    Menu_Ac,       //电流变比
    Menu_Ca,       //温度上限
    Menu_Cb,       //温度下限
    Menu_Ha,       //湿度上限
    Menu_Hb,       //湿度下限
    Menu_Pa,       //压力上限
    Menu_Pb,       //压力下限
    Menu_Fa,       //流量上限
    Menu_Fb,       //流量下限
    Menu_StartMode,//启动模式
    Menu_Size      //长度使用
}MENU;

typedef enum
{
	Show_U = 0,     //循环显示 电压
	Show_I,         //循环显示 电流
    Show_CF,        //循环显示 温度、湿度
    Show_PL         //循环显示 压力、流量
}SHOW;

typedef enum
{
	HH33 = 0, //星三角
    HH11,     //直接
    HH44,     //自耦
    HHFJ      //风机
}MotorMode;

typedef struct 
{
	uint16_t Vol_Limit_High;	//电压上限
	uint16_t Vol_Limit_Low;		//电压下限
	
	uint16_t Cur_Limit_High;	//电流上限
	uint16_t Cur_Limit_Low;		//电流下限
	
	uint16_t Temp_Limit_High;	//温度上限
	uint16_t Temp_Limit_Low;	//温度下限
	uint16_t Humi_Limit_High;	//湿度上限
	uint16_t Humi_Limit_Low;	//湿度下限
	
	uint16_t Press_Limit_High;	//压力上限
	uint16_t Press_Limit_Low;	//压力下限
	uint16_t Flow_Limit_High;	//流量上限
	uint16_t Flow_Limit_Low;	//流量下限
	
	uint16_t Motor_WorkMode;	//启动模式，星三角、直接、自耦、风机
	uint16_t swVer; 			//版本号
}ParamDat;

#ifdef GLOBAL_APP
	#define APP_EXT
#else
	#define APP_EXT extern 
#endif
    
APP_EXT MotorChar gAp,gBp;  //A/B泵相关信息
APP_EXT uint16_t gParamDat[Menu_Size];

void ReadInputDat(uint8_t _StartMode);
void KMOutAnswer(void);
void SetParam(void);
uint8_t KM_ApRunningCheck(uint8_t _StartMode);//A泵运行检测
uint8_t KM_BpRunningCheck(uint8_t _StartMode);//B泵运行检测
void KM_RunMode(uint8_t _RunMode,uint8_t _StartMode,uint8_t _RunDat);
uint8_t KM_RunningCheck(uint8_t _Auto,uint8_t _StartMode,uint8_t _RunDat);
uint8_t KMStart_Pro(uint8_t _RunMode,uint8_t _RunDat,uint8_t _StartMode);

#endif
