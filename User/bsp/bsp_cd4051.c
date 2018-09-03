#include "stm32f10x.h"		
#include "bsp_cd4051.h"		

static void BspInput_CD4051_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
}

void BspInput_CD4051_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SetInput_CSCD4051Switch(uint8_t uSetDat)	//�����ѹ�ɼ�ѡ��� ��CS5463��ʹ��
{
    switch (uSetDat)
    {
        case IV_C:
            Iutput_CSCD4051A_Low();	Iutput_CSCD4051B_Low();	
            break;
        case IV_B:
            Iutput_CSCD4051A_High();	Iutput_CSCD4051B_Low();	
            break;
        case IV_A:
            Iutput_CSCD4051A_Low();	Iutput_CSCD4051B_High();	
            break;
       default:
            break;
    }
}
void SetInput_IICD4051Switch(uint8_t uSetDat)	//��������ɼ�ѡ��� ��CS5463��ʹ��
{
    switch (uSetDat)
    {
        case II_A:
            Iutput_IICSCD4051A_Low();	Iutput_IICSCD4051B_High();	
            break;
        case II_B:
            Iutput_IICSCD4051A_High();	Iutput_IICSCD4051B_Low();	
            break;
        case II_C:
            Iutput_IICSCD4051A_Low();	Iutput_IICSCD4051B_Low();	
            break;
       default:
            break;
    }
}

static void SetInput_KPCD4051Switch(uint8_t uSetDat)
{
    switch (uSetDat)
    {
		 case 0:
			 Iutput_KPCD4051A_Low();	 Iutput_CSCD4051B_Low();	 Iutput_KPCD4051C_Low();
			 break;
		 case 1:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_Low();
			 break;
		 case 2:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_Low(); 
			 break;
		 case 3:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_Low();	 
			 break;
		 case 4:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 5:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_Low();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 6:
			 Iutput_KPCD4051A_Low();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_High();	 
			 break;
		 case 7:
			 Iutput_KPCD4051A_High();	 Iutput_KPCD4051B_High();	 Iutput_KPCD4051C_High();	 
			 break;
		default:

            break;
    }
}


/*
*********************************************************************************************************
*	�� �� ��: Read_Input_KPDevDat
*	����˵��: ���Ӵ���ǰ�����Ƿ��е�ѹ���������ϼ̵������ص��Ƿ���220v��
*	�� 	  ��: ѡ����ͨ��
*	�� �� ֵ: 0�������д�����1���ⲿ�޴����ź�
*	�� 	  ע��
*					����ź�����Ƶ��Ӧ��ϵ����
*
*					02		A��KM1ǰ�����ѹ�ź�
*					01		A��KM2ǰ�����ѹ�ź�
*					00		A��KM3ǰ�����ѹ�ź�
*					03		B��KM1ǰ�����ѹ�ź�
*					04		B��KM2ǰ�����ѹ�ź�
*					05		B��KM3ǰ�����ѹ�ź�
*
*		�㷨˵��������ǰ����220v��ѹ����˹�������˲��β��ߵ�ƽ���ߵ͵�ƽ�����Ƿ�������Ҫ�����˴���,
*				  220V��ѹ��Ƶ��Ϊ50hz��20ms��һ��������ֻ��5msʱ����Ե�ͨ�������ʱ�䲻��ͨ��
*				  �����Ҫ������β���ͨʱ��
*********************************************************************************************************
*/
u8 Read_Input_KPDevDat(uint8_t uAreaDat)  
{
	u8 i;

	SetInput_KPCD4051Switch(uAreaDat);
	BspInput_CD4051_Delay(20);
	if(uAreaDat<6)
	{
		if(Read_InputData_KP()==1)   //�ߵ�ƽ����Ҫ��һ���������һ�����ڣ��鿴�������Ƿ��м�⵽�ź�
		{
			for(i=0;i<10;i++)
			{
				
				if(Read_InputData_KP()==0)	return 0;
				BspInput_CD4051_Delay(50);
			}	
		}
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: CD4051Read_Optocoupler
*	����˵��: ����Ƿ��й����ź�
*	�� 	  ��: ѡ����ͨ��
*	�� �� ֵ: 0�����ͨ��1���ⲿ�޴����ź�
*	�� 	  ע��
*			  ������ֻ���ڼ�������������źż�⣬220���������һ��������ɣ����ʹ�ò���ϵͳע��������
			  ������ܳ���ͬʱ����һ���豸���
*********************************************************************************************************
*/
u8 CD4051Read_Optocoupler(uint8_t uAreaDat)  
{	
	SetInput_KPCD4051Switch(uAreaDat);
	BspInput_CD4051_Delay(20);
	return Read_InputData_KP();
}

