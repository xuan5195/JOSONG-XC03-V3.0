#ifndef __CHECK_APP_H
#define __CHECK_APP_H	

#include "stm32f10x.h"		

typedef struct 
{
	uint8_t Statue;         //����״̬ 0->Stop; 1->Slow; 2->HighSpeed; 3->Error
	uint8_t OldStatue;      //��¼��һ��״̬
	uint8_t Power_Statue;   //������״̬��220V������ 0->����; 1->�쳣.
	uint8_t Work_Check;     //������⣬220V������   0->����; 1->�쳣.
	uint8_t Phase_Check;    //����/���ؼ�⣬�����ź�  0->����; 1->�쳣.
	uint8_t KM1_Point;      //KM1����״̬ 0->����; 1->�쳣.
	uint8_t KM2_Point;      //KM2����״̬ 0->����; 1->�쳣.
	uint8_t KM3_Point;      //KM3����״̬ 0->����; 1->�쳣.
	uint8_t KM1_Coil;       //KM1��Ȧ״̬ 0->����; 1->�쳣.
	uint8_t KM2_Coil;       //KM2��Ȧ״̬ 0->����; 1->�쳣.
	uint8_t KM3_Coil;       //KM3��Ȧ״̬ 0->����; 1->�쳣.
	uint16_t Timer_Count;	//ˮ�õ�����ʱʱ����� �ݼ���ʽ	
	uint16_t DelayCheck_Count;	//��ʱ�쳣���ʱ����� �ݼ���ʽ	
}MotorChar; //������Խṹ��


typedef enum
{
	Stop = 0,   //ֹͣ
    Slow,       //����
    HighSpeed,  //����
    Error,      //����
}Run_Statue;

#ifdef GLOBAL_APP
	#define APP_EXT
#else
	#define APP_EXT extern 
#endif
    
APP_EXT MotorChar gAp,gBp;  //A/B�������Ϣ


void ReadInputDat(void);
void KMAutoRUN(uint8_t _Mode,uint8_t _Step);//�Զ���������


#endif
