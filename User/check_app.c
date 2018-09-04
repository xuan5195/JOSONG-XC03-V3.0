#include "stm32f10x.h"		
#include "check_app.h"		
#include "delay.h"
#include "bsp.h"

void ReadInputDat(void)
{
    uint8_t ApPowerDat=0,BpPowerDat=0;
    uint8_t ApKMDat1=0,BpKMDat1=0,ApKMDat2=0,BpKMDat2=0;
    if((Read_Optocoupler(RDKMA1)==1)||(Read_Optocoupler(RDKMA2)==1) || (Read_Optocoupler(RDKMA3)==1))
    {   ApKMDat1=0xAA;           }//A泵接触器反馈异常
    else
    {   ApKMDat1=0x00;           }//A泵接触器反馈正常
	if((Read_Optocoupler(RDKMB1)==1)||(Read_Optocoupler(RDKMB2)==1) || (Read_Optocoupler(RDKMB3)==1))
    {   BpKMDat1=0xAA;           }//B泵接触器反馈异常
    else
    {   BpKMDat1=0x00;           }//B泵接触器反馈正常
    ApPowerDat = Read_InputDevDat(2);//A泵动力电；0->异常;1->正常 
    BpPowerDat = Read_InputDevDat(3);//B泵动力电；0->异常;1->正常 

    if((Read_Input_KPDevDat(2)==1)||(Read_Input_KPDevDat(1)==1) || (Read_Input_KPDevDat(0)==1))
    {   ApKMDat2=0xAA;           }//A泵KM前导点电压信号
    else
    {   ApKMDat2=0x00;           }
    if((Read_Input_KPDevDat(3)==1)||(Read_Input_KPDevDat(4)==1) || (Read_Input_KPDevDat(5)==1))
    {   BpKMDat2=0xAA;           }//B泵KM前导点电压信号
    else
    {   BpKMDat2=0x00;           }
    
    if((ApPowerDat==0)&&(ApKMDat1==0x00)&&(ApKMDat2==0x00))    	{   RS485Dat_LED7_OFF();    } //主一故障指示灯
    else                                    					{   RS485Dat_LED7_ON();     }
    if((BpPowerDat==0)&&(BpKMDat1==0x00)&&(BpKMDat2==0x00))    	{   RS485Dat_LED8_OFF();    } //主二故障指示灯        
    else                                    					{   RS485Dat_LED8_ON();     }		

    if((ApPowerDat==0)&&(BpPowerDat==0))    {   RS485Dat_LED4_ON();     } //动力电指示灯  
    else                                    {   RS485Dat_LED4_OFF();    } 
    
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



