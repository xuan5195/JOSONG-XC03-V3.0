//����˵����
//2018.01.24  V5.1 ������δע��ʱҲ�ܹ�Զ�̸�λ
//2018.01.26  V5.1 ������δע��ʱ3min�Զ���λ����ߴ�����뿨��ʱ���߳ɹ���
//2018.01.27  V5.1 �޸��Ͽ����Ϊ0���������E300ʱ��ˢ����ɷ�ˮ��ֹͣ�����ƷѼ���ʾ����������⡣
//2018.02.03  V5.1 ������ˢ�������������ͨ���Ͻ�������˸һ�¡�
//2018.02.23  V5.2 �޸�CAN������Ϊ60kbps;ʧ����λʱ����ԭ��3min��Ϊ15Sec��
//2018.03.12  V5.3 �����ϵ������ⷢ����SN��������ע�ᣬ���ʹ���Ϊ10�Ρ�
//2018.04.21  V0.1 ���԰汾�޸ļ��Ż�Flash�洢���⣬��ԭʼֵ��Ϊȫ0��
//2018.04.24  V0.2 ���Ӳ忨/�ο����ݼ�¼���洢��ʼ��ַ0x0800D000,�ռ�10K���洢����800����
//2018.04.25  V0.2 ����Ǯ�����ݣ��������ϰ汾��
//2018.04.26  V0.3 ���ӱ���ˢ���ܿ۷ѽ����Ա��쳣�ύ"�ο����ۿ��";���Ϊ600,000,�������յ��ý��ʱ���Է�����Ϊ׼
//2018.04.26  V0.3 ����DRCCYCLE�Ʒ����ں��ύһ��"�ο����ݰ�";
//2018.04.26  V0.3 ���Ӻ˶�SN���߼���ַ�㲥,��һ�������λ;
//2018.05.01  V0.4 ����ʱ�ӽ��գ�ʱ�Ӵ洢��������־�����ڴ�ӡ��־��CANȡ��־����;
//2018.05.09  V0.5 ���ſ�ͬʱ���벢���ƶ��ÿ������ظ���д�����У��ύ����뿨�Ž�������;
//2018.05.10  V0.5 �޸�����Ǯ����һ��ʱ��ɿ���������;
//2018.05.10  V0.5 �������ϵ�ʱ������־�е�ʱ�ӵ�����-->��Ϊʱ��Ϊ0ʱ������ʱ��
//2018.05.16  V0.5 ����־��ԭ12Byte��Ϊ16Byte��������ˮ��ֵ2Byte���������ա�
//2018.05.16  V0.5 ��DecSum>200ʱ,����ط����ȴ����²忨��
//2018.05.16  V0.6 �汾��55��Ϊ56�� �޸��տ���ʱ��һ�β忨���������⡣
//                 �޸���־���⣬���忨\�ο�\���������Ƶ�BufDat[14]��ȥ�������ʽ�ĵ���
//2018.05.18  V0.6 �������Ѳ��ύ��������ֻ����־;
//2018.07.04  V0.6 �޸�����λΪ00ʱ���޷���������ֵ;

#include "bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



static void InitBoard(void);
void Delay (uint16_t nCount);

uint8_t g_RxMessage[8]={0};	//CAN��������
uint8_t g_RxMessFlag=0;		//CAN�������� ��־
uint8_t OutFlag=0;	//��ˮ��־,�ϵ��־
uint8_t InPutCount=0;		//�����������
uint32_t g_RunningTime=0;		//����ʱ��
extern CQ_FIFO_T s_gCQ;				//��ϢFIFO����,�ṹ�� */
extern uint8_t RS485_Count;
extern uint8_t RS485Dat[10];

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

void RCC_Configuration(void)
{
//    RCC_DeInit();
//    RCC_HSEConfig(RCC_HSE_OFF);
//    RCC_HSICmd(ENABLE);
//    RCC_HSEConfig(RCC_HSE_ON);
}
void CPU_IDLE(void)
{
	static uint8_t g_FlagDWG=0;
	if(++g_FlagDWG%2==0)	HardWDG_ON();
	else 					HardWDG_OFF();
}


uint8_t PutOutMemoryBuf(uint8_t *_Date)	//���һ������
{
	if (s_gCQ.Read == s_gCQ.Write)
	{
		return CQ_NONE;
	}
	else
	{
		memcpy(_Date,s_gCQ.Buf[s_gCQ.Read],16);
		if (++s_gCQ.Read >= CQ_FIFO_SIZE)
		{
			s_gCQ.Read = 0;
		}
		return CQ_OK;
	}
}

void PutInMemoryBuf(u8 *Buf)	//ĩβ�����һ������
{
    uint8_t BufDat[16]={0};
	//��־�洢
    BufDat[ 0] = (uint8_t)(g_RunningTime>>24); //ʱ��
    BufDat[ 1] = (uint8_t)(g_RunningTime>>16); //ʱ��
    BufDat[ 2] = (uint8_t)(g_RunningTime>>8);  //ʱ��
    BufDat[ 3] = (uint8_t)(g_RunningTime);     //ʱ��
    BufDat[ 4] = Buf[1];     	//����0��
    BufDat[ 5] = Buf[2];     	//����1��
    BufDat[ 6] = Buf[3];     	//����2��
    BufDat[ 7] = Buf[4];     	//����3��
    BufDat[ 8] = Buf[8];		//У��	
    BufDat[ 9] = Buf[ 5];     	//���1��λ
    BufDat[10] = Buf[ 6];     	//���2
    BufDat[11] = Buf[ 7];     	//���3
    BufDat[12] = Buf[ 9]&0x7F;	//DecSum��λ
    BufDat[13] = Buf[10];     	//DecSum��λ
	
	if((Buf[ 9]&0x80)==0x80)    BufDat[14] = 0x03;	 //������Ϣ���; 
	else
    {
        if( Buf[0] == 0xAA )	BufDat[14] = 0x01;  //�忨��
        else					BufDat[14] = 0x02;  //ȡ���� 
	}
    BufDat[15] = CRC8_Table(BufDat,15); 	//CRC
	

	memcpy(s_gCQ.Buf[s_gCQ.Write],Buf,10);

	if (++s_gCQ.Write  >= CQ_FIFO_SIZE)
	{
		s_gCQ.Write = 0;
	}
}

void Init_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SoftReset(void)
{
	__set_FAULTMASK(1);		// �ر������ж�
	NVIC_SystemReset();		// ��λ
}

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t DHT_Dat[5]={0},Buff[4]={0xFF,0x00,0xFF,0x00};
	uint8_t Time_250ms=0;
	uint8_t KeyDat=0;
	SystemInit();
	
	InitBoard();		//Ӳ����ʼ��
	BspTm1639_Show(0x01,0x00);	//�ϵ��ʼ����ʾ
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN��ʼ������ģʽ,������60Kbps  //������Ϊ:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");
	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//��ʱ��1���� 200����
	KMOFF_Show(ALARMALL);
	HC595_E1_ON();HC595_E2_ON();	
	while (1)
	{
		CPU_IDLE();
		if ( bsp_CheckTimer(1) )	//��ʱ��
		{
			bsp_StartTimer(1, 200);	//�����¸���ʱ����
			if(Time_250ms<9)	Time_250ms++;
			else	
			{	
				Time_250ms=0;
			}
			KeyDat = bsp_GetKey();
			if(KeyDat!=KEY_NONE)	//������⼰���ݴ���
			{
				switch (KeyDat)
				{
					case KEY_NONE:	//�ް�������
						break;
					case KEY_1_DOWN:	//
						if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0){;}
						printf("  KEY_1_DOWN!\r\n");
						break;
					case KEY_1_UP:		//
						printf("  KEY_1_UP!\r\n");
						break;
					case KEY_2_DOWN:	//
						printf("  KEY_2_DOWN!\r\n");
						Get_InputValue();
						break;
					case KEY_2_UP:		//
						printf("  KEY_2_UP!\r\n");
						break;
					case KEY_3_DOWN:	//
						KMON_Show(BKM1RUN);
						printf("  KEY_3_DOWN!\r\n");
						break;
					case KEY_3_UP:		//
						KMOFF_Show(BKM1RUN);
						printf("  KEY_3_UP!\r\n");
						break;
					case KEY_4_DOWN:	//
						printf("  KEY_4_DOWN!\r\n");
						Buff[0]=0xFF;Buff[1]=0xFF;
						RS485_SendDat((uint8_t *)Buff);
						break;
					case KEY_4_UP:		//
						printf("  KEY_4_UP!\r\n");
						Buff[0]=0x00;Buff[1]=0x00;
						RS485_SendDat((uint8_t *)Buff);
						break;
					default:
						break;
				}
			}


			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//���յ�����
			{
				RS485_Count = 0;
				printf("RS485:%02X%02X %02X%02X%02X %02X %02X %02X%02X.\r\n",
				RS485Dat[0],RS485Dat[1],RS485Dat[2],RS485Dat[3],RS485Dat[4],RS485Dat[5],RS485Dat[6],RS485Dat[7],RS485Dat[8]); 
			}

		}

	}
}

/*
*********************************************************************************************************
*	�� �� ��: InitBoard
*	����˵��: ��ʼ��Ӳ���豸
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitBoard(void)
{	
	RCC_Configuration();
	/* ��ʼ��systick��ʱ������������ʱ�ж� */
	bsp_InitTimer(); 
	delay_init();
	delay_ms(500);
	Init_GPIO();				//�����ʼ��
	bsp_InitKey();
	bsp_InitUart(); 	    	//��ʼ������+RS485
	printf("\r\nStarting Up...\r\nJOSONG-XS03 V3.0...\r\n");
	printf("VersionNo: %02X...\r\n",VERSION);
	printf("SystemCoreClock: %d...\r\n",SystemCoreClock);
	delay_ms(500);
	BspTm1639_Config();	        //TM1639��ʼ��
	BspDht11_Config();			//��ʪ�ȴ�������ʼ��
	bsp_HC595_Config();			//HC595��ʼ�� ���ڼ̵������
	BspInput_CD4067_Config();	//CD4067��ʼ�� �����źŲɼ�
	BspInput_CD4051_Config();	//CD4051*3
	Bsp_CS5463_Config();
	CS546x_Init(0);
	TIM3_Int_Init(99,720-1);  	//��100khz��Ƶ�ʼ�����0.01ms�жϣ�������100 *0.01ms Ϊ1ms 
//	TIM2_Cap_Init(0xFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� 
 	Adc_Init();		  		    //ADC��ʼ��
	bsp_InitCQVar();
}

extern void SysTick_ISR(void);	/* ���������ⲿ�ĺ��� */
uint8_t Time_count=0;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //TIM3�����ж� 20ms�ж�
	{
		if(Time_count>200)	Time_count=0;
		Time_count++;
		SysTick_ISR();	//���������bsp_timer.c�� 
		if((Time_count%10)==0)			bsp_KeyScan();	//ÿ10msɨ�谴��һ��
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  	//���TIMx�����жϱ�־ 					
	}
}

void Delay(__IO uint16_t nCount)
{
	while (nCount != 0)
	{
		nCount--;
	}
}



