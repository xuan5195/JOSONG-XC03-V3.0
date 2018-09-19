#include "stm32f10x.h"		
#include "bus_app.h"		
#include "delay.h"
#include "bsp.h"

extern uint16_t g_ShowDat[6];
extern uint16_t g_CANShowLED;
//总线业务处理
//a.显示板业务处理，RS485通信
//b.远程板业务处理，CAN通信

uint8_t RS485Dat_LED[3]={0};
//a.显示板业务处理，RS485通信
void DisplaySendDat(void)
{
	uint8_t Buff[4]={0};
    if(gAp.ErrorFlag==1)   RS485Dat_LED7_ON();
    if(gBp.ErrorFlag==1)   RS485Dat_LED8_ON();       
	Buff[0] = RS485Dat_LED[0];
	Buff[1] = RS485Dat_LED[1];
	Buff[2] = RS485Dat_LED[2];
	RS485_SendDat((uint8_t *)Buff); //LED显示,发送远程板
}

void RS485Dat_LED1_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x01;}	//远程通信指示灯
void RS485Dat_LED2_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x02;}	//主板通信指示灯
void RS485Dat_LED3_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x04;g_CANShowLED=g_CANShowLED|0x1000;}	//远程联动指示灯
void RS485Dat_LED4_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x08;g_CANShowLED=g_CANShowLED|0x0400;}	//水泵动力指示灯
void RS485Dat_LED5_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x10;g_CANShowLED=g_CANShowLED|0x2000;}	//压力工作指示灯
void RS485Dat_LED6_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x20;g_CANShowLED=g_CANShowLED|0x8000;}	//主三相电指示灯
void RS485Dat_LED7_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x40;g_CANShowLED=g_CANShowLED|0x0200;}	//主一故障指示灯
void RS485Dat_LED8_ON()		{RS485Dat_LED[0]=RS485Dat_LED[0]|0x80;g_CANShowLED=g_CANShowLED|0x0100;}	//主二故障指示灯
void RS485Dat_LED9_ON()		{RS485Dat_LED[1]=RS485Dat_LED[1]|0x01;g_CANShowLED=g_CANShowLED|0x0040;}	//断相过载指示灯
void RS485Dat_LED10_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x02;g_CANShowLED=g_CANShowLED|0x0800;}	//缺水    指示灯
void RS485Dat_LED11_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x04;}	//湿度工作指示灯
void RS485Dat_LED12_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x08;g_CANShowLED=g_CANShowLED&(~0x0020);}	//A泵停止指示灯
void RS485Dat_LED13_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x10;g_CANShowLED=g_CANShowLED|0x0020;}	//A泵低速指示灯
void RS485Dat_LED14_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x20;g_CANShowLED=g_CANShowLED|0x0020;}	//A泵高速指示灯
void RS485Dat_LED15_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x40;g_CANShowLED=g_CANShowLED|0x0008;}	//主一备二指示灯
void RS485Dat_LED16_ON()	{RS485Dat_LED[1]=RS485Dat_LED[1]|0x80;g_CANShowLED=g_CANShowLED|0x0004;}	//手动模式指示灯
void RS485Dat_LED17_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x01;g_CANShowLED=g_CANShowLED|0x0002;}	//主二备一指示灯
void RS485Dat_LED18_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x02;g_CANShowLED=g_CANShowLED&(~0x0010);}	//B泵停止指示灯
void RS485Dat_LED19_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x04;g_CANShowLED=g_CANShowLED|0x0010;}	//B泵低速指示灯
void RS485Dat_LED20_ON()	{RS485Dat_LED[2]=RS485Dat_LED[2]|0x08;g_CANShowLED=g_CANShowLED|0x0010;}	//B泵高速指示灯

void RS485Dat_LED1_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x01);}	//远程通信指示灯
void RS485Dat_LED2_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x02);}	//主板通信指示灯
void RS485Dat_LED3_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x04);g_CANShowLED=g_CANShowLED&(~0x1000);}	//远程联动指示灯
void RS485Dat_LED4_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x08);g_CANShowLED=g_CANShowLED&(~0x0400);}	//水泵动力指示灯
void RS485Dat_LED5_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x10);g_CANShowLED=g_CANShowLED&(~0x2000);}	//压力工作指示灯
void RS485Dat_LED6_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x20);g_CANShowLED=g_CANShowLED&(~0x8000);}	//主三相电指示灯
void RS485Dat_LED7_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x40);g_CANShowLED=g_CANShowLED&(~0x0200);}	//主一故障指示灯
void RS485Dat_LED8_OFF()	{RS485Dat_LED[0]=RS485Dat_LED[0]&(~0x80);g_CANShowLED=g_CANShowLED&(~0x0100);}	//主二故障指示灯
void RS485Dat_LED9_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x01);g_CANShowLED=g_CANShowLED&(~0x0040);}	//断相过载指示灯
void RS485Dat_LED10_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x02);g_CANShowLED=g_CANShowLED&(~0x0800);}	//缺水    指示灯
void RS485Dat_LED11_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x04);}	//湿度工作指示灯
void RS485Dat_LED12_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x08);}	//A泵停止指示灯
void RS485Dat_LED13_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x10);}	//A泵低速指示灯
void RS485Dat_LED14_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x20);}	//A泵高速指示灯
void RS485Dat_LED15_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x40);g_CANShowLED=g_CANShowLED&(~0x0008);}	//主一备二指示灯
void RS485Dat_LED16_OFF()	{RS485Dat_LED[1]=RS485Dat_LED[1]&(~0x80);g_CANShowLED=g_CANShowLED&(~0x0004);}	//手动模式指示灯
void RS485Dat_LED17_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x01);g_CANShowLED=g_CANShowLED&(~0x0002);}	//主二备一指示灯
void RS485Dat_LED18_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x02);}	//B泵停止指示灯
void RS485Dat_LED19_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x04);}	//B泵低速指示灯
void RS485Dat_LED20_OFF()	{RS485Dat_LED[2]=RS485Dat_LED[2]&(~0x08);}	//B泵高速指示灯

//b.远程板业务处理，CAN通信
void CanSendDat(uint8_t _ShowMode)
{
    static uint8_t SendMode=0x01;
	uint8_t Package_Dat[5]={0};
    switch (SendMode)
    {
        case 0x01:
            Package_Dat[0] = g_CANShowLED/256;   //LED高位
            Package_Dat[1] = g_CANShowLED%256;   //LED低位
            Package_Dat[2] = 0x00;
            Package_Dat[3] = 0x00;
            Package_Dat[4] = _ShowMode;
            break;
        case 0x02:
            Package_Dat[0] = g_ShowDat[0]/256;   //电压高位
            Package_Dat[1] = g_ShowDat[0]%256;   //电压低位
            Package_Dat[2] = g_ShowDat[1]/256;   //电流高位
            Package_Dat[3] = g_ShowDat[1]%256;   //电流低位
            Package_Dat[4] = _ShowMode;
            break;
        case 0x03:
            Package_Dat[0] = (uint8_t)g_ShowDat[2];   //温度
            Package_Dat[1] = (uint8_t)g_ShowDat[3];   //湿度
            Package_Dat[2] = (uint8_t)g_ShowDat[4];   //压力
            Package_Dat[3] = (uint8_t)g_ShowDat[5];   //流量            
            Package_Dat[4] = _ShowMode;
            break;
        default:
            break;
    }    
    Package_Send(SendMode,(uint8_t *)Package_Dat);
    if(SendMode==0x03)  SendMode = 0x01;
    else                SendMode++;
}
