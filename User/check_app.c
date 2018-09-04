#include "stm32f10x.h"		
#include "check_app.h"		
#include "delay.h"
#include "bsp.h"

void ReadInputDat(uint8_t _ApDat,uint8_t _BpDat)
{
    uint8_t ApPowerDat=0,BpPowerDat=0;
    uint8_t ApKMDat1=0,BpKMDat1=0,ApKMDat2=0,BpKMDat2=0;
    if(((Read_Optocoupler(RDKMA1)==1)||(Read_Optocoupler(RDKMA2)==1) || (Read_Optocoupler(RDKMA3)==1))&&(_ApDat==0))
    {   ApKMDat1=0xAA;           }//A泵接触器反馈异常
    else
    {   ApKMDat1=0x00;           }//A泵接触器反馈正常
	if(((Read_Optocoupler(RDKMB1)==1)||(Read_Optocoupler(RDKMB2)==1) || (Read_Optocoupler(RDKMB3)==1))&&(_BpDat==0))
    {   BpKMDat1=0xAA;           }//B泵接触器反馈异常
    else
    {   BpKMDat1=0x00;           }//B泵接触器反馈正常
    ApPowerDat = Read_InputDevDat(2);//A泵动力电；0->异常;1->正常 
    BpPowerDat = Read_InputDevDat(3);//B泵动力电；0->异常;1->正常 

    if(((Read_Input_KPDevDat(2)==1)||(Read_Input_KPDevDat(1)==1) || (Read_Input_KPDevDat(0)==1))&&(_ApDat==0))
    {   ApKMDat2=0xAA;           }//A泵KM前导点电压信号
    else
    {   ApKMDat2=0x00;           }
    if(((Read_Input_KPDevDat(3)==1)||(Read_Input_KPDevDat(4)==1) || (Read_Input_KPDevDat(5)==1))&&(_BpDat==0))
    {   BpKMDat2=0xAA;           }//B泵KM前导点电压信号
    else
    {   BpKMDat2=0x00;           }
    
    if((ApPowerDat==0)&&(ApKMDat1==0x00)&&(ApKMDat2==0x00))     {   RS485Dat_LED7_OFF();    } //主一故障指示灯
    else                                    				    {   RS485Dat_LED7_ON();     }
    if((BpPowerDat==0)&&(BpKMDat1==0x00)&&(BpKMDat2==0x00))     {   RS485Dat_LED8_OFF();    } //主二故障指示灯        
    else                                    				    {   RS485Dat_LED8_ON();     }		

    if((ApPowerDat==0)&&(BpPowerDat==0))    {   RS485Dat_LED4_ON();     } //动力电指示灯  
    else                                    {   RS485Dat_LED4_OFF();    } 

    if(_ApDat!=0)   //Ap泵运行中
    {
        if(((Read_Optocoupler(RDKMA1)==1)&&(Read_Optocoupler(RDKMA2)==1)&&(Read_Optocoupler(RDKMA3)==0)))   //KM常开触点反馈 低速运行中
        {
            RS485Dat_LED12_OFF();RS485Dat_LED13_ON();RS485Dat_LED14_OFF();
        }
        else if(((Read_Optocoupler(RDKMA1)==1)&&(Read_Optocoupler(RDKMA2)==0)&&(Read_Optocoupler(RDKMA3)==1)))   //KM常开触点反馈 高速运行中
        {
            RS485Dat_LED12_OFF();RS485Dat_LED13_OFF();RS485Dat_LED14_ON();
        }
    }
    else
    {
        RS485Dat_LED12_ON();RS485Dat_LED13_OFF();RS485Dat_LED14_OFF();
    }
    if(_BpDat!=0)   //Bp泵运行中
    {
        if(((Read_Optocoupler(RDKMB1)==1)&&(Read_Optocoupler(RDKMB2)==1)&&(Read_Optocoupler(RDKMB3)==0)))   //KM常开触点反馈 低速运行中
        {
            RS485Dat_LED18_OFF();RS485Dat_LED19_ON();RS485Dat_LED20_OFF();
        }
        else if(((Read_Optocoupler(RDKMB1)==1)&&(Read_Optocoupler(RDKMB2)==0)&&(Read_Optocoupler(RDKMB3)==1)))   //KM常开触点反馈 高速运行中
        {
            RS485Dat_LED18_OFF();RS485Dat_LED19_OFF();RS485Dat_LED20_ON();
        }
    }
    else
    {
        RS485Dat_LED18_ON();RS485Dat_LED19_OFF();RS485Dat_LED20_OFF();
    }
    
    if(Read_Optocoupler(4))//缺水检测,正常有水
    {
        RS485Dat_LED10_OFF();	//缺水    指示灯
    }        
    else				   //缺水检测,异常缺水
    {
        RS485Dat_LED10_ON();	//缺水    指示灯    
    }
//    if(Read_Optocoupler(5))//箱内进水检测，正常，未进水
//    else				   //箱内进水检测，异常，主机自动手动不能启动水泵

}

void KMAutoRUN(uint8_t _Mode,uint8_t _Step)//自动启动控制
{
    if(_Mode==1)    //主一备二
    {
        if(_Step==0)
        {
            printf("主一备二，停止.\r\n");
            KMOFF_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //停止
        }
        else if(_Step==1)
        {
            printf("主一备二，低速.\r\n");
            KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);    //低速运行
        }
        else if(_Step==2)
        {
            printf("主一备二，高速.\r\n");
            KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN); KMON_Show(AKM3RUN);    //高速运行
        }
    }
    else if(_Mode==2)    //主二备一
    {
        if(_Step==0)
        {
            printf("主二备一，停止.\r\n");
            KMOFF_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //停止运行
        }
        else if(_Step==1)
        {
            printf("主二备一，低速.\r\n");
            KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);    //低速运行
        }
        else if(_Step==2)
        {
            printf("主二备一，高速.\r\n");
            KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN); KMON_Show(BKM3RUN);    //高速运行
        }
    }
}

