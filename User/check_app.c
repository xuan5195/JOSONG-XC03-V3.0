#include "stm32f10x.h"		
#include "delay.h"

#define GLOBAL_APP

#include "check_app.h"		
#include "bsp.h"

extern uint8_t g_KeyNoneCount;
extern uint8_t g_ChangeFlag;

void ReadInputDat(void)
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
    if((gAp.Phase_Check==0)||(gBp.Phase_Check==0))  {   RS485Dat_LED9_ON();     } //断相/过载 指示灯
    else                                            {   RS485Dat_LED9_OFF();    }

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
    else if(gAp.Statue==HighSpeed)    //高速状态
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
//        if(gBp.DelayCheck_Count==0) //延时检测时间到，进入检测
//        {
//            if(gBp.Work_Check==1)   //工作检测失败，转为
//        }                
    }
    else if(gBp.Statue==HighSpeed)    //高速状态
    {
        if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1))
        {
            RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
        }
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

void KMAutoRUN(uint8_t _Mode,uint8_t _Step)//自动启动控制
{
    if(g_ChangeFlag==0xAA)  //进入互投
    {
        if(_Mode==1)    _Mode=2;
        else            _Mode=1;
    }
    else if(g_ChangeFlag==0xBB)  //互投过一次，直接退出
    {
        _Mode = 0;  //改为0，不进行启动动作
    }
    if(_Mode==1)    //主一备二
    {
        if(_Step==0)
        {
            printf("主一备二，停止.\r\n");
            gAp.Statue = Stop;
            KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //停止
            KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //停止
        }
        else if(_Step==1)
        {
            printf("主一备二，低速.\r\n");
            gAp.Statue = Slow;  gAp.DelayCheck_Count = 30;
            KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //低速运行
        }
        else if(_Step==2)
        {
            printf("主一备二，高速.\r\n");
            gAp.Statue = HighSpeed; gAp.DelayCheck_Count = 30;
            KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMON_Show(AKM3RUN);    //高速运行
        }
    }
    else if(_Mode==2)    //主二备一
    {
        if(_Step==0)
        {
            printf("主二备一，停止.\r\n");
            gBp.Statue = Stop;
            KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //停止
            KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //停止
        }
        else if(_Step==1)
        {
            printf("主二备一，低速.\r\n");
            gBp.Statue = Slow; gBp.DelayCheck_Count = 30;
            KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //低速运行
        }
        else if(_Step==2)
        {
            printf("主二备一，高速.\r\n");
            gBp.Statue = HighSpeed; gBp.DelayCheck_Count = 30;
            KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMON_Show(BKM3RUN);    //高速运行
        }
    }
}

//A泵运行检测
uint8_t KM_ApRunningCheck(void)
{
    if(gAp.Statue==Slow)    //A泵低速
    {
        if(gAp.DelayCheck_Count==0)
        {
            if((gAp.KM1_Point==1)&&(gAp.KM2_Point==1)&&(gAp.KM3_Point==0)\
                &&(gAp.Work_Check==0)&&(gAp.Phase_Check==1))  
                    return 0x00;    //正常运行
            else    return 0x01;    //异常运行
        }
    }
    else if(gAp.Statue==HighSpeed)    //A泵高速
    {
        if(gAp.DelayCheck_Count==0)
        {
            if((gAp.KM1_Point==1)&&(gAp.KM2_Point==0)&&(gAp.KM3_Point==1)\
                &&(gAp.Work_Check==0)&&(gAp.Phase_Check==1))  
                    return 0x00;
            else    return 0x01;
        }
    }
    return 0x02;    //未检测
}

//B泵运行检测
uint8_t KM_BpRunningCheck(void)
{
    if(gBp.Statue==Slow)    //B泵低速
    {
        if(gBp.DelayCheck_Count==0)
        {
            if((gBp.KM1_Point==1)&&(gBp.KM2_Point==1)&&(gBp.KM3_Point==0)\
                &&(gBp.Work_Check==0)&&(gBp.Phase_Check==1))  
                    return 0x00;    //正常运行
            else    return 0x01;    //异常运行
        }
    }
    else if(gBp.Statue==HighSpeed)    //B泵高速
    {
        if(gBp.DelayCheck_Count==0)
        {
            if((gBp.KM1_Point==1)&&(gBp.KM2_Point==0)&&(gBp.KM3_Point==1)\
                &&(gBp.Work_Check==0)&&(gBp.Phase_Check==1))  
                    return 0x00;
            else    return 0x01;
        }
    }
    return 0x02;    //未检测
}

//自动模式下，A/B泵检测
uint8_t KM_RunningAutoCheck(uint8_t _Mode)
{
    if(g_ChangeFlag==0xAA)  //进入互投
    {
        if(_Mode==1)    _Mode=2;
        else            _Mode=1;
    }
    else if(g_ChangeFlag==0xBB)  //互投过一次，直接退出
    {
        _Mode = 0;  //改为0，不进行启动动作
    }
    if(_Mode==1)    //主一备二模式
    {
        if(0x01==KM_ApRunningCheck())   
        {
            printf("A泵异常.\r\n");
            return 0x01;
        }//A泵异常，进入停止
        return 0x00; 
    }
    else if(_Mode==2)    //主二备一模式
    {
        if(0x01==KM_BpRunningCheck())   
        {
            printf("B泵异常.\r\n");
            return 0x01;
        }//B泵异常，进入停止
        return 0x00; 
    }
    return 0x00; 
}

uint8_t Menu=Menu_Idle;
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
                if(Menu==Menu_Fb)
                {
                    Menu = Menu_Ua;       //电压上限
                }
                else if(Menu!=Menu_Idle)
                {
                    Menu++;	//菜单模式+1
                }
                break;
            case KEY_1_LONG:							
                if(Menu==Menu_Idle)
                {
                    printf("进入设置菜单.\r\n");
                    Menu = Menu_Ua;       //电压上限
                }
                break;
            case KEY_2_DOWN:
                gParamDat[Menu]++;
                if(Menu==Menu_A)    //电流最大值600A
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
                    printf("电压上限:%2d.电压下限:%2d.\r\n",gParamDat[Menu_Ua],gParamDat[Menu_Ub]);
                    printf("电流基数:%3d.电流上限:%2d.电流下限:%2d.电流变比:%2d.\r\n",gParamDat[Menu_A],gParamDat[Menu_Aa],gParamDat[Menu_Ab],gParamDat[Menu_Ac]);
                    printf("温度上限:%2d.温度下限:%2d.湿度上限:%2d.湿度下限:%2d.\r\n",gParamDat[Menu_Ca],gParamDat[Menu_Cb],gParamDat[Menu_Ha],gParamDat[Menu_Hb]);
                    printf("压力上限:%2d.压力下限:%2d.流量上限:%2d.流量下限:%2d.\r\n",gParamDat[Menu_Pa],gParamDat[Menu_Pb],gParamDat[Menu_Fa],gParamDat[Menu_Fb]);
                    Write_Flash_Dat();  //写Flash数据
                    Menu = Menu_Idle;     //保存参数退出
                    printf("保存,退出菜单.\r\n");
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

