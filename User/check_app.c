#include "stm32f10x.h"		
#include "delay.h"

#define GLOBAL_APP

#include "check_app.h"		
#include "bsp.h"

extern uint8_t g_KeyNoneCount;
extern uint8_t g_ChangeFlag;

void ReadInputDat(void)
{
    gAp.KM1_Point = Read_Optocoupler(KMA1_PointNo);//KM1����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    gAp.KM2_Point = Read_Optocoupler(KMA2_PointNo);//KM2����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    gAp.KM3_Point = Read_Optocoupler(KMA3_PointNo);//KM3����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    gBp.KM1_Point = Read_Optocoupler(KMB1_PointNo);//KM1����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    gBp.KM2_Point = Read_Optocoupler(KMB2_PointNo);//KM2����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    gBp.KM3_Point = Read_Optocoupler(KMB3_PointNo);//KM3����״̬ NC���մ��� 0->�պϣ�1->�Ͽ�
    
    gAp.Power_Statue = Read_InputDevDat(2);//A�ö����磻0->����;1->�쳣 
    gBp.Power_Statue = Read_InputDevDat(3);//B�ö����磻0->����;1->�쳣 

    gAp.Work_Check = Read_InputDevDat(0);//A�ù�����⣻0->����;1->�쳣 
    gBp.Work_Check = Read_InputDevDat(1);//B�ù�����⣻0->����;1->�쳣 

    gAp.KM1_Coil = Read_Input_KPDevDat(KMA1_CoilNo);//KM1��Ȧ״̬��0->����;1->�쳣
    gAp.KM2_Coil = Read_Input_KPDevDat(KMA2_CoilNo);//KM2��Ȧ״̬��0->����;1->�쳣
    gAp.KM3_Coil = Read_Input_KPDevDat(KMA3_CoilNo);//KM3��Ȧ״̬��0->����;1->�쳣
    gBp.KM1_Coil = Read_Input_KPDevDat(KMB1_CoilNo);//KM1��Ȧ״̬��0->����;1->�쳣
    gBp.KM2_Coil = Read_Input_KPDevDat(KMB2_CoilNo);//KM2��Ȧ״̬��0->����;1->�쳣
    gBp.KM3_Coil = Read_Input_KPDevDat(KMB3_CoilNo);//KM3��Ȧ״̬��0->����;1->�쳣

    gAp.Phase_Check = Read_Optocoupler(6);  //A����/���ؼ��,�����ź�; 0->�쳣; 1->����.
    gBp.Phase_Check = Read_Optocoupler(7);  //B����/���ؼ��,�����ź�; 0->�쳣; 1->����.
   
    if(Read_Optocoupler(4)) { RS485Dat_LED10_OFF(); }//ȱˮ���,������ˮ ȱˮָʾ��OFF        
    else                    { RS485Dat_LED10_ON();  }//ȱˮ���,�쳣ȱˮ ȱˮָʾ��ON    
//  if(Read_Optocoupler(5))//���ڽ�ˮ��⣬������δ��ˮ
//  else				   //���ڽ�ˮ��⣬�쳣�������Զ��ֶ���������ˮ��

    if((gAp.Power_Statue==0)&&(gBp.Power_Statue==0)){   RS485Dat_LED4_ON();     } //������ָʾ��
    else                                            {   RS485Dat_LED4_OFF();    }
    if((gAp.Phase_Check==0)||(gBp.Phase_Check==0))  {   RS485Dat_LED9_ON();     } //����/���� ָʾ��
    else                                            {   RS485Dat_LED9_OFF();    }

    if(gAp.Statue==Stop)    //ֹͣ״̬
    {
        if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
        {
            RS485Dat_LED12_ON();RS485Dat_LED13_OFF();RS485Dat_LED14_OFF();
        }
        if( (gAp.KM1_Point==0)&&(gAp.KM2_Point==0x00)&&(gAp.KM3_Point==0x00)&&
            (gAp.KM1_Coil==0)&&(gAp.KM2_Coil==0x00)&&(gAp.KM3_Coil==0x00)&&(gAp.Power_Statue==0))    	
                {   RS485Dat_LED7_OFF();    } //��һ����ָʾ��
        else    {   RS485Dat_LED7_ON();     }
    }
    else if(gAp.Statue==Slow)    //����״̬
    {
        if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0))
        {
            RS485Dat_LED12_OFF();RS485Dat_LED13_ON();RS485Dat_LED14_OFF();
        }
    }
    else if(gAp.Statue==HighSpeed)    //����״̬
    {
        if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1))
        {
            RS485Dat_LED12_OFF();RS485Dat_LED13_OFF();RS485Dat_LED14_ON();
        }
    }    
    if(gBp.Statue==Stop)    //ֹͣ״̬
    {
        if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0))
        {
            RS485Dat_LED18_ON();RS485Dat_LED19_OFF();RS485Dat_LED20_OFF();
        }
        if( (gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)&&
            (gBp.KM1_Coil==0)&&(gBp.KM2_Coil==0)&&(gBp.KM3_Coil==0)&&(gBp.Power_Statue==0) )    	
                {   RS485Dat_LED8_OFF();    } //��������ָʾ��
        else    {   RS485Dat_LED8_ON();     }
    }
    else if(gBp.Statue==Slow)    //����״̬
    {
        if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0))
        {
            RS485Dat_LED18_OFF();RS485Dat_LED19_ON();RS485Dat_LED20_OFF();
        }
//        if(gBp.DelayCheck_Count==0) //��ʱ���ʱ�䵽��������
//        {
//            if(gBp.Work_Check==1)   //�������ʧ�ܣ�תΪ
//        }                
    }
    else if(gBp.Statue==HighSpeed)    //����״̬
    {
        if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1))
        {
            RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
        }
    }     
}

//��Դ/��Դ�̵����������
void KMOutAnswer(void)
{
    if((gAp.Statue == Stop)&&(gBp.Statue == Stop))
    {
        KMOFF_Show(ALARMOUT1);  KMOFF_Show(ALARMOUT2);
    }
    else
    {
        KMON_Show(ALARMOUT1);  KMON_Show(ALARMOUT2);
    }
}

void KMAutoRUN(uint8_t _Mode,uint8_t _Step)//�Զ���������
{
    if(g_ChangeFlag==0xAA)  //���뻥Ͷ
    {
        if(_Mode==1)    _Mode=2;
        else            _Mode=1;
    }
    else if(g_ChangeFlag==0xBB)  //��Ͷ��һ�Σ�ֱ���˳�
    {
        _Mode = 0;  //��Ϊ0����������������
    }
    if(_Mode==1)    //��һ����
    {
        if(_Step==0)
        {
            printf("��һ������ֹͣ.\r\n");
            gAp.Statue = Stop;
            KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //ֹͣ
            KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //ֹͣ
        }
        else if(_Step==1)
        {
            printf("��һ����������.\r\n");
            gAp.Statue = Slow;  gAp.DelayCheck_Count = 30;
            KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //��������
        }
        else if(_Step==2)
        {
            printf("��һ����������.\r\n");
            gAp.Statue = HighSpeed; gAp.DelayCheck_Count = 30;
            KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMON_Show(AKM3RUN);    //��������
        }
    }
    else if(_Mode==2)    //������һ
    {
        if(_Step==0)
        {
            printf("������һ��ֹͣ.\r\n");
            gBp.Statue = Stop;
            KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //ֹͣ
            KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //ֹͣ
        }
        else if(_Step==1)
        {
            printf("������һ������.\r\n");
            gBp.Statue = Slow; gBp.DelayCheck_Count = 30;
            KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //��������
        }
        else if(_Step==2)
        {
            printf("������һ������.\r\n");
            gBp.Statue = HighSpeed; gBp.DelayCheck_Count = 30;
            KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMON_Show(BKM3RUN);    //��������
        }
    }
}

//A�����м��
uint8_t KM_ApRunningCheck(void)
{
    if(gAp.Statue==Slow)    //A�õ���
    {
        if(gAp.DelayCheck_Count==0)
        {
            if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
                &&(gAp.Work_Check==0)&&(gAp.Phase_Check==1))  
                    return 0x00;    //��������
            else    return 0x01;    //�쳣����
        }
    }
    else if(gAp.Statue==HighSpeed)    //A�ø���
    {
        if(gAp.DelayCheck_Count==0)
        {
            if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
                &&(gAp.Work_Check==0)&&(gAp.Phase_Check==1))  
                    return 0x00;
            else    return 0x01;
        }
    }
    return 0x02;    //δ���
}

//B�����м��
uint8_t KM_BpRunningCheck(void)
{
    if(gBp.Statue==Slow)    //B�õ���
    {
        if(gBp.DelayCheck_Count==0)
        {
            if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
                &&(gBp.Work_Check==0)&&(gBp.Phase_Check==1))  
                    return 0x00;    //��������
            else    return 0x01;    //�쳣����
        }
    }
    else if(gBp.Statue==HighSpeed)    //B�ø���
    {
        if(gBp.DelayCheck_Count==0)
        {
            if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
                &&(gBp.Work_Check==0)&&(gBp.Phase_Check==1))  
                    return 0x00;
            else    return 0x01;
        }
    }
    return 0x02;    //δ���
}

//�Զ�ģʽ�£�A/B�ü��
uint8_t KM_RunningAutoCheck(uint8_t _Mode)
{
    if(g_ChangeFlag==0xAA)  //���뻥Ͷ
    {
        if(_Mode==1)    _Mode=2;
        else            _Mode=1;
    }
    else if(g_ChangeFlag==0xBB)  //��Ͷ��һ�Σ�ֱ���˳�
    {
        _Mode = 0;  //��Ϊ0����������������
    }
    if(_Mode==1)    //��һ����ģʽ
    {
        if(0x01==KM_ApRunningCheck())   
        {
            printf("A���쳣.\r\n");
            return 0x01;
        }//A���쳣������ֹͣ
        return 0x00; 
    }
    else if(_Mode==2)    //������һģʽ
    {
        if(0x01==KM_BpRunningCheck())   
        {
            printf("B���쳣.\r\n");
            return 0x01;
        }//B���쳣������ֹͣ
        return 0x00; 
    }
    return 0x00; 
}

uint8_t Menu=Menu_Idle;
//���ò��� ���ѹ���������޵�
void SetParam(void)
{
	uint8_t KeyDat=0;
    KeyDat = bsp_GetKey();
    if(KeyDat!=KEY_NONE)	//������⼰���ݴ���
    {
        switch (KeyDat)
        {
            case KEY_NONE:	//�ް�������
                break;
            case KEY_1_DOWN:							
                if(Menu==Menu_Fb)
                {
                    Menu = Menu_Ua;       //��ѹ����
                }
                else if(Menu!=Menu_Idle)
                {
                    Menu++;	//�˵�ģʽ+1
                }
                break;
            case KEY_1_LONG:							
                if(Menu==Menu_Idle)
                {
                    printf("�������ò˵�.\r\n");
                    Menu = Menu_Ua;       //��ѹ����
                }
                break;
            case KEY_2_DOWN:
                gParamDat[Menu]++;
                if(Menu==Menu_A)    //�������ֵ600A
                {   if(gParamDat[Menu]>600)  gParamDat[Menu]=600; }
                else if((Menu==Menu_Ua)||(Menu==Menu_Ub)||(Menu==Menu_Aa)||(Menu==Menu_Ab))
                {   if(gParamDat[Menu]>10)  gParamDat[Menu]=10; }
                else //if((Menu==Menu_Ca)||(Menu==Menu_Cb)||(Menu==Menu_Ha)||(Menu==Menu_Hb))
                {   if(gParamDat[Menu]>99)  gParamDat[Menu]=99; }
                break;
            case KEY_3_DOWN:	
                gParamDat[Menu]--;
                if(gParamDat[Menu]==0)  gParamDat[Menu]=0;
                break;
            case KEY_4_DOWN:	
                if(Menu!=Menu_Idle)
                {
                    printf("��ѹ����:%2d.��ѹ����:%2d.\r\n",gParamDat[Menu_Ua],gParamDat[Menu_Ub]);
                    printf("��������:%3d.��������:%2d.��������:%2d.�������:%2d.\r\n",gParamDat[Menu_A],gParamDat[Menu_Aa],gParamDat[Menu_Ab],gParamDat[Menu_Ac]);
                    printf("�¶�����:%2d.�¶�����:%2d.ʪ������:%2d.ʪ������:%2d.\r\n",gParamDat[Menu_Ca],gParamDat[Menu_Cb],gParamDat[Menu_Ha],gParamDat[Menu_Hb]);
                    printf("ѹ������:%2d.ѹ������:%2d.��������:%2d.��������:%2d.\r\n",gParamDat[Menu_Pa],gParamDat[Menu_Pb],gParamDat[Menu_Fa],gParamDat[Menu_Fb]);
                    Write_Flash_Dat();  //дFlash����
                    Menu = Menu_Idle;     //��������˳�
                    printf("����,�˳��˵�.\r\n");
                }
                break;
            default:
                break;
        }
        g_KeyNoneCount=40;    //�ް������£��˳���������
    }
    if((g_KeyNoneCount<5)&&(Menu!=Menu_Idle))
    {   
        g_KeyNoneCount=0;
        Menu=Menu_Idle;     //����������˳�
        printf("������,�˳��˵�.\r\n");
    }
}

