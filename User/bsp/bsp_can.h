#ifndef __BSP_CAN_H
#define __BSP_CAN_H	 

#include "stm32f10x.h"

//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	1		//0,不使能;1,使能.								    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据
u8 Can_Receive_Msg(u8 *buf);							//接收数据
void Package_Send(u8 _mode,u8 *Package_Dat);
void Read_PageLogDat(uint16_t _Page);   //按页读取并通过CAN发送_Page页内80条数据
void Read_NoLogDat(uint16_t _No);       //按条数读取，CAN发送当前条目数

#endif

















