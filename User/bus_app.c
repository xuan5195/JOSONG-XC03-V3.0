#include "stm32f10x.h"		
#include "bus_app.h"		
#include "delay.h"
#include "bsp.h"

//����ҵ����
//a.��ʾ��ҵ����RS485ͨ��
//b.Զ�̰�ҵ����CANͨ��

uint8_t RS485Dat_LED[3];
//a.��ʾ��ҵ����RS485ͨ��
void DisplaySendDat(void)
{
	uint8_t Buff[4]={0};
	Buff[0] = RS485Dat_LED[0];
	Buff[1] = RS485Dat_LED[1];
	Buff[2] = RS485Dat_LED[2];
	RS485_SendDat((uint8_t *)Buff); //LED��ʾ,����Զ�̰�
}

void RS485Dat_LED1_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x01;}	//Զ��ͨ��ָʾ��
void RS485Dat_LED2_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x02;}	//����ͨ��ָʾ��
void RS485Dat_LED3_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x04;}	//Զ������ָʾ��
void RS485Dat_LED4_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x08;}	//ˮ�ö���ָʾ��
void RS485Dat_LED5_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x10;}	//ѹ������ָʾ��
void RS485Dat_LED6_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x20;}	//�������ָʾ��
void RS485Dat_LED7_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x40;}	//��һ����ָʾ��
void RS485Dat_LED8_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x80;}	//��������ָʾ��
void RS485Dat_LED9_ON()		{RS485Dat_LED[1]=RS485Dat_LED[1]|0x01;}	//�������ָʾ��
void RS485Dat_LED10_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x02;}	//ȱˮ    ָʾ��
void RS485Dat_LED11_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x04;}	//ʪ�ȹ���ָʾ��
void RS485Dat_LED12_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x08;}	//A��ָֹͣʾ��
void RS485Dat_LED13_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x10;}	//A�õ���ָʾ��
void RS485Dat_LED14_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x20;}	//A�ø���ָʾ��
void RS485Dat_LED15_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x40;}	//��һ����ָʾ��
void RS485Dat_LED16_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x80;}	//�ֶ�ģʽָʾ��
void RS485Dat_LED17_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x01;}	//������һָʾ��
void RS485Dat_LED18_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x02;}	//B��ָֹͣʾ��
void RS485Dat_LED19_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x04;}	//B�õ���ָʾ��
void RS485Dat_LED20_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x08;}	//B�ø���ָʾ��

void RS485Dat_LED1_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x01);}	//Զ��ͨ��ָʾ��
void RS485Dat_LED2_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x02);}	//����ͨ��ָʾ��
void RS485Dat_LED3_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x04);}	//Զ������ָʾ��
void RS485Dat_LED4_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x08);}	//ˮ�ö���ָʾ��
void RS485Dat_LED5_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x10);}	//ѹ������ָʾ��
void RS485Dat_LED6_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x20);}	//�������ָʾ��
void RS485Dat_LED7_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x40);}	//��һ����ָʾ��
void RS485Dat_LED8_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x80);}	//��������ָʾ��
void RS485Dat_LED9_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x01);}	//�������ָʾ��
void RS485Dat_LED10_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x02);}	//ȱˮ    ָʾ��
void RS485Dat_LED11_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x04);}	//ʪ�ȹ���ָʾ��
void RS485Dat_LED12_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x08);}	//A��ָֹͣʾ��
void RS485Dat_LED13_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x10);}	//A�õ���ָʾ��
void RS485Dat_LED14_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x20);}	//A�ø���ָʾ��
void RS485Dat_LED15_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x40);}	//��һ����ָʾ��
void RS485Dat_LED16_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x80);}	//�ֶ�ģʽָʾ��
void RS485Dat_LED17_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x01);}	//������һָʾ��
void RS485Dat_LED18_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x02);}	//B��ָֹͣʾ��
void RS485Dat_LED19_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x04);}	//B�õ���ָʾ��
void RS485Dat_LED20_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x08);}	//B�ø���ָʾ��

//b.Զ�̰�ҵ����CANͨ��

