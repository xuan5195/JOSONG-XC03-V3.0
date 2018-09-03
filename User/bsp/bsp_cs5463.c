#include "stm32f10x.h"		
#include "bsp_cs5463.h"		
#include "delay.h"
#include "bsp.h"

static UN32 CS546x_Buff;					//CS5463������
PhaseHandle Board_Phase;

/*static void BspInput_CS5463_Delay(u16 z) 
{ 
	u8 x;
	while(z--)
	{
		for(x=8;x>0;x--);
	}
}*/

void Bsp_CS5463_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: CS546x_Read_Byte
*	����˵��: ��ȡ�Ĵ���һ���ֽ�����
*	��     ��: ��
*	�� �� ֵ: ��ȡ����һ���ֽ����� 
*	��     ע��
*********************************************************************************************************
*/
u8 CS546x_Read_Byte(void)
{	
	u8 ReceiveDat,i;	
	ReceiveDat = 0;
	CS546x_SET_SDI;
//	delay_us(10);
	for(i=0;i<7;i++) 
	{
		CS546x_RESET_CLK;
		delay_us(5);
		CS546x_SET_CLK;
		delay_us(5);
		if(CS546x_Get_SDO)	ReceiveDat |= 1;
		ReceiveDat <<= 1;	
	}	
	return ReceiveDat;
}
/*
*********************************************************************************************************
*	�� �� ��: CS546x_Write_Byte
*	����˵��: ��Ĵ�����дһ���ֽ�����
*	��     ��: Ҫд�ĵ��ֽ�����
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS546x_Write_Byte(u8 WrDat)
{
	u8  tmp_8u,i;
	tmp_8u = WrDat;
	for(i=0;i<8;i++)
	{
		delay_us(10);
		CS546x_RESET_CLK;
		if((tmp_8u&0x80)==0)	CS546x_RESET_SDI;
		else					CS546x_SET_SDI;
		delay_us(2);
		tmp_8u <<= 1;
		CS546x_SET_CLK;
	}
	delay_us(10);
	CS546x_RESET_CLK;
}
/*
*********************************************************************************************************
*	�� �� ��: CS546x_RedReg
*	����˵��: ���ȡ�Ĵ���3���ֽ�����
*	��     ��: RegAddr,�Ĵ�����ַ;DustBuf,�������ݵĻ���;
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS546x_RedReg(u8 RegAddr,u8 *DustBuf)
{
	u8 i;	
	CS546x_RESET_CS;
	delay_us(10);
	CS546x_Write_Byte(RegAddr);
	for(i=0;i<3;i++)
	{
		*(DustBuf+i) = CS546x_Read_Byte();
	}
	CS546x_SET_CS;

}

/*
*********************************************************************************************************
*	�� �� ��: CS546x_WriteReg
*	����˵��: ��Ĵ�����д3�ֽ�����
*	��     ��: RegAddr,�Ĵ�����ַ;DustBuf��Ҫд�ĵ��ֽ�����;
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS546x_WriteReg(u8 RegAddr,u8 *DustBuf)
{
	u8 i;	
	CS546x_RESET_CS;
	delay_us(10);
	CS546x_Write_Byte(RegAddr);
	for(i=0;i<3;i++)
	{
		CS546x_Write_Byte(*(DustBuf+i));
	}
	CS546x_SET_CS;

}

/*
*********************************************************************************************************
*	�� �� ��: CS546x_ResetStaReg
*	����˵��: ���״̬�Ĵ�����־
*	��     ��: ��
*	�� �� ֵ: �� 
*	��     ע��д1��Ч��д0��Ч�������κδ���
*********************************************************************************************************
*/
void CS546x_ResetStaReg()
{
	CS546x_Buff.u8[0] = 0xFF;
	CS546x_Buff.u8[1] = 0xFF;
	CS546x_Buff.u8[2] = 0xFF;

	CS546x_WriteReg(Status | REG_WR,CS546x_Buff.u8);
}



#define READ_MASK		0xBF	//���Ĵ���ʱ�������룬�루д����ַ����
#define CMD_SYNC0    	0XFE  	//�����������³�ʼ�� 
#define CMD_SYNC1    	0XFF  	//��ʼ�������³�ʼ��
#define REG_CONFR   	0x40   	//���� 
#define REG_CYCCONT 	0x4A   	//һ���������ڵ�A/Dת���� 
#define REG_STATUSR 	0x5E 	//״̬ 
#define REG_MODER   	0x64  	//����ģʽ 
#define REG_MASKR   	0x74  	//�ж����� 
#define REG_CTRLR   	0x78  	//���� 
#define CMD_STARTC   	0XE8  	//ִ��������������

#define REG_VRMSR   	0X18  	//VRMS
#define REG_IRMSR   	0X16  	//IRMS
#define REG_Pactive   	0X14  	//Pactive


static u8 RX_Buff[4];					//CS5463��д������
u8 sta;									//оƬ״̬

/*
*********************************************************************************************************
*	�� �� ��: CS5463CMD
*	����˵��: �������оƬ
*	��     ��: ��
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
static void CS5463CMD(u8 cmd)
{
    u8 i;
    CS546x_SET_CLK;		//SCLK = 1;
    CS546x_RESET_CS;	//CS = 0;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(cmd&0x80)    CS546x_SET_SDI;		//MOSI = 1;
        else		    CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;			 		//��ʱ�������أ����ݱ�д��CS5463
        cmd <<= 1;
        i++;
    }
    delay_us(50);		//uDelay(50);
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
}


/*
*********************************************************************************************************
*	�� �� ��: CS5463WriteReg
*	����˵��: ���ļĴ�������
*	��     ��: addr->�Ĵ�����ַ;*p->��������;
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS5463WriteReg(u8 addr,u8 *p)
{	
    u8 i,j;
    u8 dat;
    CS546x_RESET_CS;	//CS = 0;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(addr&0x80)CS546x_SET_SDI;	//MOSI = 1;
        else		 CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;	//��ʱ�������أ����ݱ�д��CS5463
        addr <<= 1;
        i++;
    }
    j = 0;
    delay_us(50);		//uDelay(50);
    while(j<3)
    {
        dat = *(p+j);
        i = 0;
        while(i<8)
        {
            delay_us(50);		//uDelay(50);
            CS546x_RESET_CLK;	//SCLK = 0;
            if(dat&0x80)CS546x_SET_SDI;		//MOSI = 1;
            else		CS546x_RESET_SDI;	//MOSI = 0;
            delay_us(50);		//uDelay(50);
            CS546x_SET_CLK;		//SCLK = 1;   //��ʱ�������أ����ݱ�д��CS5463
            dat <<= 1;
            i++;
        }
        delay_us(50);		//uDelay(50);
        j++;
    }
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
    delay_us(50);		//uDelay(50);	
}

/*
*********************************************************************************************************
*	�� �� ��: CS5463ReadReg
*	����˵��: ��ȡ�Ĵ�����ֵ
*	��     ��: addr->�Ĵ�����ַ;*p->��������;
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS5463ReadReg(u8 addr,u8 *p)
{	
    u8 i,j;
    u8 dat;
    CS546x_RESET_CS;	//CS = 0;
    addr &= READ_MASK;
    i = 0;
    while(i<8)
    {
        delay_us(50);		//uDelay(50);
        CS546x_RESET_CLK;	//SCLK = 0;
        if(addr&0x80)CS546x_SET_SDI;	//MOSI = 1;
        else		 CS546x_RESET_SDI;	//MOSI = 0;
        delay_us(50);		//uDelay(50);
        CS546x_SET_CLK;		//SCLK = 1;
        addr <<= 1;	//��ʱ�������أ����ݱ�д��CS5463
        i++;
    }

    CS546x_SET_SDI;		//MOSI = 1;
    j = 0;
    while(j<3)
    {
        i = 0;
        dat = 0;
        while(i<8)
        {
            delay_us(50);		//uDelay(50);
            CS546x_RESET_CLK;	//SCLK = 0;
            if(i==7)CS546x_RESET_SDI;	//MOSI = 0;
            else	CS546x_SET_SDI;		//MOSI = 1;
            delay_us(50);		//uDelay(50);
            dat <<= 1;			 			
            if(CS546x_Get_SDO)	dat |= 0x01;
            else				dat &= 0xFE;
            CS546x_SET_CLK;		//SCLK = 1;
            delay_us(50);		//uDelay(50);					 		
            i++;
        }
        *(p+j) = dat;
        j++;
    }
    delay_us(50);		//uDelay(50);
    CS546x_SET_SDI;		//MOSI = 1;
    CS546x_SET_CS;		//CS = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: CS5463_ResetStatusReg
*	����˵��: ��λ״̬�Ĵ�����ֵ
*	��     ��: ��
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
static void CS5463_ResetStatusReg(void)
{
    RX_Buff[0] = 0xFF;
    RX_Buff[1] = 0xFF;
    RX_Buff[2] = 0xFF;
    CS5463WriteReg(0x5E,RX_Buff);		//��λ״̬�Ĵ���	#define REG_STATUSR 	0x5E 	//״̬  
}

/*
*********************************************************************************************************
*	�� �� ��: CS5463_GetStaReg_Val
*	����˵��: ��ȡ״̬�Ĵ�����ֵ
*	��     ��: ��
*	�� �� ֵ: �����ж���Ϣ 
*	��     ע��
*********************************************************************************************************
*/
u8 CS5463_GetStatusReg(void)
{ 
	u8 sta=0;

	CS5463ReadReg(0x1E,RX_Buff);	   //1E ��ʲô��   ״̬�Ĵ���
	if(RX_Buff[0]&0x80)		   			//��⣺��������ѹ�����ʲ����Ƿ����
	{
		//������/��ѹ�Ƿ񳬳���Χ
		//��������Чֵ/��ѹ��Чֵ/�����Ƿ񳬳���Χ
		if((RX_Buff[0]&0x03)||(RX_Buff[1]&0x70))
		{
			CS5463_ResetStatusReg();		//��λ״̬�Ĵ���
		}
		else
		{
			sta |= 0x01;//B0000_0001;	//��ʲô��˼ ����������д�� PT2017-2-8   �ָ����� 
		}
	}

	if(RX_Buff[2]&0x80)			   	//��⣺�¶Ȳ����Ƿ����
	{
		sta |=0x02; //B0000_0010;
	} 
	return(sta);	
}  



/*
*********************************************************************************************************
*	�� �� ��: CS5463_GetStaReg_Val
*	����˵��: ��ȡ״̬�Ĵ�����ֵ
*	��     ��: ��
*	�� �� ֵ: �����ж���Ϣ 
*	��     ע��
*********************************************************************************************************
*/
u8 CS5463_GetStaReg_Val(void)
{
	u8 ret=0;

	CS5463ReadReg(Status | REG_RD,CS546x_Buff.u8);
	if(CS546x_Buff.u8[0] & 0x80)
	{
		//������/��ѹ�Ƿ񳬳���Χ
		//��������Чֵ/��ѹ��Чֵ/�����Ƿ񳬳���Χ
		if((CS546x_Buff.u8[0]&0x03)||(CS546x_Buff.u8[1]&0x70))
		{
		 	CS546x_ResetStaReg();		//��λ״̬�Ĵ���
		}else
		{
			ret |= 0x01;//B0000_0001;	//��ʲô��˼ ����������д�� PT2017-2-8   �ָ����� 
		}
	}

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: CS546x_Get_Vrms
*	����˵��: ��ȡ��Ч��ѹ����
*	��     ��: DustBuf���������������ݻ���;
*	�� �� ֵ: �� 
*	��     ע���㷨����
*
*			����һ�κ�����ϵ y=kx+b;����yΪ���������˵�ѹ��xΪ�Ĵ�����ֵ/0xFFFFFF,k
*			Ϊ����ϵ����bΪƫ��ֵ�������Ҫ���k��bֵ�����ڼĴ���ÿ�ζ�ȡ��������������Ҫ
*			���ȡֵ��ƽ��ֵ�Լ�С���
*
*
*	�������ѹ�Ƶ������ѹ��Vres/100*5/2*640
*
*
*********************************************************************************************************
*/
u32 CS546x_Get_Vrms()
{
	float Tmp;
	UN32 ResceBuf;
	u8 DustBuf[4]={0};
	
	ResceBuf.u32 = 0;
	CS5463ReadReg(REG_RD|Vrms,DustBuf);
//	ShowHex(DustBuf,4);						//��ӡԭʼ����

	ResceBuf.u8[0] =  DustBuf[2];
	ResceBuf.u8[1] =  DustBuf[1];
	ResceBuf.u8[2] =  DustBuf[0];
	Tmp = ResceBuf.u32/16777215.0;
//	printf("result-K: %f\r\n",Tmp);		//תΪС������ֵ    
	Tmp = Tmp*CS5463_VScale;		//V_Coff;     //�����ѹֵ220V*250mv/(110mv/1.414)=704.8V
//    printf("��ѹ: %f\r\n",Tmp);		//תΪС������ֵ   
    Tmp *= 10;						//�Ŵ�ʮ��
	ResceBuf.u32 = (u32)Tmp;
//	memcpy(DustBuf,ResceBuf.u8,4);

	return ResceBuf.u32;
}

/*
*********************************************************************************************************
*	�� �� ��: CS546x_Get_Irms
*	����˵��: ��ȡ��Ч��������
*	��     ��: DustBuf���������������ݻ���;
*	�� �� ֵ: �� 
*	��     ע��
*
*				ʵ������
*			��������ֵ266ma�����������ѹ��6.4mv�����ۼ����������Ӧ����10.64mv����˻���������©�ţ�
*		��ϵ��Ϊ10.64/6.4=	1.6625����ϵ��ֵ��Ҫ��������ͳ�Ƶ���
*
*
*			����һ�κ�����ϵ y=kx+b;����yΪ���������˵�ѹ��xΪ�Ĵ�����ֵ/0xFFFFFF,k
*			Ϊ����ϵ����bΪƫ��8ֵ�������Ҫ���k��bֵ�����ڼĴ���ÿ�ζ�ȡ��������������Ҫ
*			���ȡֵ��ƽ��ֵ�Լ�С���
*			
*			��������KֵΪ112.74��bֵΪ3.69��������Ϊy=112.74x+3.69
*			
*			��������
*            Vres                �Ĵ�������					ת��ΪС��
*            5.8mv				04 DC DC				0.0187
*            6.4mv				06 DC 50  				0.024022
*
*
*
*            
*********************************************************************************************************
*/
u32 CS546x_Get_Irms()
{
	float Tmp;
	UN32 ResceBuf;
    u8 DustBuf[4]={0};

    
	ResceBuf.u32 = 0;
	Tmp = 0;
	CS5463ReadReg(REG_RD|Irms,DustBuf);
	ResceBuf.u8[0] =  DustBuf[2];
	ResceBuf.u8[1] =  DustBuf[1];
	ResceBuf.u8[2] =  DustBuf[0];
	Tmp = ResceBuf.u32/16777215.0;
	Tmp = Tmp*CS5463_IScale;//I_Coff;		//�������ֵ ��ʱ���� 
	Tmp *= 1000;
	ResceBuf.u32 = (u32)Tmp;
	
	return ResceBuf.u32;
}


/*
*********************************************************************************************************
*	�� �� ��: CS546x_Init
*	����˵��: CS546xоƬ��ʼ��
*	��     ��: IsReadEEP���Ƿ�ʹ�ô洢���ڱ���Ĳ�����������оƬ;1��ʹ��;0����ʹ��;
*	�� �� ֵ: �� 
*	��     ע��
*********************************************************************************************************
*/
void CS546x_Init(unsigned char IsReadEEP)
{
	CS546x_CLR_RST;
	delay_ms(10);
	CS546x_SET_RST;

	if(IsReadEEP)
	{			
		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//ͬ������

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x0f;
		CS546x_Buff.u8[2] = 0xA0;
		CS5463WriteReg(CycleCount | REG_WR,CS546x_Buff.u8); //����ÿ���������ڵ�A/Dת����->4000

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Mode| REG_WR,CS546x_Buff.u8);	//������ͨ�˲�

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x01;
		CS5463WriteReg(Cfg | REG_WR,CS546x_Buff.u8);	//���üĴ���

		CS546x_Buff.u8[0] = 0xFF;
		CS546x_Buff.u8[1] = 0xFF;
		CS546x_Buff.u8[2] = 0xFF;
		CS5463WriteReg(Status | REG_WR,CS546x_Buff.u8);//clear state reg

		CS546x_Buff.u8[0] = 0x80;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(MASK | REG_WR,CS546x_Buff.u8);	// allow int

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Ctrl | REG_WR,CS546x_Buff.u8);	//��ʼ�����ƼĴ���

		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//ͬ������

		CS546x_RESET_CS;
		delay_us(10);
		CS5463CMD(0xe8);  						//���������������ڲ���ģʽ
		CS546x_SET_CS;
	}
	else
	{			
		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8);  			//�������³�ʼ������  ->SYNC1����

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x01;
		CS5463WriteReg(Cfg | REG_WR,CS546x_Buff.u8);	//���üĴ���						

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x60;                       //���������͵�ѹ��ͨ�˲���
		CS5463WriteReg(Mode | REG_WR,CS546x_Buff.u8);	//����ģʽ�Ĵ���	
		
		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x0f;
		CS546x_Buff.u8[2] = 0xA0;
		CS5463WriteReg(CycleCount | REG_WR,CS546x_Buff.u8);	//����ÿ���������ڵ�A/Dת����->4000

		CS546x_Buff.u8[0] = 0xFF;
		CS546x_Buff.u8[1] = 0xFF;
		CS546x_Buff.u8[2] = 0xFF;
		CS5463WriteReg(Status | REG_WR,CS546x_Buff.u8);//clear state reg

		CS546x_Buff.u8[0] = 0x80;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x80;
		CS5463WriteReg(MASK | REG_WR,CS546x_Buff.u8);	// allow int

		CS546x_Buff.u8[0] = 0x00;
		CS546x_Buff.u8[1] = 0x00;
		CS546x_Buff.u8[2] = 0x00;
		CS5463WriteReg(Ctrl | REG_WR,CS546x_Buff.u8);	//��ʼ�����ƼĴ���		

		CS546x_Buff.u8[0] = SYNC1;
		CS546x_Buff.u8[1] = SYNC1;
		CS546x_Buff.u8[2] = SYNC0;
		CS5463WriteReg(SYNC1,CS546x_Buff.u8); 			//ͬ������

		CS546x_RESET_CS;
		delay_us(10);
		CS5463CMD(0xe8);  						//���������������ڲ���ģʽ,��������
		CS546x_SET_CS;
	}
}
//Channal ���ͨ��
void Get_InputValue(void)
{
	static UN32 Voltage_VA,Voltage_IA;
	u8 CS546x_Sta;
    static u8 Channal=Channal_A;
    if(Channal==Channal_A)
    {
        SetInput_CSCD4051Switch(IV_A);	//A���ѹͨ��ѡ��
        SetInput_IICD4051Switch(II_A);	//A�����ͨ��ѡ��
        delay_us(30);					//��Ҫ��ʱ
        CS546x_Sta	= CS5463_GetStaReg_Val();			//����жϲ�����ԭ��
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//����Ч����ֵ
            printf("A V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal++;
        }
    }
    else if(Channal==Channal_B)
    {
        SetInput_CSCD4051Switch(IV_B);	//B���ѹͨ��ѡ��
        SetInput_IICD4051Switch(II_B);	//B�����ͨ��ѡ��
        delay_us(30);					//��Ҫ��ʱ
        CS546x_Sta	= CS5463_GetStaReg_Val();			//����жϲ�����ԭ��
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//����Ч����ֵ
            printf("B V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal++;
        }
    }
    else if(Channal==Channal_C)
    {
        SetInput_CSCD4051Switch(IV_C);	//B���ѹͨ��ѡ��
        SetInput_IICD4051Switch(II_C);	//B�����ͨ��ѡ��
        delay_us(30);					//��Ҫ��ʱ
        CS546x_Sta	= CS5463_GetStaReg_Val();			//����жϲ�����ԭ��
        if(0x01==(CS546x_Sta&0x01))  
        {
            Voltage_VA.u32 = CS546x_Get_Vrms(); 
            Voltage_IA.u32 = CS546x_Get_Irms(); 	//����Ч����ֵ
            printf("C V:%d.%dV , I:%dmA.\r\n",Voltage_VA.u16[0]/10,Voltage_VA.u16[0]%10,Voltage_IA.u32);
            CS546x_ResetStaReg();
            Channal=Channal_A;
        }
    }
}




