#ifndef __CHECK_APP_H
#define __CHECK_APP_H	

#include "stm32f10x.h"		

typedef struct 
{
	uint8_t Statue;         //����״̬
	uint8_t Power_Statue;   //������״̬��220V������
	uint8_t Work_Check;     //������⣬220V������
	uint8_t Phase_Check;    //����/���ؼ�⣬�����ź�
	uint8_t KM1_Point;      //KM1����״̬
	uint8_t KM2_Point;      //KM2����״̬
	uint8_t KM3_Point;      //KM3����״̬
	uint8_t KM1_Coil;       //KM1��Ȧ״̬
	uint8_t KM2_Coil;       //KM2��Ȧ״̬
	uint8_t KM3_Coil;       //KM3��Ȧ״̬
	uint16_t Timer_Count;	//ˮ�õ�����ʱʱ����� �ݼ���ʽ	
}MotorChar; //������Խṹ��

#ifdef GLOBAL_APP
	#define APP_EXT
#else
	#define APP_EXT extern 
#endif
APP_EXT MotorChar gAp,gBp;  //A/B�������Ϣ


void ReadInputDat(uint8_t _ApDat,uint8_t _BpDat);
void KMAutoRUN(uint8_t _Mode,uint8_t _Step);//�Զ���������


#endif
