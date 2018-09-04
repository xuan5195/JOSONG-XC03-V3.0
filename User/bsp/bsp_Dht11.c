#include "stm32f10x.h"
#include "bsp_Dht11.h"
#include "bsp.h"


//��λDHT11
static void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//����Ϊ���
	DHT11_DQ_Low(); 	//����DQ
	delay_ms(20);    	//��������18ms
	DHT11_DQ_High(); 	//DQ=1 
	delay_us(30);     	//��������20~40us
}

/*
*********************************************************************************************************
*	�� �� ��: DHT11_Check
*	����˵��: ��ʪ���豸���߼��
*	��     ��: ��
*	�� �� ֵ: 0,����;1,δ��⵽DHT11�Ĵ��� 
*	��     ע��
*********************************************************************************************************
*/
static uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;	
	DHT11_IO_IN();      //����Ϊ����	 
	while (DHT11_DQ_IN()&&(retry<110))//DHT11������83us
	{
		retry++;
		delay_us(1);
	}	 
	if(retry>=100)	return 1;
	else 			retry=0;

	while ((!DHT11_DQ_IN())&&(retry<100))//DHT11���ͺ���ٴ�����87us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)	return 1;	    
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: DHT11_Read_Bit
*	����˵��: ��DHT11��ȡһ��λ
*	��     ��: ��
*	�� �� ֵ: 1/0 
*	��     ע��54 ΢��ĵ͵�ƽ�� 23-27 ΢��ĸߵ�ƽ��λ���ݡ�1���ĸ�ʽΪ�� 54 ΢��ĵ͵�ƽ��68-
*				74΢��ĸߵ�ƽ
*********************************************************************************************************
*/
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;	
	while(DHT11_DQ_IN()&&(retry<100))	//�ȴ���Ϊ�͵�ƽ�����ȴ�����54us�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN()&&(retry<100))	//�ȴ���ߵ�ƽ,֮����ݸߵ�ƽʱ���ж�������1����0
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);		//�ȴ�48us����ֵ����ȡ���Է�������һλ��ʼʱ��
	if(DHT11_DQ_IN())return 1;
	else return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: DHT11_Read_Byte
*	����˵��: ��DHT11��ȡһ���ֽ�
*	��     ��: ��
*	�� �� ֵ: ���������� 
*	��     ע��
*********************************************************************************************************
*/
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	
	dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
/*
*********************************************************************************************************
*	�� �� ��: DHT11_Read_Data
*	����˵��: ��ʪ�ȼ��
*	��     ��: temp->�¶����ݣ�humi->ʪ������
*	�� �� ֵ: 0,����;1,��ȡʧ�� 
*	��     ע��temp:�¶�ֵ(��Χ:0~50��)��humi:ʪ��ֵ(��Χ:20%~90%)
*********************************************************************************************************
*/
uint8_t DHT11_Read_Data(uint8_t *buf)    
{        
	uint8_t i;
	
	DHT11_Rst();
	if(DHT11_Check()==0)
	{	
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();			
		}
		//printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X.",buf[0],buf[1],buf[2],buf[3],buf[4]);
		if(buf[4] != ((uint8_t)(buf[0]+buf[1]+buf[2]+buf[3])))
		{
			printf("Read Date Check Error!\r\n");
			return 1;
		}
		//printf(">>DHT11_Read_Data H:%d.%d; T:%d.%d.\r\n",buf[0],buf[1],buf[2],buf[3]);
	}
	else	return 1;
	return 0;	    
}


void BspDht11_Config(void) 
{
	uint8_t buf[5],i;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	DHT11_Rst();	
	if(DHT11_Check()==0)	printf("DHT11_Init OK!   ");
	else					printf("DHT11_Init Error!   ");
	for(i=0;i<5;i++)//��ȡ40λ����
	{
		buf[i]=DHT11_Read_Byte();			
	}
	if(buf[4] != ((uint8_t)(buf[0]+buf[1]+buf[2]+buf[3])))
			printf("Read Date Check Error!\r\n");
	else	printf("H:%d.%d; T:%d.%d.\r\n",buf[0],buf[1],buf[2],buf[3]);
}

