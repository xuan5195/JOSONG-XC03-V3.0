//����˵����


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
	uint8_t DHT_Dat[5]={0},RS485Dat_Key[2]={0};
	uint8_t Time_250ms=0;
	uint8_t KeyDat=0;
    uint8_t RunMode = 0;    //0�ֶ�ģʽ(Ĭ��)��1��һ������2������һ;
    KMDat ApRun,BpRun;      //A/B�� ����״̬    
	SystemInit();
	
	InitBoard();		//Ӳ����ʼ��
	BspTm1639_Show(0x01,0x00);	//�ϵ��ʼ����ʾ
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN��ʼ������ģʽ,������60Kbps  //������Ϊ:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");
	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//��ʱ��1���� 200����
	KMOFF_Show(ALARMALL);
	HC595_E1_ON();HC595_E2_ON();
    RS485Dat_LED1_ON();	//Զ��ͨ��ָʾ��
    RS485Dat_LED2_ON(); //����ͨ��ָʾ��	
    RS485Dat_LED12_ON();//A��ָֹͣʾ��
    RS485Dat_LED18_ON();//B��ָֹͣʾ��
    RS485Dat_LED16_ON();//�ֶ�ģʽָʾ��
    while (1)
	{
		CPU_IDLE();
		if ( bsp_CheckTimer(1) )	//��ʱ��
		{
			bsp_StartTimer(1, 200);	//�����¸���ʱ����
			if(Time_250ms<20)	Time_250ms++;
			else	
			{	
				Time_250ms=0;
                if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0){;}
                Get_InputValue();
			}
            //ReadDat_CD4067();
            ReadInputDat();
            DisplaySendDat();
			KeyDat = bsp_GetKey();
			if(KeyDat!=KEY_NONE)	//������⼰���ݴ���
			{
				switch (KeyDat)
				{
					case KEY_NONE:	//�ް�������
						break;
					case KEY_1_DOWN:	//						
						printf("  KEY_1_DOWN!\r\n");
						break;
					case KEY_1_UP:		//
						printf("  KEY_1_UP!\r\n");
						break;
					case KEY_2_DOWN:	//
						printf("  KEY_2_DOWN!\r\n");						
						break;
					case KEY_2_UP:		//
						printf("  KEY_2_UP!\r\n");
						break;
					case KEY_3_DOWN:	//
                        RS485Dat_LED1_ON();
						printf("  KEY_3_DOWN!\r\n");
						break;
					case KEY_3_UP:		//
                        RS485Dat_LED1_OFF();
						printf("  KEY_3_UP!\r\n");
						break;
					case KEY_4_DOWN:	//
                        RS485Dat_LED2_ON();
						printf("  KEY_4_DOWN!\r\n");						
						break;
					case KEY_4_UP:		//
                        RS485Dat_LED2_OFF();
						printf("  KEY_4_UP!\r\n");
						break;
					default:
						break;
				}
			}

			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//���յ�����
			{
				RS485_Count = 0;
                RS485Dat_Key[0] = RS485Dat[4];  RS485Dat_Key[1] = RS485Dat[5];  //��ȡ����ֵ
                if((RS485Dat_Key[0]!=0x00)||(RS485Dat_Key[1]!=0x00))
                printf("RS485_ReceiveDat(),%02X,%02X.\r\n",RS485Dat_Key[0],RS485Dat_Key[1]);
                if(RS485Dat_Key[1]&0x01==0x01){;}       //ϵͳ��λ
                else if((RS485Dat_Key[1]&0x08)==0x08){RunMode=1;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //��һ����
                else if((RS485Dat_Key[1]&0x10)==0x10){RunMode=0;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //�ֶ�ģʽ
                else if((RS485Dat_Key[1]&0x20)==0x20){RunMode=2;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //������һ
			}
            if(RunMode==0)  //�ֶ�ģʽ
            {
                if((RS485Dat_Key[1]&0x02)==0x02)//A��ֹͣ        
                {   
                    ApRun.Statue=Stop;
                    printf("�ֶ�ģʽ,A��ֹͣ.\r\n");
                    KMOFF_Show(AKM1RUN);KMOFF_Show(AKM2RUN);KMOFF_Show(AKM3RUN);
                }  
                else if((RS485Dat_Key[0]&0x01)==0x01)   //A�õ���
                {   
                    ApRun.Statue=Slow;     
                    printf("�ֶ�ģʽ,A�õ���.\r\n");
                    KMON_Show(AKM1RUN); KMON_Show(AKM2RUN); KMOFF_Show(AKM3RUN);
                }  
                else if((RS485Dat_Key[1]&0x04)==0x04)   //A�ø���
                {   
                    ApRun.Statue=HighSpeed;
                    printf("�ֶ�ģʽ,A�ø���.\r\n");
                    KMON_Show(AKM1RUN); KMOFF_Show(AKM2RUN);KMON_Show(AKM3RUN); 
                }  
                else if((RS485Dat_Key[1]&0x40)==0x40)   //B��ֹͣ
                {   
                    BpRun.Statue=Stop;     
                    printf("�ֶ�ģʽ,B��ֹͣ.\r\n");
                    KMOFF_Show(BKM1RUN);KMOFF_Show(BKM2RUN);KMOFF_Show(BKM3RUN);
                }  
                else if((RS485Dat_Key[0]&0x02)==0x02)   //B�õ���
                {   
                    BpRun.Statue=Slow;     
                    printf("�ֶ�ģʽ,B�õ���.\r\n");
                    KMON_Show(BKM1RUN); KMON_Show(BKM2RUN); KMOFF_Show(BKM3RUN);
                }  
                else if((RS485Dat_Key[1]&0x80)==0x80)   //B�ø��� 
                {   
                    BpRun.Statue=HighSpeed;
                    printf("�ֶ�ģʽ,B�ø���.\r\n");
                    KMON_Show(BKM1RUN); KMOFF_Show(BKM2RUN);KMON_Show(BKM3RUN); 
                }                 
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



