#include "stm32f10x.h"		
#include "delay.h"

#define GLOBAL_APP

#include "check_app.h"		
#include "bsp.h"

extern uint8_t g_KeyNoneCount;
extern uint8_t g_ChangeFlag;//��Ͷ��־

//_StartMode:����ģʽ�������ǡ�ֱ�ӡ�������
void ReadInputDat(uint8_t _StartMode)
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
    if((gAp.Phase_Check==1)||(gBp.Phase_Check==1))  {   RS485Dat_LED9_ON();     } //����/���� ָʾ��
    else                                            {   RS485Dat_LED9_OFF();    }

    switch (_StartMode)
    {
    	case HH33:  //������ģʽ
    	case HHFJ:  //���ģʽ
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
            else if(gAp.Statue==Fast)    //����״̬
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
            }
            else if(gBp.Statue==Fast)    //����״̬
            {
                if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
                }
            }     
    		break;
    	case HH11:  //ֱ��
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
            else
            {
                if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
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
            else
            {
                if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
                }
            }     
    		break;
    	case HH44:  //����
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
            else if(gAp.Statue==Fast)    //����״̬
            {
                if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1))
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
            }
            else if(gBp.Statue==Fast)    //����״̬
            {
                if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
                }
            }     
    		break;
    	default:
    		break;
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
//A�����м��
uint8_t KM_ApRunningCheck(uint8_t _StartMode)
{
	uint8_t uTemp=0;
	switch (_StartMode)
	{
		case HHFJ:	//���
		case HH33:	//������
			if(gAp.Statue==Slow)    	//A�õ���
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A��, KM1����,����.\r\n");
                        else                    printf("     A��, KM1����,�쳣.\r\n");
                        if(gAp.KM2_Point==1)    printf("     A��, KM2����,����.\r\n");
                        else                    printf("     A��, KM2����,�쳣.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A��, KM3����,����.\r\n");
                        else                    printf("     A��, KM3����,�쳣.\r\n");
                        if(gAp.Work_Check==0)   printf("     A��,�������,����.\r\n");
                        else                    printf("     A��,�������,�쳣.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A��,���ؼ��,����.\r\n");
                        else                    printf("     A��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			else if(gAp.Statue==Fast)	//A�ø���
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A��, KM1����,����.\r\n");
                        else                    printf("     A��, KM1����,�쳣.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A��, KM2����,����.\r\n");
                        else                    printf("     A��, KM2����,�쳣.\r\n");
                        if(gAp.KM3_Point==1)    printf("     A��, KM3����,����.\r\n");
                        else                    printf("     A��, KM3����,�쳣.\r\n");
                        if(gAp.Work_Check==0)   printf("     A��,�������,����.\r\n");
                        else                    printf("     A��,�������,�쳣.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A��,���ؼ��,����.\r\n");
                        else                    printf("     A��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		case HH11:	//ֱ��
			if(gAp.Statue==Slow)    //A�õ���
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A��, KM1����,����.\r\n");
                        else                    printf("     A��, KM1����,�쳣.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A��, KM2����,����.\r\n");
                        else                    printf("     A��, KM2����,�쳣.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A��, KM3����,����.\r\n");
                        else                    printf("     A��, KM3����,�쳣.\r\n");
                        if(gAp.Work_Check==0)   printf("     A��,�������,����.\r\n");
                        else                    printf("     A��,�������,�쳣.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A��,���ؼ��,����.\r\n");
                        else                    printf("     A��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		case HH44:	//����
			if(gAp.Statue==Slow)    	//A�õ���
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A��, KM1����,����.\r\n");
                        else                    printf("     A��, KM1����,�쳣.\r\n");
                        if(gAp.KM2_Point==1)    printf("     A��, KM2����,����.\r\n");
                        else                    printf("     A��, KM2����,�쳣.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A��, KM3����,����.\r\n");
                        else                    printf("     A��, KM3����,�쳣.\r\n");
                        if(gAp.Work_Check==0)   printf("     A��,�������,����.\r\n");
                        else                    printf("     A��,�������,�쳣.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A��,���ؼ��,����.\r\n");
                        else                    printf("     A��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			else if(gAp.Statue==Fast)	//A�ø���
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==0)    printf("     A��, KM1����,����.\r\n");
                        else                    printf("     A��, KM1����,�쳣.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A��, KM2����,����.\r\n");
                        else                    printf("     A��, KM2����,�쳣.\r\n");
                        if(gAp.KM3_Point==1)    printf("     A��, KM3����,����.\r\n");
                        else                    printf("     A��, KM3����,�쳣.\r\n");
                        if(gAp.Work_Check==0)   printf("     A��,�������,����.\r\n");
                        else                    printf("     A��,�������,�쳣.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A��,���ؼ��,����.\r\n");
                        else                    printf("     A��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		default:
			break;
	}
    return uTemp;    //�����
}

//B�����м��
uint8_t KM_BpRunningCheck(uint8_t _StartMode)
{
	uint8_t uTemp=0;
	switch (_StartMode)
	{
		case HHFJ:	//���
		case HH33:	//������
			if(gBp.Statue==Slow)    	//B�õ���
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B��, KM1����,����.\r\n");
                        else                    printf("     B��, KM1����,�쳣.\r\n");
                        if(gBp.KM2_Point==1)    printf("     B��, KM2����,����.\r\n");
                        else                    printf("     B��, KM2����,�쳣.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B��, KM3����,����.\r\n");
                        else                    printf("     B��, KM3����,�쳣.\r\n");
                        if(gBp.Work_Check==0)   printf("     B��,�������,����.\r\n");
                        else                    printf("     B��,�������,�쳣.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B��,���ؼ��,����.\r\n");
                        else                    printf("     B��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			else if(gBp.Statue==Fast)	//B�ø���
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B��, KM1����,����.\r\n");
                        else                    printf("     B��, KM1����,�쳣.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B��, KM2����,����.\r\n");
                        else                    printf("     B��, KM2����,�쳣.\r\n");
                        if(gBp.KM3_Point==1)    printf("     B��, KM3����,����.\r\n");
                        else                    printf("     B��, KM3����,�쳣.\r\n");
                        if(gBp.Work_Check==0)   printf("     B��,�������,����.\r\n");
                        else                    printf("     B��,�������,�쳣.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B��,���ؼ��,����.\r\n");
                        else                    printf("     B��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		case HH11:	//ֱ��
			if(gBp.Statue==Slow)    //B�õ���
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B��, KM1����,����.\r\n");
                        else                    printf("     B��, KM1����,�쳣.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B��, KM2����,����.\r\n");
                        else                    printf("     B��, KM2����,�쳣.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B��, KM3����,����.\r\n");
                        else                    printf("     B��, KM3����,�쳣.\r\n");
                        if(gBp.Work_Check==0)   printf("     B��,�������,����.\r\n");
                        else                    printf("     B��,�������,�쳣.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B��,���ؼ��,����.\r\n");
                        else                    printf("     B��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		case HH44:	//����
			if(gBp.Statue==Slow)    	//B�õ���
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B��, KM1����,����.\r\n");
                        else                    printf("     B��, KM1����,�쳣.\r\n");
                        if(gBp.KM2_Point==1)    printf("     B��, KM2����,����.\r\n");
                        else                    printf("     B��, KM2����,�쳣.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B��, KM3����,����.\r\n");
                        else                    printf("     B��, KM3����,�쳣.\r\n");
                        if(gBp.Work_Check==0)   printf("     B��,�������,����.\r\n");
                        else                    printf("     B��,�������,�쳣.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B��,���ؼ��,����.\r\n");
                        else                    printf("     B��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			else if(gBp.Statue==Fast)	//B�ø���
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //��������  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==0)    printf("     B��, KM1����,����.\r\n");
                        else                    printf("     B��, KM1����,�쳣.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B��, KM2����,����.\r\n");
                        else                    printf("     B��, KM2����,�쳣.\r\n");
                        if(gBp.KM3_Point==1)    printf("     B��, KM3����,����.\r\n");
                        else                    printf("     B��, KM3����,�쳣.\r\n");
                        if(gBp.Work_Check==0)   printf("     B��,�������,����.\r\n");
                        else                    printf("     B��,�������,�쳣.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B��,���ؼ��,����.\r\n");
                        else                    printf("     B��,���ؼ��,�쳣.\r\n");
                        uTemp = 0x01;    //�쳣����
                    }
				}
			}
			break;
		default:
			break;
	}
    return uTemp;    //�����
}


uint8_t Menu=Menu_Idle,g_ShowMode=Show_U;
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
                if(Menu==Menu_StartMode)
                {
                    Menu = Menu_Ua;       //��ѹ����
                }
                else if(Menu!=Menu_Idle)
                {
                    Menu++;	//�˵�ģʽ+1
                }
                break;
            case KEY_1_UP:							
                if(Menu==Menu_Idle)    //����ģʽ
                {
                    g_ShowMode = (g_ShowMode+1)%4;  //0-1-2-3-0
                }
                break;
            case KEY_1_LONG:							
                if(Menu==Menu_Idle)
                {
                    printf("�������ò˵�.\r\n------------------------\r\n");
                    Menu = Menu_Ua;       //��ѹ����
                }
                break;
            case KEY_2_DOWN:
                gParamDat[Menu]++;
                if(Menu==Menu_StartMode)
                {   if(gParamDat[Menu]>3)   gParamDat[Menu] = 3;    }
                else if(Menu==Menu_A)    //�������ֵ600A
                {   if(gParamDat[Menu]>600) gParamDat[Menu]=600; }
                else if((Menu==Menu_Ua)||(Menu==Menu_Ub)||(Menu==Menu_Aa)||(Menu==Menu_Ab))
                {   if(gParamDat[Menu]>10)  gParamDat[Menu]=10; }
                else //if((Menu==Menu_Ca)||(Menu==Menu_Cb)||(Menu==Menu_Ha)||(Menu==Menu_Hb))
                {   if(gParamDat[Menu]>99)  gParamDat[Menu]=99; }
                break;
            case KEY_3_DOWN:	
                if(gParamDat[Menu]==0)  gParamDat[Menu]=0;
                else                    gParamDat[Menu]--;
                break;
            case KEY_4_DOWN:	
                if(Menu!=Menu_Idle)
                {
                    printf("   ��ѹ(Limit:%2d-%2d).\r\n",gParamDat[Menu_Ub],gParamDat[Menu_Ua]);
                    printf("   ����(Limit:%2d-%2d).  ����:%2d.  ���:%2d.\r\n",gParamDat[Menu_Ab],gParamDat[Menu_Aa],gParamDat[Menu_A],gParamDat[Menu_Ac]);
                    printf("   �¶�(Limit:%2d-%2d).  ʪ��(Limit:%2d-%2d).\r\n",gParamDat[Menu_Cb],gParamDat[Menu_Ca],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                    printf("   ѹ��(Limit:%2d-%2d).  ����(Limit:%2d-%2d).\r\n",gParamDat[Menu_Pb],gParamDat[Menu_Pa],gParamDat[Menu_Fb],gParamDat[Menu_Fa]);
                    if(gParamDat[Menu_StartMode]==0x00)        {   printf("   ����ģʽ��HH33 ������.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x01)   {   printf("   ����ģʽ��HH11 ֱ��.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x02)   {   printf("   ����ģʽ��HH44 ����.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x03)   {   printf("   ����ģʽ��HHFJ ���.\r\n");}
                    Write_Flash_Dat();  //дFlash����
                    Menu = Menu_Idle;     //��������˳�
                    printf("����,�˳��˵�.\r\n------------------------\r\n");
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


//_StartMode:����ģʽ�������ǡ�ֱ�ӡ�������
//_RunDat:���з�ʽ��ֹͣ/����/����
void KM_RunMode(uint8_t _RunMode,uint8_t _StartMode,uint8_t _RunDat)
{
	switch (_StartMode)
	{
		case HH33:	//������
			if((_RunDat&0x0F)==0x01)		//A��ֹͣ        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A�õ���
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A�ø���
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B��ֹͣ
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B�õ���
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B�ø��� 
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
			}                 
			break;
		case HH11:	//ֱ��
			if((_RunDat&0x0F)==0x01)		//A��ֹͣ        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A��KM1
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			if((_RunDat&0xF0)==0x10)   		//B��ֹͣ
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B��KM1
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			break;
		case HH44:	//����
			if((_RunDat&0x0F)==0x01)		//A��ֹͣ        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A�õ���
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A�ø���
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B��ֹͣ
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B�õ���
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B�ø��� 
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
			}                 
			break;
		case HHFJ:	//���
			if((_RunDat&0x0F)==0x01)		//A��ֹͣ        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A�õ���
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;if(_RunMode==0)gAp.SlowTimer_Count=250;else gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A�ø���
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B��ֹͣ
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B�õ���
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;if(_RunMode==0)gBp.SlowTimer_Count=250;else gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B�ø��� 
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
			}                 
			break;
		default:
			break;
	}
}

//_RunMode:�Զ�/�ֶ�
//_StartMode:����ģʽ�������ǡ�ֱ�ӡ�������
//_RunDat:���з�ʽ��ֹͣ/����/����
//uint8_t g_ChangeFlag=0;//��Ͷ��־
uint8_t KMStart_Pro(uint8_t _RunMode,uint8_t _RunDat,uint8_t _StartMode)
{
	if(g_ChangeFlag==0xAA)//��Ͷ��־
	{
		if(_RunMode==1)	{_RunMode=2;if((_RunDat==0x12)||(_RunDat==0x11))_RunDat=0x21;if(_RunDat==0x14)_RunDat=0x41; }
		else			{_RunMode=1;if((_RunDat==0x21)||(_RunDat==0x11))_RunDat=0x12;if(_RunDat==0x41)_RunDat=0x14; }
	}

	KM_RunMode(_RunMode,_StartMode,_RunDat);	//_StartMode:����ģʽ;_RunDat:���з�ʽ��ֹͣ/����/����
	if(_RunMode==0)	//�ֶ�
	{
		if((_RunDat&0x0F)!=0x01)	//A�ö�����
		{
			if(0x00!=KM_ApRunningCheck(_StartMode))//A���쳣������ֹͣ
			{
				_RunDat = (_RunDat&0xF0)|0x01;
				printf("A���쳣��ֹͣ.0x%02X.\r\n",_RunDat);	
                gAp.ErrorFlag=1;
			}
			else
			{
				if(gAp.SlowTimer_Count==0)	_RunDat = (_RunDat&0xF0)|0x04;	//ת�����
			}
		}
		if((_RunDat&0xF0)!=0x10)	//B�ö�����
		{
			if(0x00!=KM_BpRunningCheck(_StartMode))//B���쳣������ֹͣ
			{
				_RunDat = (_RunDat&0x0F)|0x10;
				printf("B���쳣��ֹͣ.0x%02X.\r\n",_RunDat);				
                gBp.ErrorFlag=1;
			}
			else
			{
				if(gBp.SlowTimer_Count==0)	_RunDat = (_RunDat&0x0F)|0x40;	//ת�����
			}
		}		
	}
	else if(_RunMode==1)	//�Զ� ��һ����
	{
		if((_RunDat&0x0F)!=0x01)	//A�ö�����
		{
			if(0x00!=KM_ApRunningCheck(_StartMode))//A���쳣������ֹͣ
			{
				_RunDat = (_RunDat&0xF0)|0x01;
				printf("A���쳣��ֹͣ.0x%02X.\r\n",_RunDat);				
                gAp.ErrorFlag=1;
				if(g_ChangeFlag==0x00)	{	g_ChangeFlag=0xAA;	printf("��Ͷ�л�.  ");}//��Ͷ��־
				else					{	g_ChangeFlag=0xBB;	printf("�ѻ�Ͷ�������˳�!\r\n");}//��Ͷ��־
			}
			else
			{
				if(gAp.SlowTimer_Count==0)	_RunDat = (_RunDat&0xF0)|0x04;	//ת�����
			}
		}
	}
	else if(_RunMode==2)	//�Զ� ������һ
	{
		if((_RunDat&0xF0)!=0x10)	//B�ö�����
		{
			if(0x00!=KM_BpRunningCheck(_StartMode))//B���쳣������ֹͣ
			{
				_RunDat = (_RunDat&0x0F)|0x10;
                gBp.ErrorFlag=1;
				printf("B���쳣��ֹͣ.0x%02X.\r\n",_RunDat);				
				if(g_ChangeFlag==0x00)	{	g_ChangeFlag=0xAA;	printf("��Ͷ�л�.  ");}//��Ͷ��־
				else					{	g_ChangeFlag=0xBB;	printf("�ѻ�Ͷ�������˳�!\r\n");}//��Ͷ��־
			}
			else
			{
				if(gBp.SlowTimer_Count==0)	_RunDat = (_RunDat&0x0F)|0x40;	//ת�����
			}
		}		
	}
	return _RunDat;
}




