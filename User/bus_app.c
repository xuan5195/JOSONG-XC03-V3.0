#include "stm32f10x.h"		
#include "bus_app.h"		
#include "delay.h"
#include "bsp.h"

//总线业务处理
//a.显示板业务处理，RS485通信
//b.远程板业务处理，CAN通信

uint8_t RS485Dat_LED[3];
//a.显示板业务处理，RS485通信
void DisplaySendDat(void)
{
	uint8_t Buff[4]={0};
	Buff[0] = RS485Dat_LED[0];
	Buff[1] = RS485Dat_LED[1];
	Buff[2] = RS485Dat_LED[2];
	RS485_SendDat((uint8_t *)Buff); //LED显示,发送远程板
}

void RS485Dat_LED1_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x01;}	//远程通信指示灯
void RS485Dat_LED2_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x02;}	//主板通信指示灯
void RS485Dat_LED3_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x04;}	//远程联动指示灯
void RS485Dat_LED4_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x08;}	//水泵动力指示灯
void RS485Dat_LED5_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x10;}	//压力工作指示灯
void RS485Dat_LED6_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x20;}	//主三相电指示灯
void RS485Dat_LED7_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x40;}	//主一故障指示灯
void RS485Dat_LED8_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x80;}	//主二故障指示灯
void RS485Dat_LED9_ON()		{RS485Dat_LED[1]=RS485Dat_LED[1]|0x01;}	//断相过载指示灯
void RS485Dat_LED10_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x02;}	//缺水    指示灯
void RS485Dat_LED11_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x04;}	//湿度工作指示灯
void RS485Dat_LED12_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x08;}	//A泵停止指示灯
void RS485Dat_LED13_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x10;}	//A泵低速指示灯
void RS485Dat_LED14_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x20;}	//A泵高速指示灯
void RS485Dat_LED15_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x40;}	//主一备二指示灯
void RS485Dat_LED16_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x80;}	//手动模式指示灯
void RS485Dat_LED17_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x01;}	//主二备一指示灯
void RS485Dat_LED18_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x02;}	//B泵停止指示灯
void RS485Dat_LED19_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x04;}	//B泵低速指示灯
void RS485Dat_LED20_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x08;}	//B泵高速指示灯

void RS485Dat_LED1_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x01);}	//远程通信指示灯
void RS485Dat_LED2_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x02);}	//主板通信指示灯
void RS485Dat_LED3_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x04);}	//远程联动指示灯
void RS485Dat_LED4_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x08);}	//水泵动力指示灯
void RS485Dat_LED5_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x10);}	//压力工作指示灯
void RS485Dat_LED6_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x20);}	//主三相电指示灯
void RS485Dat_LED7_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x40);}	//主一故障指示灯
void RS485Dat_LED8_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x80);}	//主二故障指示灯
void RS485Dat_LED9_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x01);}	//断相过载指示灯
void RS485Dat_LED10_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x02);}	//缺水    指示灯
void RS485Dat_LED11_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x04);}	//湿度工作指示灯
void RS485Dat_LED12_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x08);}	//A泵停止指示灯
void RS485Dat_LED13_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x10);}	//A泵低速指示灯
void RS485Dat_LED14_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x20);}	//A泵高速指示灯
void RS485Dat_LED15_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x40);}	//主一备二指示灯
void RS485Dat_LED16_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x80);}	//手动模式指示灯
void RS485Dat_LED17_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x01);}	//主二备一指示灯
void RS485Dat_LED18_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x02);}	//B泵停止指示灯
void RS485Dat_LED19_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x04);}	//B泵低速指示灯
void RS485Dat_LED20_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[2]&(~0x08);}	//B泵高速指示灯

//b.远程板业务处理，CAN通信

