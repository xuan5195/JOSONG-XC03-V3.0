//����˵����


#include "bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void InitBoard(void);
void Delay (uint16_t nCount);

uint8_t g_RxMessage[8]={0};	//CAN��������
uint8_t g_RxMessFlag=0;		//CAN�������� ��־
uint8_t OutFlag=0;	        //��ˮ��־,�ϵ��־
uint8_t InPutCount=0;		    //�����������
uint8_t g_ShowUpDateFlag=0;		//��ʾ���±�־
uint32_t g_RunningTime=0;		//����ʱ��
uint16_t g_CANShowLED;          //���ڴ洢CAN������LED��ʾ
uint16_t g_ShowDat[6]={0};
uint8_t StartTimerFlag = 0x00;  //�����Զ������л���ʱ��־��
uint8_t User_StartT_Flag = 0x00;  //�����Զ������л���ʱ��־��
uint8_t g_TM1639Flag;
uint8_t g_KeyNoneCount;
uint8_t g_ChangeFlag=0x00;
//uint8_t g_StartMode = HH33;
extern CQ_FIFO_T s_gCQ;			//��ϢFIFO����,�ṹ��
extern uint8_t RS485_Count;
extern uint8_t RS485Dat[10];
extern MotorChar gAp,gBp;  //A\B�������Ϣ
extern uint8_t Menu;
extern uint8_t g_ShowMode;

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
    BufDat[ 4] = Buf[1];     	
    BufDat[ 5] = Buf[2];     	
    BufDat[ 6] = Buf[3];     	
    BufDat[ 7] = Buf[4];     	
    BufDat[ 8] = Buf[8];		
    BufDat[ 9] = Buf[5];     
    BufDat[10] = Buf[6];     
    BufDat[11] = Buf[7];  
    BufDat[12] = Buf[9];
    BufDat[13] = Buf[10];
	BufDat[14] = 0x02;
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
    uint8_t RunMode = 0;    //0�ֶ�ģʽ(Ĭ��)��1��һ������2������һ;
    uint8_t RunDate = 0x11; //�������ݣ�����λΪA��,����λΪB�� 0x1ֹͣ��0x2���٣�0x4����;
    uint8_t OldRunMode=0,OldStartFlag=0;
    uint8_t OlineSrart=0;   //Զ��������־
    uint8_t HumiFlag=0;     //��ʪ��־��0x00��ʾֹͣ��0x05��ʾ������
	uint8_t canbuf[8]={0};  //CAN���ݻ���
	SystemInit();
	
	InitBoard();		//Ӳ����ʼ��
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS1_4tq,CAN_BS2_3tq,75,CAN_Mode_Normal);	//CAN��ʼ������ģʽ,������60Kbps  //������Ϊ:36M/((1+2+1)*150)= 60Kbps CAN_Normal_Init(1,2,1,150,1);   
	//printf("CAN_Mode_Init,CAN_Baud: 60Kbps...\r\n");	
    printf("-------------------------------------------------------------------\r\n");
	bsp_StartTimer(1, 200);			//��ʱ��1���� 200����
    RS485Dat_LED1_ON();	//Զ��ͨ��ָʾ��
    RS485Dat_LED2_ON(); //����ͨ��ָʾ��	
    RS485Dat_LED12_ON();//A��ָֹͣʾ��
    RS485Dat_LED18_ON();//B��ָֹͣʾ��
    RS485Dat_LED16_ON();//�ֶ�ģʽָʾ��
    RS485Dat_LED6_ON(); //�������
    delay_ms(10);
	KMOFF_Show(ALARMALL);KMOutUpdat();
	HC595_E1_ON();HC595_E2_ON();
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
                if(DHT11_Read_Data((uint8_t *)DHT_Dat)==0)
                {   g_ShowDat[2] = DHT_Dat[2]; g_ShowDat[3] = DHT_Dat[0];   }
                Get_InputValue();
			}
            if(Time_250ms%10==0)        RS485Dat_LED2_ON();     //����ͨ��ָʾ��
            else if(Time_250ms%10==5)   RS485Dat_LED2_OFF();    //����ͨ��ָʾ��
            if(Time_250ms%2==0)         CanSendDat(g_ShowMode); //CAN���ݷ���
            ReadInputDat(gParamDat[Menu_StartMode]);     //��ȡA/B��ȫ��״̬������ڽṹ����
            DisplaySendDat();   //RS485���ݷ���
			RS485_ReceiveDat();
			if( ( 0x80 & RS485_Count ) == 0x80 )	//���յ�����
			{
				RS485_Count = 0;
                RS485Dat_Key[0] = RS485Dat[4];  RS485Dat_Key[1] = RS485Dat[5];  //��ȡ����ֵ
                if((RS485Dat_Key[0]!=0x00)||(RS485Dat_Key[1]!=0x00))
                {
                    //printf("RS485_ReceiveDat(),%02X,%02X.\r\n",RS485Dat_Key[0],RS485Dat_Key[1]);
                    if((RS485Dat_Key[1]&0x01)==0x01){NVIC_SystemReset();}       //ϵͳ��λ
                    else if((RS485Dat_Key[1]&0x08)==0x08){RunMode=1;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //��һ����
                    else if((RS485Dat_Key[1]&0x10)==0x10){RunMode=0;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //�ֶ�ģʽ
                    else if((RS485Dat_Key[1]&0x20)==0x20){RunMode=2;RS485Dat_Key[0]=0;RS485Dat_Key[1]=0;}  //������һ
                }
			}
            if(RunMode==0)      //�ֶ�ģʽ,ֱ�Ӳ����̵������
            {
                RS485Dat_LED15_OFF();RS485Dat_LED16_ON();RS485Dat_LED17_OFF();
				if(gParamDat[Menu_StartMode]==HHFJ)	//���ģʽ�����ٰ�����Ч
				{
					if((RS485Dat_Key[1]&0x02)==0x02)//A��ֹͣ        
					{
						RunDate = (RunDate&0xF0)|0x01;
						printf("�ֶ�����,A�ã�ֹͣ.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[0]&0x01)==0x01)   //A�õ���
					{   
						RunDate = (RunDate&0xF0)|0x02;
						printf("�ֶ�����,A�ã�����.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x04)==0x04)   //A�ø���
					{   
						RunDate = (RunDate&0xF0)|0x04;
						printf("�ֶ�����,A�ã�����.0x%02X.\r\n",RunDate);
					}				
					else if((RS485Dat_Key[1]&0x40)==0x40)   //B��ֹͣ
					{   
						RunDate = (RunDate&0x0F)|0x10;
						printf("�ֶ�����,B�ã�ֹͣ.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[0]&0x02)==0x02)   //B�õ���
					{   
						RunDate = (RunDate&0x0F)|0x20;
						printf("�ֶ�����,B�ã�����.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x80)==0x80)   //B�ø��� 
					{   
						RunDate = (RunDate&0x0F)|0x40;
						printf("�ֶ�����,B�ã�����.0x%02X.\r\n",RunDate);
					}
				}
				else	
				{
					if((RS485Dat_Key[1]&0x02)==0x02)//A��ֹͣ        
					{
						RunDate = (RunDate&0xF0)|0x01;
						printf("�ֶ�����,A�ã�ֹͣ.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x04)==0x04)   //A�ø���
					{   
						RunDate = (RunDate&0xF0)|0x02;
						printf("�ֶ�����,A�ã�����.0x%02X.\r\n",RunDate);
					}				
					else if((RS485Dat_Key[1]&0x40)==0x40)   //B��ֹͣ
					{   
						RunDate = (RunDate&0x0F)|0x10;
						printf("�ֶ�����,B�ã�ֹͣ.0x%02X.\r\n",RunDate);
					}
					else if((RS485Dat_Key[1]&0x80)==0x80)   //B�ø��� 
					{   
						RunDate = (RunDate&0x0F)|0x20;
						printf("�ֶ�����,B�ã�����.0x%02X.\r\n",RunDate);
					}
				}					
            }
            else if(RunMode==1)  //��һ����
            {
                RS485Dat_LED15_ON();RS485Dat_LED16_OFF();RS485Dat_LED17_OFF();
            }
            else if(RunMode==2)  //������һ
            {
                RS485Dat_LED15_OFF();RS485Dat_LED16_OFF();RS485Dat_LED17_ON();
            }
            if(RunMode!=OldRunMode) //����ģʽ�л���������ģʽȫ��תΪֹͣ
            {
                OldRunMode = RunMode;
                RunDate = 0x11;
                printf(">>����ģʽ�л�,ȫ��תΪֹͣ.0x%02X.  ",RunDate);
                if(RunMode==0)      {   printf("�ֶ�ģʽ.\r\n");    }
                else if(RunMode==1) {   printf("��һ����ģʽ.\r\n");}
                else if(RunMode==2) {   printf("������һģʽ.\r\n");}
            }
            if(RunMode!=0)  //�Զ�ģʽ
            {
                if((Read_Optocoupler(8)==0)||(Read_Optocoupler(9)==0)||(OlineSrart == 0xAA)) //Զ��1/Զ��2���
                {
                    if(StartTimerFlag==0x00)
                    {
                        StartTimerFlag=0xAA;        //��ǽ���������������ʱ����
						if(RunMode==1)  	{RunDate=0x12;printf("�Զ�ģʽ.����A��.");}//��һ����
						else if(RunMode==2) {RunDate=0x21;printf("�Զ�ģʽ.����B��.");}//������һ
                    }
                    else if(StartTimerFlag==0xBB)   //����ڶ��׶Σ���ʱʱ�䵽���ö�ʱ�ڶ�ʱ����ʵ��
                    {
                        StartTimerFlag=0xCC;        //��ǽ����������
						if(RunMode==1)  	{RunDate=0x14;printf("�л�����.\r\n");}//��һ����
						else if(RunMode==2) {RunDate=0x41;printf("�л�����.\r\n");}//������һ
                    }
                }
                else
                {
                    if(StartTimerFlag!=0x00)
                    {
						RunDate = 0x11;
						StartTimerFlag = 0x00;  
						g_ChangeFlag = 0x00;
					}
                }
            }
			else
			{
				if(gParamDat[Menu_StartMode]!=HHFJ)	//���� ���ģʽ������-�����Զ���ת
				{
					if((RunDate&0x0F)!=0x01)	
					{
						if(gAp.StartT_Flag==0x00)
						{
							gAp.StartT_Flag=0xAA;
							RunDate = (RunDate&0xF0)|0x02;
                            printf("�ֶ�ģʽ.����A��.  ");
						}
						else if(gAp.StartT_Flag==0xBB)
						{
							gAp.StartT_Flag=0xCC;
							RunDate = (RunDate&0xF0)|0x04;
                            printf("�л�����.\r\n");
						}
						
					}
					else
					{
						gAp.StartT_Flag=0x00;
						RunDate = (RunDate&0xF0)|0x01;
					}
					
					if((RunDate&0xF0)!=0x10)	
					{
						if(gBp.StartT_Flag==0x00)
						{
							gBp.StartT_Flag=0xAA;
							RunDate = (RunDate&0x0F)|0x20;
                            printf("�ֶ�ģʽ.����B��.  ");
						}
						else if(gBp.StartT_Flag==0xBB)
						{
							gBp.StartT_Flag=0xCC;
							RunDate = (RunDate&0x0F)|0x40;
                            printf("�л�����.\r\n");
						}
						
					}
					else
					{
						gBp.StartT_Flag=0x00;
						RunDate = (RunDate&0x0F)|0x10;
					}				
				}
			}
            RunDate = KMStart_Pro(RunMode,RunDate,gParamDat[Menu_StartMode]);
            if(gParamDat[Menu_Ha]>gParamDat[Menu_Hb])   //ʪ������>����ֵ �����Զ���ʪ����
            {
                if((g_ShowDat[3]>gParamDat[Menu_Ha])&&(HumiFlag==0x00))
                {
                    HumiFlag = 0x05;
                    KMON_Show(AHUMIDITY);   //�򿪳�ʪ
                    printf("ʪ��%2d(Limit:%2d-%2d),������ʪ�豸!\r\n",g_ShowDat[3],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                }
                if(((g_ShowDat[3])<gParamDat[Menu_Hb])&&(HumiFlag!=0x00))
                {
                    HumiFlag = 0x00;
                    KMOFF_Show(AHUMIDITY);   //�رճ�ʪ
                    printf("ʪ��%2d(Limit:%2d-%2d),�رճ�ʪ�豸.\r\n",g_ShowDat[3],gParamDat[Menu_Hb],gParamDat[Menu_Ha]);
                }
            }
            KMOutAnswer();  //��Դ/��Դ�̵����������
        }
        KMOutUpdat();   //ͳһ���¼̵������
        SetParam();     //�����޸Ĳ���
        if(Menu==Menu_Idle)
        {
            if(g_ShowMode == Show_UV)       { BspTm1639_Show(g_ShowUpDateFlag+0,g_ShowDat[g_ShowUpDateFlag+0]);}   //ѭ����ʾ ��ѹ������
            else if(g_ShowMode == Show_CF)  { BspTm1639_Show(g_ShowUpDateFlag+2,g_ShowDat[g_ShowUpDateFlag+2]);}   //ѭ����ʾ �¶ȡ�ʪ��
            else if(g_ShowMode == Show_PL)  { BspTm1639_Show(g_ShowUpDateFlag+4,g_ShowDat[g_ShowUpDateFlag+4]);}   //ѭ����ʾ ѹ��������
        }
        else                BspTm1639_ShowParam(g_TM1639Flag,Menu,gParamDat[Menu]);
        if(bsp_GetCQ((uint8_t *)canbuf)!=CQ_NONE)//���յ�������
        {
			//printf(">>CanRxMsg:%02X %02X %02X%02X%02X%02X%02X%02X.\r\n",canbuf[0],canbuf[1],canbuf[2],canbuf[3],canbuf[4],canbuf[5],canbuf[6],canbuf[7]);
            if     (canbuf[0]==0x01){RunMode=1;}  //��һ����
            else if(canbuf[0]==0x02){RunMode=0;}  //�ֶ�ģʽ
            else if(canbuf[0]==0x03){RunMode=2;}  //������һ
            if((OldStartFlag==0x00)&&(canbuf[1]==0x55))
            {   //Զ�̰�,������������
                OldStartFlag=0xAA;  printf("Զ�̰�,������������!\r\n");
                OlineSrart = 0xAA;  RS485Dat_LED3_ON();
            }  
            else if((OldStartFlag==0xAA)&&(canbuf[1]!=0x55))
            {
                OldStartFlag=0x00;  printf("Զ�̰�,���������ɿ�!\r\n");
                OlineSrart = 0x00;  RS485Dat_LED3_OFF();
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
	delay_ms(200);
	Init_GPIO();				//�����ʼ��
    CPU_IDLE();delay_ms(200);
	bsp_InitKey();
	bsp_InitUart(); 	    	//��ʼ������+RS485
	printf("\r\nStarting Up...\r\nJOSONG-XS03 V3.0...\r\n");
	printf("VersionNo: %02X...\r\n",VERSION);
	printf("SystemCoreClock: %d...\r\n",SystemCoreClock);
	delay_ms(100);
	BspTm1639_Config();	        //TM1639��ʼ��
	BspTm1639_Show(0xA1,0x00);	//�ϵ��ʼ����ʾ
	BspDht11_Config();			//��ʪ�ȴ�������ʼ��
	bsp_HC595_Config();			//HC595��ʼ�� ���ڼ̵������
	BspInput_CD4067_Config();	//CD4067��ʼ�� �����źŲɼ�
	BspInput_CD4051_Config();	//CD4051*3
	Bsp_CS5463_Config();
	CS546x_Init(0);
    Read_Flash_Dat();   //����Flash����
    CPU_IDLE();
    delay_ms(200);
	TIM3_Int_Init(99,720-1);  	//��100khz��Ƶ�ʼ�����0.01ms�жϣ�������100 *0.01ms Ϊ1ms 
//	TIM2_Cap_Init(0xFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� 
 	Adc_Init();		  		    //ADC��ʼ��
	bsp_InitCQVar();
}

extern void SysTick_ISR(void);	/* ���������ⲿ�ĺ��� */
uint16_t Time_count=0,KMTime_count=0;
uint16_t gApTime_Count=0,gBpTime_Count=0;
uint8_t KMTime_Sec=0,gApTime_Sec=0,gBpTime_Sec=0;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //TIM3�����ж� 1ms�ж�
	{
		if(Time_count>3000) //3�����ڲ���
        {
            Time_count = 0;
            g_ShowUpDateFlag = (1+g_ShowUpDateFlag)%2;  //0-1-0
        }
		Time_count++;
        if((Time_count%100)==1) //100mS�ж�
        {
            if(gAp.DelayCheck_Count>0)  gAp.DelayCheck_Count--;//��ʱ���ʱ�䵽��������
            else                        gAp.DelayCheck_Count=0;
            if(gBp.DelayCheck_Count>0)  gBp.DelayCheck_Count--;//��ʱ���ʱ�䵽��������
            else                        gBp.DelayCheck_Count=0;
            if((gAp.SlowTimer_Count>0)&&(gAp.SlowTimer_Count<200))   gAp.SlowTimer_Count--;//��������ʱ��
            if((gBp.SlowTimer_Count>0)&&(gAp.SlowTimer_Count<200))   gBp.SlowTimer_Count--;//��������ʱ��
        }
        if((Time_count%1000==700)&&(g_TM1639Flag==0))    g_TM1639Flag = 1;
        if((Time_count%1000==  1)&&(g_TM1639Flag==1))    g_TM1639Flag = 0;
        if(Time_count%1000==1)  {  if(g_KeyNoneCount>0)  g_KeyNoneCount--;  }
        if(StartTimerFlag==0xAA)    
        {
            KMTime_count++;
            if(KMTime_count>1000) //1�����ڶ�ʱ
            {
                KMTime_count = 0;
                KMTime_Sec++;
                if(KMTime_Sec>6)    StartTimerFlag = 0xBB;  //��ʱʱ�䵽
            }
        }
        else if(StartTimerFlag==0x00)
        {
            KMTime_count = 0;   KMTime_Sec = 0;
        }

		if(gAp.StartT_Flag==0xAA)
		{
			gApTime_Count++;
			if(gApTime_Count>1000)
			{
				gApTime_Count=0;
				gApTime_Sec++;
				if(gApTime_Sec>6)	gAp.StartT_Flag = 0xBB;
			}
		}
		else if(gBp.StartT_Flag==0x00)
		{
			gApTime_Count=0;gApTime_Sec=0;
		}
		if(gBp.StartT_Flag==0xAA)
		{
			gBpTime_Count++;
			if(gBpTime_Count>1000)
			{
				gBpTime_Count=0;
				gBpTime_Sec++;
				if(gBpTime_Sec>6)	gBp.StartT_Flag = 0xBB;
			}
		}
		else if(gBp.StartT_Flag==0x00)
		{
			gBpTime_Count=0;gBpTime_Sec=0;
		}		
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



