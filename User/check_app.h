#ifndef __CHECK_APP_H
#define __CHECK_APP_H	

#include "stm32f10x.h"		

typedef struct 
{
	uint8_t Statue;         //����״̬ 0->Stop; 1->Slow; 2->Fast; 3->Error
	uint8_t OldStatue;      //��¼��һ��״̬
	uint8_t StartT_Flag;    //�����ֶ�����ʱ��ʱ��־
	uint8_t Power_Statue;   //������״̬��220V������ 0->����; 1->�쳣.
	uint8_t Work_Check;     //������⣬220V������   0->����; 1->�쳣.
	uint8_t Phase_Check;    //����/���ؼ�⣬�����ź�  0->����; 1->�쳣.
	uint8_t KM1_Point;      //KM1����״̬ 0->����; 1->�쳣.
	uint8_t KM2_Point;      //KM2����״̬ 0->����; 1->�쳣.
	uint8_t KM3_Point;      //KM3����״̬ 0->����; 1->�쳣.
	uint8_t KM1_Coil;       //KM1��Ȧ״̬ 0->����; 1->�쳣.
	uint8_t KM2_Coil;       //KM2��Ȧ״̬ 0->����; 1->�쳣.
	uint8_t KM3_Coil;       //KM3��Ȧ״̬ 0->����; 1->�쳣.
	uint8_t ErrorFlag;      //�����־ 0->����; 1->�쳣.ֻ����������������ݱ���
	uint16_t SlowTimer_Count;	//ˮ�õ�����ʱʱ����� �ݼ���ʽ	
	uint16_t DelayCheck_Count;	//��ʱ�쳣���ʱ����� �ݼ���ʽ	    
}MotorChar; //������Խṹ��


typedef enum
{
	Stop = 0,   //ֹͣ
    Slow,       //����
    Fast,       //����
    Error,      //����
}Run_Statue;

typedef enum
{
	Menu_Idle = 0, //����״̬ �����ѭ����ʾ ��ѹ���������¶ȡ�ʪ�ȡ�ѹ��������
    Menu_Work,     //ˮ�ù���   ˮ�ù�����ֻѭ����ʾ��ѹ������
    Menu_U,        //��ѹֵ
    Menu_Ua,       //��ѹ����
    Menu_Ub,       //��ѹ����
    Menu_A,        //����ֵ
    Menu_Aa,       //��������
    Menu_Ab,       //��������
    Menu_Ac,       //�������
    Menu_Ca,       //�¶�����
    Menu_Cb,       //�¶�����
    Menu_Ha,       //ʪ������
    Menu_Hb,       //ʪ������
    Menu_Pa,       //ѹ������
    Menu_Pb,       //ѹ������
    Menu_Fa,       //��������
    Menu_Fb,       //��������
    Menu_StartMode,//����ģʽ
    Menu_Size      //����ʹ��
}MENU;

typedef enum
{
	Show_U = 0,     //ѭ����ʾ ��ѹ
	Show_I,         //ѭ����ʾ ����
    Show_CF,        //ѭ����ʾ �¶ȡ�ʪ��
    Show_PL         //ѭ����ʾ ѹ��������
}SHOW;

typedef enum
{
	HH33 = 0, //������
    HH11,     //ֱ��
    HH44,     //����
    HHFJ      //���
}MotorMode;

typedef struct 
{
	uint16_t Vol_Limit_High;	//��ѹ����
	uint16_t Vol_Limit_Low;		//��ѹ����
	
	uint16_t Cur_Limit_High;	//��������
	uint16_t Cur_Limit_Low;		//��������
	
	uint16_t Temp_Limit_High;	//�¶�����
	uint16_t Temp_Limit_Low;	//�¶�����
	uint16_t Humi_Limit_High;	//ʪ������
	uint16_t Humi_Limit_Low;	//ʪ������
	
	uint16_t Press_Limit_High;	//ѹ������
	uint16_t Press_Limit_Low;	//ѹ������
	uint16_t Flow_Limit_High;	//��������
	uint16_t Flow_Limit_Low;	//��������
	
	uint16_t Motor_WorkMode;	//����ģʽ�������ǡ�ֱ�ӡ�������
	uint16_t swVer; 			//�汾��
}ParamDat;

#ifdef GLOBAL_APP
	#define APP_EXT
#else
	#define APP_EXT extern 
#endif
    
APP_EXT MotorChar gAp,gBp;  //A/B�������Ϣ
APP_EXT uint16_t gParamDat[Menu_Size];

void ReadInputDat(uint8_t _StartMode);
void KMOutAnswer(void);
void SetParam(void);
uint8_t KM_ApRunningCheck(uint8_t _StartMode);//A�����м��
uint8_t KM_BpRunningCheck(uint8_t _StartMode);//B�����м��
void KM_RunMode(uint8_t _RunMode,uint8_t _StartMode,uint8_t _RunDat);
uint8_t KM_RunningCheck(uint8_t _Auto,uint8_t _StartMode,uint8_t _RunDat);
uint8_t KMStart_Pro(uint8_t _RunMode,uint8_t _RunDat,uint8_t _StartMode);

#endif
