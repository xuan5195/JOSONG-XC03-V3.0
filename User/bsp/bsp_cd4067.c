#include "stm32f10x.h"		
#include "bsp_cd4067.h"		

static void BspInput_CD4067_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
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
u8 Read_InputDevDat(uint8_t uAreaDat)  //
{
	u8 i;	
	SetInput_CD4067Switch(uAreaDat);
	BspInput_CD4067_Delay(5);
	if(uAreaDat<4) 
	{
		if(Read_InputData()==1)   //�ߵ�ƽ����Ҫ��һ������,�����ǵ͵�ƽ
		{
			for(i=0;i<10;i++)
			{
				if(Read_InputData()==0)	return 0;
				BspInput_CD4067_Delay(5);
			}	
		}
	}
	return 1;
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
	BspInput_CD4067_Delay(5);
	return Read_InputData();
}


