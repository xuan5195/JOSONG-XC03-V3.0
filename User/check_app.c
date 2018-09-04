#include "stm32f10x.h"		
#include "check_app.h"		
#include "delay.h"
#include "bsp.h"

void ReadInputDat(void)
{
    uint8_t ApPowerDat=0,BpPowerDat=0;
    uint8_t ApKMDat1=0,BpKMDat1=0,ApKMDat2=0,BpKMDat2=0;
    if((Read_Optocoupler(RDKMA1)==1)||(Read_Optocoupler(RDKMA2)==1) || (Read_Optocoupler(RDKMA3)==1))
    {   ApKMDat1=0xAA;           }//A�ýӴ��������쳣
    else
    {   ApKMDat1=0x00;           }//A�ýӴ�����������
	if((Read_Optocoupler(RDKMB1)==1)||(Read_Optocoupler(RDKMB2)==1) || (Read_Optocoupler(RDKMB3)==1))
    {   BpKMDat1=0xAA;           }//B�ýӴ��������쳣
    else
    {   BpKMDat1=0x00;           }//B�ýӴ�����������
    ApPowerDat = Read_InputDevDat(2);//A�ö����磻0->�쳣;1->���� 
    BpPowerDat = Read_InputDevDat(3);//B�ö����磻0->�쳣;1->���� 

    if((Read_Input_KPDevDat(2)==1)||(Read_Input_KPDevDat(1)==1) || (Read_Input_KPDevDat(0)==1))
    {   ApKMDat2=0xAA;           }//A��KMǰ�����ѹ�ź�
    else
    {   ApKMDat2=0x00;           }
    if((Read_Input_KPDevDat(3)==1)||(Read_Input_KPDevDat(4)==1) || (Read_Input_KPDevDat(5)==1))
    {   BpKMDat2=0xAA;           }//B��KMǰ�����ѹ�ź�
    else
    {   BpKMDat2=0x00;           }
    
    if((ApPowerDat==0)&&(ApKMDat1==0x00)&&(ApKMDat2==0x00))    	{   RS485Dat_LED7_OFF();    } //��һ����ָʾ��
    else                                    					{   RS485Dat_LED7_ON();     }
    if((BpPowerDat==0)&&(BpKMDat1==0x00)&&(BpKMDat2==0x00))    	{   RS485Dat_LED8_OFF();    } //��������ָʾ��        
    else                                    					{   RS485Dat_LED8_ON();     }		

    if((ApPowerDat==0)&&(BpPowerDat==0))    {   RS485Dat_LED4_ON();     } //������ָʾ��  
    else                                    {   RS485Dat_LED4_OFF();    } 
    
    if(Read_Optocoupler(4))//ȱˮ���,������ˮ
    {
        RS485Dat_LED10_OFF();	//ȱˮ    ָʾ��
    }        
    else				   //ȱˮ���,�쳣ȱˮ
    {
        RS485Dat_LED10_ON();	//ȱˮ    ָʾ��    
    }
//    if(Read_Optocoupler(5))//���ڽ�ˮ��⣬������δ��ˮ
//    else				   //���ڽ�ˮ��⣬�쳣�������Զ��ֶ���������ˮ��

}



