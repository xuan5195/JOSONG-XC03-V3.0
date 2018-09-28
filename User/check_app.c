#include "stm32f10x.h"		
#include "delay.h"

#define GLOBAL_APP

#include "check_app.h"		
#include "bsp.h"

extern uint8_t g_KeyNoneCount;
extern uint8_t g_ChangeFlag;//互投标志

//_StartMode:启动模式，星三角、直接、自耦、风机
void ReadInputDat(uint8_t _StartMode)
{
    gAp.KM1_Point = Read_Optocoupler(KMA1_PointNo);//KM1触点状态 NC常闭触点 0->闭合；1->断开
    gAp.KM2_Point = Read_Optocoupler(KMA2_PointNo);//KM2触点状态 NC常闭触点 0->闭合；1->断开
    gAp.KM3_Point = Read_Optocoupler(KMA3_PointNo);//KM3触点状态 NC常闭触点 0->闭合；1->断开
    gBp.KM1_Point = Read_Optocoupler(KMB1_PointNo);//KM1触点状态 NC常闭触点 0->闭合；1->断开
    gBp.KM2_Point = Read_Optocoupler(KMB2_PointNo);//KM2触点状态 NC常闭触点 0->闭合；1->断开
    gBp.KM3_Point = Read_Optocoupler(KMB3_PointNo);//KM3触点状态 NC常闭触点 0->闭合；1->断开
    
    gAp.Power_Statue = Read_InputDevDat(2);//A泵动力电；0->正常;1->异常 
    gBp.Power_Statue = Read_InputDevDat(3);//B泵动力电；0->正常;1->异常 

    gAp.Work_Check = Read_InputDevDat(0);//A泵工作检测；0->正常;1->异常 
    gBp.Work_Check = Read_InputDevDat(1);//B泵工作检测；0->正常;1->异常 

    gAp.KM1_Coil = Read_Input_KPDevDat(KMA1_CoilNo);//KM1线圈状态；0->正常;1->异常
    gAp.KM2_Coil = Read_Input_KPDevDat(KMA2_CoilNo);//KM2线圈状态；0->正常;1->异常
    gAp.KM3_Coil = Read_Input_KPDevDat(KMA3_CoilNo);//KM3线圈状态；0->正常;1->异常
    gBp.KM1_Coil = Read_Input_KPDevDat(KMB1_CoilNo);//KM1线圈状态；0->正常;1->异常
    gBp.KM2_Coil = Read_Input_KPDevDat(KMB2_CoilNo);//KM2线圈状态；0->正常;1->异常
    gBp.KM3_Coil = Read_Input_KPDevDat(KMB3_CoilNo);//KM3线圈状态；0->正常;1->异常

    gAp.Phase_Check = Read_Optocoupler(6);  //A断相/过载检测,开关信号; 0->异常; 1->正常.
    gBp.Phase_Check = Read_Optocoupler(7);  //B断相/过载检测,开关信号; 0->异常; 1->正常.
   
    if(Read_Optocoupler(4)) { RS485Dat_LED10_OFF(); }//缺水检测,正常有水 缺水指示灯OFF        
    else                    { RS485Dat_LED10_ON();  }//缺水检测,异常缺水 缺水指示灯ON    
//  if(Read_Optocoupler(5))//箱内进水检测，正常，未进水
//  else				   //箱内进水检测，异常，主机自动手动不能启动水泵

    if((gAp.Power_Statue==0)&&(gBp.Power_Statue==0)){   RS485Dat_LED4_ON();     } //动力电指示灯
    else                                            {   RS485Dat_LED4_OFF();    }
    if((gAp.Phase_Check==1)||(gBp.Phase_Check==1))  {   RS485Dat_LED9_ON();     } //断相/过载 指示灯
    else                                            {   RS485Dat_LED9_OFF();    }

    switch (_StartMode)
    {
    	case HH33:  //星三角模式
    	case HHFJ:  //风机模式
            if(gAp.Statue==Stop)    //停止状态
            {
                if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_ON();RS485Dat_LED13_OFF();RS485Dat_LED14_OFF();
                }
                if( (gAp.KM1_Point==0)&&(gAp.KM2_Point==0x00)&&(gAp.KM3_Point==0x00)&&
                    (gAp.KM1_Coil==0)&&(gAp.KM2_Coil==0x00)&&(gAp.KM3_Coil==0x00)&&(gAp.Power_Statue==0))    	
                        {   RS485Dat_LED7_OFF();    } //主一故障指示灯
                else    {   RS485Dat_LED7_ON();     }
            }
            else if(gAp.Statue==Slow)    //低速状态
            {
                if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_OFF();RS485Dat_LED13_ON();RS485Dat_LED14_OFF();
                }
            }
            else if(gAp.Statue==Fast)    //高速状态
            {
                if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1))
                {
                    RS485Dat_LED12_OFF();RS485Dat_LED13_OFF();RS485Dat_LED14_ON();
                }
            }    
            if(gBp.Statue==Stop)    //停止状态
            {
                if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_ON();RS485Dat_LED19_OFF();RS485Dat_LED20_OFF();
                }
                if( (gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)&&
                    (gBp.KM1_Coil==0)&&(gBp.KM2_Coil==0)&&(gBp.KM3_Coil==0)&&(gBp.Power_Statue==0) )    	
                        {   RS485Dat_LED8_OFF();    } //主二故障指示灯
                else    {   RS485Dat_LED8_ON();     }
            }
            else if(gBp.Statue==Slow)    //低速状态
            {
                if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_ON();RS485Dat_LED20_OFF();
                }               
            }
            else if(gBp.Statue==Fast)    //高速状态
            {
                if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
                }
            }     
    		break;
    	case HH11:  //直启
            if(gAp.Statue==Stop)    //停止状态
            {
                if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_ON();RS485Dat_LED13_OFF();RS485Dat_LED14_OFF();
                }
                if( (gAp.KM1_Point==0)&&(gAp.KM2_Point==0x00)&&(gAp.KM3_Point==0x00)&&
                    (gAp.KM1_Coil==0)&&(gAp.KM2_Coil==0x00)&&(gAp.KM3_Coil==0x00)&&(gAp.Power_Statue==0))    	
                        {   RS485Dat_LED7_OFF();    } //主一故障指示灯
                else    {   RS485Dat_LED7_ON();     }
            }
            else
            {
                if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_OFF();RS485Dat_LED13_OFF();RS485Dat_LED14_ON();
                }
            }    
            if(gBp.Statue==Stop)    //停止状态
            {
                if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_ON();RS485Dat_LED19_OFF();RS485Dat_LED20_OFF();
                }
                if( (gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)&&
                    (gBp.KM1_Coil==0)&&(gBp.KM2_Coil==0)&&(gBp.KM3_Coil==0)&&(gBp.Power_Statue==0) )    	
                        {   RS485Dat_LED8_OFF();    } //主二故障指示灯
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
    	case HH44:  //自耦
            if(gAp.Statue==Stop)    //停止状态
            {
                if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_ON();RS485Dat_LED13_OFF();RS485Dat_LED14_OFF();
                }
                if( (gAp.KM1_Point==0)&&(gAp.KM2_Point==0x00)&&(gAp.KM3_Point==0x00)&&
                    (gAp.KM1_Coil==0)&&(gAp.KM2_Coil==0x00)&&(gAp.KM3_Coil==0x00)&&(gAp.Power_Statue==0))    	
                        {   RS485Dat_LED7_OFF();    } //主一故障指示灯
                else    {   RS485Dat_LED7_ON();     }
            }
            else if(gAp.Statue==Slow)    //低速状态
            {
                if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0))
                {
                    RS485Dat_LED12_OFF();RS485Dat_LED13_ON();RS485Dat_LED14_OFF();
                }
            }
            else if(gAp.Statue==Fast)    //高速状态
            {
                if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1))
                {
                    RS485Dat_LED12_OFF();RS485Dat_LED13_OFF();RS485Dat_LED14_ON();
                }
            }    
            if(gBp.Statue==Stop)    //停止状态
            {
                if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_ON();RS485Dat_LED19_OFF();RS485Dat_LED20_OFF();
                }
                if( (gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)&&
                    (gBp.KM1_Coil==0)&&(gBp.KM2_Coil==0)&&(gBp.KM3_Coil==0)&&(gBp.Power_Statue==0) )    	
                        {   RS485Dat_LED8_OFF();    } //主二故障指示灯
                else    {   RS485Dat_LED8_ON();     }
            }
            else if(gBp.Statue==Slow)    //低速状态
            {
                if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0))
                {
                    RS485Dat_LED18_OFF();RS485Dat_LED19_ON();RS485Dat_LED20_OFF();
                }               
            }
            else if(gBp.Statue==Fast)    //高速状态
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

//无源/有源继电器输出控制
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
//A泵运行检测
uint8_t KM_ApRunningCheck(uint8_t _StartMode)
{
	uint8_t uTemp=0;
	switch (_StartMode)
	{
		case HHFJ:	//风机
		case HH33:	//星三角
			if(gAp.Statue==Slow)    	//A泵低速
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A泵, KM1触点,正常.\r\n");
                        else                    printf("     A泵, KM1触点,异常.\r\n");
                        if(gAp.KM2_Point==1)    printf("     A泵, KM2触点,正常.\r\n");
                        else                    printf("     A泵, KM2触点,异常.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A泵, KM3触点,正常.\r\n");
                        else                    printf("     A泵, KM3触点,异常.\r\n");
                        if(gAp.Work_Check==0)   printf("     A泵,工作检测,正常.\r\n");
                        else                    printf("     A泵,工作检测,异常.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A泵,过载检测,正常.\r\n");
                        else                    printf("     A泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			else if(gAp.Statue==Fast)	//A泵高速
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A泵, KM1触点,正常.\r\n");
                        else                    printf("     A泵, KM1触点,异常.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A泵, KM2触点,正常.\r\n");
                        else                    printf("     A泵, KM2触点,异常.\r\n");
                        if(gAp.KM3_Point==1)    printf("     A泵, KM3触点,正常.\r\n");
                        else                    printf("     A泵, KM3触点,异常.\r\n");
                        if(gAp.Work_Check==0)   printf("     A泵,工作检测,正常.\r\n");
                        else                    printf("     A泵,工作检测,异常.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A泵,过载检测,正常.\r\n");
                        else                    printf("     A泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		case HH11:	//直接
			if(gAp.Statue==Slow)    //A泵低速
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A泵, KM1触点,正常.\r\n");
                        else                    printf("     A泵, KM1触点,异常.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A泵, KM2触点,正常.\r\n");
                        else                    printf("     A泵, KM2触点,异常.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A泵, KM3触点,正常.\r\n");
                        else                    printf("     A泵, KM3触点,异常.\r\n");
                        if(gAp.Work_Check==0)   printf("     A泵,工作检测,正常.\r\n");
                        else                    printf("     A泵,工作检测,异常.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A泵,过载检测,正常.\r\n");
                        else                    printf("     A泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		case HH44:	//自耦
			if(gAp.Statue==Slow)    	//A泵低速
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==1)    printf("     A泵, KM1触点,正常.\r\n");
                        else                    printf("     A泵, KM1触点,异常.\r\n");
                        if(gAp.KM2_Point==1)    printf("     A泵, KM2触点,正常.\r\n");
                        else                    printf("     A泵, KM2触点,异常.\r\n");
                        if(gAp.KM3_Point==0)    printf("     A泵, KM3触点,正常.\r\n");
                        else                    printf("     A泵, KM3触点,异常.\r\n");
                        if(gAp.Work_Check==0)   printf("     A泵,工作检测,正常.\r\n");
                        else                    printf("     A泵,工作检测,异常.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A泵,过载检测,正常.\r\n");
                        else                    printf("     A泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			else if(gAp.Statue==Fast)	//A泵高速
			{
				if(gAp.DelayCheck_Count==0)
				{
					if((gAp.KM1_Point==0)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
						&&(gAp.Work_Check==0)&&(gAp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gAp.KM1_Point==0)    printf("     A泵, KM1触点,正常.\r\n");
                        else                    printf("     A泵, KM1触点,异常.\r\n");
                        if(gAp.KM2_Point==0)    printf("     A泵, KM2触点,正常.\r\n");
                        else                    printf("     A泵, KM2触点,异常.\r\n");
                        if(gAp.KM3_Point==1)    printf("     A泵, KM3触点,正常.\r\n");
                        else                    printf("     A泵, KM3触点,异常.\r\n");
                        if(gAp.Work_Check==0)   printf("     A泵,工作检测,正常.\r\n");
                        else                    printf("     A泵,工作检测,异常.\r\n");
                        if(gAp.Phase_Check==0)  printf("     A泵,过载检测,正常.\r\n");
                        else                    printf("     A泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		default:
			break;
	}
    return uTemp;    //检测结果
}

//B泵运行检测
uint8_t KM_BpRunningCheck(uint8_t _StartMode)
{
	uint8_t uTemp=0;
	switch (_StartMode)
	{
		case HHFJ:	//风机
		case HH33:	//星三角
			if(gBp.Statue==Slow)    	//B泵低速
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B泵, KM1触点,正常.\r\n");
                        else                    printf("     B泵, KM1触点,异常.\r\n");
                        if(gBp.KM2_Point==1)    printf("     B泵, KM2触点,正常.\r\n");
                        else                    printf("     B泵, KM2触点,异常.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B泵, KM3触点,正常.\r\n");
                        else                    printf("     B泵, KM3触点,异常.\r\n");
                        if(gBp.Work_Check==0)   printf("     B泵,工作检测,正常.\r\n");
                        else                    printf("     B泵,工作检测,异常.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B泵,过载检测,正常.\r\n");
                        else                    printf("     B泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			else if(gBp.Statue==Fast)	//B泵高速
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B泵, KM1触点,正常.\r\n");
                        else                    printf("     B泵, KM1触点,异常.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B泵, KM2触点,正常.\r\n");
                        else                    printf("     B泵, KM2触点,异常.\r\n");
                        if(gBp.KM3_Point==1)    printf("     B泵, KM3触点,正常.\r\n");
                        else                    printf("     B泵, KM3触点,异常.\r\n");
                        if(gBp.Work_Check==0)   printf("     B泵,工作检测,正常.\r\n");
                        else                    printf("     B泵,工作检测,异常.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B泵,过载检测,正常.\r\n");
                        else                    printf("     B泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		case HH11:	//直接
			if(gBp.Statue==Slow)    //B泵低速
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B泵, KM1触点,正常.\r\n");
                        else                    printf("     B泵, KM1触点,异常.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B泵, KM2触点,正常.\r\n");
                        else                    printf("     B泵, KM2触点,异常.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B泵, KM3触点,正常.\r\n");
                        else                    printf("     B泵, KM3触点,异常.\r\n");
                        if(gBp.Work_Check==0)   printf("     B泵,工作检测,正常.\r\n");
                        else                    printf("     B泵,工作检测,异常.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B泵,过载检测,正常.\r\n");
                        else                    printf("     B泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		case HH44:	//自耦
			if(gBp.Statue==Slow)    	//B泵低速
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==1)    printf("     B泵, KM1触点,正常.\r\n");
                        else                    printf("     B泵, KM1触点,异常.\r\n");
                        if(gBp.KM2_Point==1)    printf("     B泵, KM2触点,正常.\r\n");
                        else                    printf("     B泵, KM2触点,异常.\r\n");
                        if(gBp.KM3_Point==0)    printf("     B泵, KM3触点,正常.\r\n");
                        else                    printf("     B泵, KM3触点,异常.\r\n");
                        if(gBp.Work_Check==0)   printf("     B泵,工作检测,正常.\r\n");
                        else                    printf("     B泵,工作检测,异常.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B泵,过载检测,正常.\r\n");
                        else                    printf("     B泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			else if(gBp.Statue==Fast)	//B泵高速
			{
				if(gBp.DelayCheck_Count==0)
				{
					if((gBp.KM1_Point==0)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
						&&(gBp.Work_Check==0)&&(gBp.Phase_Check==0));    //正常运行  
					else
                    {
                        printf("\r\n");
                        if(gBp.KM1_Point==0)    printf("     B泵, KM1触点,正常.\r\n");
                        else                    printf("     B泵, KM1触点,异常.\r\n");
                        if(gBp.KM2_Point==0)    printf("     B泵, KM2触点,正常.\r\n");
                        else                    printf("     B泵, KM2触点,异常.\r\n");
                        if(gBp.KM3_Point==1)    printf("     B泵, KM3触点,正常.\r\n");
                        else                    printf("     B泵, KM3触点,异常.\r\n");
                        if(gBp.Work_Check==0)   printf("     B泵,工作检测,正常.\r\n");
                        else                    printf("     B泵,工作检测,异常.\r\n");
                        if(gBp.Phase_Check==0)  printf("     B泵,过载检测,正常.\r\n");
                        else                    printf("     B泵,过载检测,异常.\r\n");
                        uTemp = 0x01;    //异常运行
                    }
				}
			}
			break;
		default:
			break;
	}
    return uTemp;    //检测结果
}


uint8_t Menu=Menu_Idle,g_ShowMode=Show_U;
//设置参数 如电压电流上下限等
void SetParam(void)
{
	uint8_t KeyDat=0;
    KeyDat = bsp_GetKey();
    if(KeyDat!=KEY_NONE)	//按键检测及数据处理
    {
        switch (KeyDat)
        {
            case KEY_NONE:	//无按键按下
                break;
            case KEY_1_DOWN:							
                if(Menu==Menu_StartMode)
                {
                    Menu = Menu_Ua;       //电压上限
                }
                else if(Menu!=Menu_Idle)
                {
                    Menu++;	//菜单模式+1
                }
                break;
            case KEY_1_UP:							
                if(Menu==Menu_Idle)    //空闲模式
                {
                    g_ShowMode = (g_ShowMode+1)%4;  //0-1-2-3-0
                }
                break;
            case KEY_1_LONG:							
                if(Menu==Menu_Idle)
                {
                    printf("进入设置菜单.\r\n------------------------\r\n");
                    Menu = Menu_Ua;       //电压上限
                }
                break;
            case KEY_2_DOWN:
                gParamDat[Menu]++;
                if(Menu==Menu_StartMode)
                {   if(gParamDat[Menu]>3)   gParamDat[Menu] = 3;    }
                else if(Menu==Menu_A)    //电流最大值600A
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
                    printf("   电压(Limit:%2d-%2d).\r\n",gParamDat[Menu_Ub],gParamDat[Menu_Ua]);
                    printf("   电流(Limit:%2d-%2d).  基数:%2d.  变比:%2d.\r\n",gParamDat[Menu_Ab],gParamDat[Menu_Aa],gParamDat[Menu_A],gParamDat[Menu_Ac]);
                    printf("   温度(Limit:%2d-%2d).  湿度(Limit:%2d-%2d).\r\n",gParamDat[Menu_Cb],gParamDat[Menu_Ca],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                    printf("   压力(Limit:%2d-%2d).  流量(Limit:%2d-%2d).\r\n",gParamDat[Menu_Pb],gParamDat[Menu_Pa],gParamDat[Menu_Fb],gParamDat[Menu_Fa]);
                    if(gParamDat[Menu_StartMode]==0x00)        {   printf("   启动模式：HH33 星三角.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x01)   {   printf("   启动模式：HH11 直接.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x02)   {   printf("   启动模式：HH44 自耦.\r\n");}
                    else if(gParamDat[Menu_StartMode]==0x03)   {   printf("   启动模式：HHFJ 风机.\r\n");}
                    Write_Flash_Dat();  //写Flash数据
                    Menu = Menu_Idle;     //保存参数退出
                    printf("保存,退出菜单.\r\n------------------------\r\n");
                }
                break;
            default:
                break;
        }
        g_KeyNoneCount=40;    //无按键按下，退出参数设置
    }
    if((g_KeyNoneCount<5)&&(Menu!=Menu_Idle))
    {   
        g_KeyNoneCount=0;
        Menu=Menu_Idle;     //不保存参数退出
        printf("不保存,退出菜单.\r\n");
    }
}


//_StartMode:启动模式，星三角、直接、自耦、风机
//_RunDat:运行方式，停止/低速/高速
void KM_RunMode(uint8_t _RunMode,uint8_t _StartMode,uint8_t _RunDat)
{
	switch (_StartMode)
	{
		case HH33:	//星三角
			if((_RunDat&0x0F)==0x01)		//A泵停止        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A泵低速
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A泵高速
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B泵停止
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B泵低速
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B泵高速 
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
			}                 
			break;
		case HH11:	//直接
			if((_RunDat&0x0F)==0x01)		//A泵停止        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A泵KM1
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			if((_RunDat&0xF0)==0x10)   		//B泵停止
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B泵KM1
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			break;
		case HH44:	//自耦
			if((_RunDat&0x0F)==0x01)		//A泵停止        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A泵低速
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A泵高速
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B泵停止
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B泵低速
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B泵高速 
			{   
				gBp.Statue=Fast; 
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;gBp.SlowTimer_Count=250;}
				KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
			}                 
			break;
		case HHFJ:	//风机
			if((_RunDat&0x0F)==0x01)		//A泵停止        
			{   
				gAp.Statue=Stop;
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;	gAp.SlowTimer_Count=0;}
				KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x02)   //A泵低速
			{   
				gAp.Statue=Slow;      
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;if(_RunMode==0)gAp.SlowTimer_Count=250;else gAp.SlowTimer_Count=80;}
				KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
			}  
			else if((_RunDat&0x0F)==0x04)   //A泵高速
			{   
				gAp.Statue=Fast; 
				if(gAp.Statue!=gAp.OldStatue)	
				{	gAp.OldStatue=gAp.Statue;gAp.DelayCheck_Count = 30;gAp.SlowTimer_Count=250;}
				KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
			}  
			if((_RunDat&0xF0)==0x10)   		//B泵停止
			{   
				gBp.Statue=Stop;     
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;	gBp.SlowTimer_Count=0;}
				KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x20)   //B泵低速
			{   
				gBp.Statue=Slow;      
				if(gBp.Statue!=gBp.OldStatue)	
				{	gBp.OldStatue=gBp.Statue;gBp.DelayCheck_Count = 30;if(_RunMode==0)gBp.SlowTimer_Count=250;else gBp.SlowTimer_Count=80;}
				KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
			}  
			else if((_RunDat&0xF0)==0x40)   //B泵高速 
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

//_RunMode:自动/手动
//_StartMode:启动模式，星三角、直接、自耦、风机
//_RunDat:运行方式，停止/低速/高速
//uint8_t g_ChangeFlag=0;//互投标志
uint8_t KMStart_Pro(uint8_t _RunMode,uint8_t _RunDat,uint8_t _StartMode)
{
	if(g_ChangeFlag==0xAA)//互投标志
	{
		if(_RunMode==1)	{_RunMode=2;if((_RunDat==0x12)||(_RunDat==0x11))_RunDat=0x21;if(_RunDat==0x14)_RunDat=0x41; }
		else			{_RunMode=1;if((_RunDat==0x21)||(_RunDat==0x11))_RunDat=0x12;if(_RunDat==0x41)_RunDat=0x14; }
	}

	KM_RunMode(_RunMode,_StartMode,_RunDat);	//_StartMode:启动模式;_RunDat:运行方式，停止/低速/高速
	if(_RunMode==0)	//手动
	{
		if((_RunDat&0x0F)!=0x01)	//A泵动作中
		{
			if(0x00!=KM_ApRunningCheck(_StartMode))//A泵异常，进入停止
			{
				_RunDat = (_RunDat&0xF0)|0x01;
				printf("A泵异常，停止.0x%02X.\r\n",_RunDat);	
                gAp.ErrorFlag=1;
			}
			else
			{
				if(gAp.SlowTimer_Count==0)	_RunDat = (_RunDat&0xF0)|0x04;	//转入高速
			}
		}
		if((_RunDat&0xF0)!=0x10)	//B泵动作中
		{
			if(0x00!=KM_BpRunningCheck(_StartMode))//B泵异常，进入停止
			{
				_RunDat = (_RunDat&0x0F)|0x10;
				printf("B泵异常，停止.0x%02X.\r\n",_RunDat);				
                gBp.ErrorFlag=1;
			}
			else
			{
				if(gBp.SlowTimer_Count==0)	_RunDat = (_RunDat&0x0F)|0x40;	//转入高速
			}
		}		
	}
	else if(_RunMode==1)	//自动 主一备二
	{
		if((_RunDat&0x0F)!=0x01)	//A泵动作中
		{
			if(0x00!=KM_ApRunningCheck(_StartMode))//A泵异常，进入停止
			{
				_RunDat = (_RunDat&0xF0)|0x01;
				printf("A泵异常，停止.0x%02X.\r\n",_RunDat);				
                gAp.ErrorFlag=1;
				if(g_ChangeFlag==0x00)	{	g_ChangeFlag=0xAA;	printf("互投切换.  ");}//互投标志
				else					{	g_ChangeFlag=0xBB;	printf("已互投，错误退出!\r\n");}//互投标志
			}
			else
			{
				if(gAp.SlowTimer_Count==0)	_RunDat = (_RunDat&0xF0)|0x04;	//转入高速
			}
		}
	}
	else if(_RunMode==2)	//自动 主二备一
	{
		if((_RunDat&0xF0)!=0x10)	//B泵动作中
		{
			if(0x00!=KM_BpRunningCheck(_StartMode))//B泵异常，进入停止
			{
				_RunDat = (_RunDat&0x0F)|0x10;
                gBp.ErrorFlag=1;
				printf("B泵异常，停止.0x%02X.\r\n",_RunDat);				
				if(g_ChangeFlag==0x00)	{	g_ChangeFlag=0xAA;	printf("互投切换.  ");}//互投标志
				else					{	g_ChangeFlag=0xBB;	printf("已互投，错误退出!\r\n");}//互投标志
			}
			else
			{
				if(gBp.SlowTimer_Count==0)	_RunDat = (_RunDat&0x0F)|0x40;	//转入高速
			}
		}		
	}
	return _RunDat;
}




