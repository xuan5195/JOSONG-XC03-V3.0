#include "stm32f10x.h"		
#include "bsp_cd4067.h"		
#include "bsp.h"

void BspInput_CD4067_Delay_us(u16 z) 
{ 
    delay_us(z);
}
void BspInput_CD4067_Delay_ms(u16 z) 
{ 
    delay_ms(z);
}

void BspInput_CD4067_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void SetInput_CD4067Switch(uint8_t uSetDat)
{
    switch (uSetDat)
    {
        case 0:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 1:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 2:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 3:
            Iutput_CD4067A_High();	Iutput_CD4067B_High(); 	Iutput_CD4067C_Low();	Iutput_CD4067D_Low();
            break;
        case 4:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 5:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 6:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 7:
            Iutput_CD4067A_High();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_Low();
            break;
        case 8:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 9:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 10:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 11:
            Iutput_CD4067A_High();	Iutput_CD4067B_High(); 	Iutput_CD4067C_Low();	Iutput_CD4067D_High();
            break;
        case 12:
            Iutput_CD4067A_Low();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 13:
            Iutput_CD4067A_High();	Iutput_CD4067B_Low();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 14:
            Iutput_CD4067A_Low();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
        case 15:
            Iutput_CD4067A_High();	Iutput_CD4067B_High();	Iutput_CD4067C_High();	Iutput_CD4067D_High();
            break;
       default:
            break;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Read_InputDevDat
*	����˵��: �������Ƿ�ͨ
*	�� 	  ��: ѡ����ͨ��
*	�� �� ֵ: 0�����ͨ��1���ⲿ�޴����ź�
*	�� 	  ע��
*			 0~3�������Ǽ�⽻��220V��һ�������ڻ���һ���ʱ������ͨ�������Ҫ���⴦��
*
*		�㷨˵��������ǰ����220v��ѹ����˹�������˲��β��ߵ�ƽ���ߵ͵�ƽ�����Ƿ�������Ҫ�����˴���,
*				  220V��ѹ��Ƶ��Ϊ50hz��20ms��һ��������ֻ��5msʱ����Ե�ͨ�������ʱ�䲻��ͨ��
*				  �����Ҫ������β���ͨʱ��
*********************************************************************************************************
*/
u8 Count[4]={10,10,10,10},OldState[4]={0};
u8 Read_InputDevDat(uint8_t _no)  
{
	if(_no<4) 
	{
        SetInput_CD4067Switch(_no);
        BspInput_CD4067_Delay_us(20);
        if(Read_InputData()==0)
        {
            Count[_no] = 10; OldState[_no] = 0;
            return OldState[_no];
        }
		else   //�ߵ�ƽ����Ҫ��һ������,�����ǵ͵�ƽ
		{
            if(Count[_no]>0)
            {
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(3);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                BspInput_CD4067_Delay_ms(5);
                if(Read_InputData()==0) {   Count[_no] = 10;OldState[_no] =0;return OldState[_no]; }
                else                    {   Count[_no]--;    return OldState[_no];            }
            }
            else
            {
                Count[_no] = 10; OldState[_no] = 1;
                return OldState[_no];
            }
		}       
	}
	return 0xEF;
}


/*
*********************************************************************************************************
*	�� �� ��: Read_KMFB
*	����˵��: ����Ƿ��й����ź�
*	�� 	  ��: ѡ����ͨ��
*	�� �� ֵ: 0�����ͨ��1���ⲿ�޴����ź�
*	�� 	  ע��
*			  ������ֻ���ڼ�������������źż�⣬220���������һ��������ɣ����ʹ�ò���ϵͳע��������
			  ������ܳ���ͬʱ����һ���豸���
*********************************************************************************************************
*/
u8 Read_Optocoupler(uint8_t uAreaDat)  //
{	
	SetInput_CD4067Switch(uAreaDat);
	BspInput_CD4067_Delay_us(50);
	return Read_InputData();
}


void ReadDat_CD4067(void)
{
    uint8_t ApPowerDat=0,BpPowerDat=0;
    uint8_t ApKMDat=0,BpKMDat=0;
    if((Read_Optocoupler(RDKMA1)==1)||(Read_Optocoupler(RDKMA2)==1) || (Read_Optocoupler(RDKMA3)==1))
    {   ApKMDat=0xAA;           }//A�ýӴ��������쳣
    else
    {   ApKMDat=0x00;           }//A�ýӴ�����������
	if((Read_Optocoupler(RDKMB1)==1)||(Read_Optocoupler(RDKMB2)==1) || (Read_Optocoupler(RDKMB3)==1))
    {   BpKMDat=0xAA;           }//B�ýӴ��������쳣
    else
    {   BpKMDat=0x00;           }//B�ýӴ�����������
    ApPowerDat = Read_InputDevDat(2);//A�ö����磻0->�쳣;1->���� 
    BpPowerDat = Read_InputDevDat(3);//B�ö����磻0->�쳣;1->���� 
    
    if((ApPowerDat==0)&&(ApKMDat==0x00))    {   RS485Dat_LED7_OFF();    } //��һ����ָʾ��
    else                                    {   RS485Dat_LED7_ON();     }
    if((BpPowerDat==0)&&(BpKMDat==0x00))    {   RS485Dat_LED8_OFF();    } //��������ָʾ��        
    else                                    {   RS485Dat_LED8_ON();     }		

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


