#ifndef __BSP_CS5463_H
#define __BSP_CS5463_H	

#include "stm32f10x.h"		

//CS546x IO口操作
#define CS546x_SET_RST  		GPIO_SetBits(GPIOB, GPIO_Pin_4) 	//(PBout(CS546x_RST) = 1)
#define CS546x_CLR_RST  		GPIO_ResetBits(GPIOB, GPIO_Pin_4) 	//(PBout(CS546x_RST) = 0)
#define CS546x_SET_SDI 			GPIO_SetBits(GPIOA, GPIO_Pin_7) 	//(PAout(CS546x_SDI) = 1)
#define CS546x_RESET_SDI  		GPIO_ResetBits(GPIOA, GPIO_Pin_7) 	//(PAout(CS546x_SDI) = 0)
#define CS546x_SET_CLK  		GPIO_SetBits(GPIOA, GPIO_Pin_5) 	//(PAout(CS546x_CLK) = 1)
#define CS546x_RESET_CLK  		GPIO_ResetBits(GPIOA, GPIO_Pin_5) 	//(PAout(CS546x_CLK) = 0)
#define CS546x_SET_CS   		GPIO_SetBits(GPIOB, GPIO_Pin_5) 	//(PBout(CS546x_CS) = 1)
#define CS546x_RESET_CS   		GPIO_ResetBits(GPIOB, GPIO_Pin_5) 	//(PBout(CS546x_CS) = 0)

#define CS546x_IS_INT       	GPIO_ResetBits(GPIOB, GPIO_Pin_3) 	//(PBin(CS546x_INT)) 			//如果CAR_DET为低
#define CS546x_Get_SDO    		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)


#define TransformerIScale	1.562   //互感器漏磁系数   1.6625
#define TransformerVScale	1.03   	 //电流型电压互感器误差系数

#define VScale_K			242.5765 //电压通道一次函数K值
#define VScale_b			0.01     //电压通道一次函数b值
#define IScale_K			9352.7   //电流通道一次函数K值   112.74
#define IScale_b			-5.138   //电流通道一次函数b值

#define SYNC1	0xFF
#define	SYNC0	0xFE   

#define CS5463_VScale        986	//525   //计算电压比例,220V*250mv/110mv=500V	   972
#define CS5463_IScale        9		//(250/10)    //计算电流比例

/****以下是cs5463读写寄存器****/
#define REG_WR	0x40
#define REG_RD	0x00

//Register Page 0
#define Cfg  		(0x00<<1)		//配置寄存器
#define	IdcOff		(0x01<<1)		//直流电流偏移寄存器
#define	Ign			(0x02<<1)		//电流增益
#define	VdcOff		(0x03<<1)		//直流电压偏移
#define Vgn			(0x04<<1)		//电压增益
#define	CycleCount	(0x05<<1)		//脉冲周期计数
#define	PulRateE	(0x06<<1)		//
#define	II			(0x07<<1)		//瞬时电流值
#define	IV  		(0x08<<1)		//瞬时电压值
#define	IP			(0x09<<1)		//瞬时功率
#define	Pact		(0x0A<<1)		//有效功率
#define	Irms		(0x0B<<1)		//有效电流值
#define	Vrms		(0x0C<<1)		//有效电压值
#define	Epsilon		(0x0D<<1)		//
#define	Poff		(0x0E<<1)		//功率偏移
#define	Status		(0x0F<<1)		//状态寄存器->判断设备状态，必要时 进行状态清零处理
#define	Iacoff		(0x10<<1)		//交流有效电流偏移
#define	Vacoff		(0x11<<1)		//交流有效电压偏移
#define	Mode        (0x12<<1)		//工作模式
#define	TempT       (0x13<<1)		//温度
#define	Qavg		(0x14<<1)		//平均无功功率
#define	IQ			(0x15<<1)		//瞬时无功功率
#define	Ipeak		(0x16<<1)		//峰值电流
#define	Vpeak		(0x17<<1)		//峰值电压
#define	Qtrig		(0x18<<1)		//功率三角形计算的无功功率  	 		  Reactive Power calculated from Power Triangle
#define	PF      	(0x19<<1)		//功率因素
#define	MASK		(0x1A<<1)		//中断屏蔽
#define	Pa          (0x1B<<1)		//视在功率
#define	Ctrl		(0x1C<<1)		//控制器
#define	Ph			(0x1D<<1)		//谐波有功功率
#define	Paf			(0x1E<<1)		//基波有功功率
#define	Qf			(0x1F<<1)		//基波无功


//Register Page 1
#define	PulWidth	(0x00<<1)		//电量脉冲输出宽度  		Energy Pulse Output Width
#define	LoadMin		(0x01<<1)		//未加载最小阈值			No Load Threshold
#define	Tgain		(0x02<<1)		//温度传感器增益
#define	Toff		(0x03<<1)		//温度传感器偏移

//Register Page 3
#define	VSAGdr		(0x06<<1)		//电压下降取样频率  Voltage sag sample interval
#define	VSAGlv		(0x07<<1)		//电压下降等级
#define	ISAGdr		(0x0A<<1)		//电流异常采样频率
#define	ISAGlv		(0x0B<<1)		//电流异常等级

//特别说明，禁止写未定义的寄存器

typedef union{		/* for C51 */
	u16	u16;		/* u8[0] = u16>>8; */
	u8	u8[2];		/* u8[1] = u16&0x00ff; */
	s16	s16;
	s8	s8[2];
}	UN16;

typedef union{     /* for stm32 */
	u32		u32;
	u16		u16[2];
	u8		u8[4];
	s32		s32;
	s16		s16[2];
	s8		s8[4];
}	UN32;

enum
{
	Less_phase=1,		//缺相
	Voltage_Over=2,		//过载
	Voltage_Low=4,		//欠压
	misphaseFlag=8,		//错相	
};
typedef struct 
{
	UN16 Voltage;		//有效电压
	UN16 Current;		//有效电流	
	UN16 Pactive;		//有效功率
	u16  VollackCount;	//超欠压超时计数，当计数满足时候确定超欠压时间发生
	u8   Vollacksta;	//5->超压计数时间到;4->超压计数使能;2->欠压采集完成;1->欠压计数时间到;0->欠压计数使能;
	u8 ValidCount;		//有效采集计数
	u8 InvalidCount;	//无效采集计数
	u8 DevSta;			//6->电流异常;5->断相;4->三相电压异常标记;3->错相;2->欠压;1->过载;0->缺相;    //这里注意下，除了使用5463判断过载，外部同样提供一个断相过载检测端子
}PhaseHandle;


void Bsp_CS5463_Config(void) ;
void CS546x_Init(unsigned char IsReadEEP);
void CS546x_ResetStaReg(void);
u8 CS5463_GetStaReg_Val(void);
u8 CS5463_GetStatusReg(void);
u32 CS546x_Get_Irms(void);
u32 CS546x_Get_Vrms(void);
uint8_t Get_PowerValue(uint8_t _channal,uint32_t *_ValueDat);
		 				    
#endif
