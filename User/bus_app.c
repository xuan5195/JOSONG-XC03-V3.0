#include "stm32f10x.h"		
#include "bus_app.h"		
#include "delay.h"
#include "bsp.h"

extern uint16_t g_ShowDat[6];
extern uint16_t g_CANShowLED;
//����ҵ����
//a.��ʾ��ҵ����RS485ͨ��
//b.Զ�̰�ҵ����CANͨ��

uint8_t RS485Dat_LED[3]={0};
//a.��ʾ��ҵ����RS485ͨ��
void DisplaySendDat(void)
{
	uint8_t Buff[4]={0};
    if(gAp.ErrorFlag==1)   RS485Dat_LED7_ON();
    if(gBp.ErrorFlag==1)   RS485Dat_LED8_ON();       
	Buff[0] = RS485Dat_LED[0];
	Buff[1] = RS485Dat_LED[1];
	Buff[2] = RS485Dat_LED[2];
	RS485_SendDat((uint8_t *)Buff); //LED��ʾ,����Զ�̰�
}

void RS485Dat_LED1_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x01;}	//Զ��ͨ��ָʾ��
void RS485Dat_LED2_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x02;}	//����ͨ��ָʾ��
void RS485Dat_LED3_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x04;g_CANShowLED=g_CANShowLED|0x1000;}	//Զ������ָʾ��
void RS485Dat_LED4_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x08;g_CANShowLED=g_CANShowLED|0x0400;}	//ˮ�ö���ָʾ��
void RS485Dat_LED5_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x10;g_CANShowLED=g_CANShowLED|0x2000;}	//ѹ������ָʾ��
void RS485Dat_LED6_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x20;g_CANShowLED=g_CANShowLED|0x8000;}	//�������ָʾ��
void RS485Dat_LED7_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x40;g_CANShowLED=g_CANShowLED|0x0200;}	//��һ����ָʾ��
void RS485Dat_LED8_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x80;g_CANShowLED=g_CANShowLED|0x0100;}	//��������ָʾ��
void RS485Dat_LED9_ON()		{RS485Dat_LED[1]=RS485Dat_LED[1]|0x01;g_CANShowLED=g_CANShowLED|0x0040;}	//�������ָʾ��
void RS485Dat_LED10_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x02;g_CANShowLED=g_CANShowLED|0x0800;}	//ȱˮ    ָʾ��
void RS485Dat_LED11_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x04;}	//ʪ�ȹ���ָʾ��
void RS485Dat_LED12_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x08;g_CANShowLED=g_CANShowLED&(~0x0020);}	//A��ָֹͣʾ��
void RS485Dat_LED13_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x10;g_CANShowLED=g_CANShowLED|0x0020;}	//A�õ���ָʾ��
void RS485Dat_LED14_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x20;g_CANShowLED=g_CANShowLED|0x0020;}	//A�ø���ָʾ��
void RS485Dat_LED15_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x40;g_CANShowLED=g_CANShowLED|0x0008;}	//��һ����ָʾ��
void RS485Dat_LED16_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x80;g_CANShowLED=g_CANShowLED|0x0004;}	//�ֶ�ģʽָʾ��
void RS485Dat_LED17_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x01;g_CANShowLED=g_CANShowLED|0x0002;}	//������һָʾ��
void RS485Dat_LED18_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x02;g_CANShowLED=g_CANShowLED&(~0x0010);}	//B��ָֹͣʾ��
void RS485Dat_LED19_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x04;g_CANShowLED=g_CANShowLED|0x0010;}	//B�õ���ָʾ��
void RS485Dat_LED20_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x08;g_CANShowLED=g_CANShowLED|0x0010;}	//B�ø���ָʾ��

void RS485Dat_LED1_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x01);}	//Զ��ͨ��ָʾ��
void RS485Dat_LED2_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x02);}	//����ͨ��ָʾ��
void RS485Dat_LED3_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x04);g_CANShowLED=g_CANShowLED&(~0x1000);}	//Զ������ָʾ��
void RS485Dat_LED4_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x08);g_CANShowLED=g_CANShowLED&(~0x0400);}	//ˮ�ö���ָʾ��
void RS485Dat_LED5_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x10);g_CANShowLED=g_CANShowLED&(~0x2000);}	//ѹ������ָʾ��
void RS485Dat_LED6_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x20);g_CANShowLED=g_CANShowLED&(~0x8000);}	//�������ָʾ��
void RS485Dat_LED7_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x40);g_CANShowLED=g_CANShowLED&(~0x0200);}	//��һ����ָʾ��
void RS485Dat_LED8_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x80);g_CANShowLED=g_CANShowLED&(~0x0100);}	//��������ָʾ��
void RS485Dat_LED9_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x01);g_CANShowLED=g_CANShowLED&(~0x0040);}	//�������ָʾ��
void RS485Dat_LED10_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x02);g_CANShowLED=g_CANShowLED&(~0x0800);}	//ȱˮ    ָʾ��
void RS485Dat_LED11_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x04);}	//ʪ�ȹ���ָʾ��
void RS485Dat_LED12_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x08);}	//A��ָֹͣʾ��
void RS485Dat_LED13_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x10);}	//A�õ���ָʾ��
void RS485Dat_LED14_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x20);}	//A�ø���ָʾ��
void RS485Dat_LED15_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x40);g_CANShowLED=g_CANShowLED&(~0x0008);}	//��һ����ָʾ��
void RS485Dat_LED16_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x80);g_CANShowLED=g_CANShowLED&(~0x0004);}	//�ֶ�ģʽָʾ��
void RS485Dat_LED17_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x01);g_CANShowLED=g_CANShowLED&(~0x0002);}	//������һָʾ��
void RS485Dat_LED18_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x02);}	//B��ָֹͣʾ��
void RS485Dat_LED19_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x04);}	//B�õ���ָʾ��
void RS485Dat_LED20_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x08);}	//B�ø���ָʾ��

//b.Զ�̰�ҵ����CANͨ��
void CanSendDat(uint8_t _ShowMode)
{
    static uint8_t SendMode=0x01;
	uint8_t Package_Dat[5]={0};
    switch (SendMode)
    {
        case 0x01:
            Package_Dat[0] = g_CANShowLED/256;   //LED��λ
            Package_Dat[1] = g_CANShowLED%256;   //LED��λ
            Package_Dat[2] = 0x00;
            Package_Dat[3] = 0x00;
            Package_Dat[4] = _ShowMode;
            break;
        case 0x02:
            Package_Dat[0] = g_ShowDat[0]/256;   //��ѹ��λ
            Package_Dat[1] = g_ShowDat[0]%256;   //��ѹ��λ
            Package_Dat[2] = g_ShowDat[1]/256;   //������λ
            Package_Dat[3] = g_ShowDat[1]%256;   //������λ
            Package_Dat[4] = _ShowMode;
            break;
        case 0x03:
            Package_Dat[0] = (uint8_t)g_ShowDat[2];   //�¶�
            Package_Dat[1] = (uint8_t)g_ShowDat[3];   //ʪ��
            Package_Dat[2] = (uint8_t)g_ShowDat[4];   //ѹ��
            Package_Dat[3] = (uint8_t)g_ShowDat[5];   //����            
            Package_Dat[4] = _ShowMode;
            break;
        default:
            break;
    }    
    Package_Send(SendMode,(uint8_t *)Package_Dat);
    if(SendMode==0x03)  SendMode = 0x01;
    else                SendMode++;
}
