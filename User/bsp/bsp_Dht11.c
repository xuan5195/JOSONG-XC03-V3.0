#include "stm32f10x.h"
#include "bsp_Dht11.h"
#include "bsp.h"


//复位DHT11
static void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//设置为输出
	DHT11_DQ_Low(); 	//拉低DQ
	delay_ms(20);    	//拉低至少18ms
	DHT11_DQ_High(); 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}

/*
*********************************************************************************************************
*	函 数 名: DHT11_Check
*	功能说明: 温湿度设备在线检测
*	形     参: 无
*	返 回 值: 0,存在;1,未检测到DHT11的存在 
*	备     注：
*********************************************************************************************************
*/
static uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;	
	DHT11_IO_IN();      //设置为输入	 
	while (DHT11_DQ_IN()&&(retry<110))//DHT11会拉低83us
	{
		retry++;
		delay_us(1);
	}	 
	if(retry>=100)	return 1;
	else 			retry=0;

	while ((!DHT11_DQ_IN())&&(retry<100))//DHT11拉低后会再次拉高87us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)	return 1;	    
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: DHT11_Read_Bit
*	功能说明: 从DHT11读取一个位
*	形     参: 无
*	返 回 值: 1/0 
*	备     注：54 微秒的低电平和 23-27 微秒的高电平，位数据“1”的格式为： 54 微秒的低电平加68-
*				74微秒的高电平
*********************************************************************************************************
*/
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;	
	while(DHT11_DQ_IN()&&(retry<100))	//等待变为低电平，即等待进入54us低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN()&&(retry<100))	//等待变高电平,之后根据高电平时间判断数据是1还是0
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);		//等待48us，该值不宜取大，以防跳出下一位开始时间
	if(DHT11_DQ_IN())return 1;
	else return 0;
}
/*
*********************************************************************************************************
*	函 数 名: DHT11_Read_Byte
*	功能说明: 从DHT11读取一个字节
*	形     参: 无
*	返 回 值: 读到的数据 
*	备     注：
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
*	函 数 名: DHT11_Read_Data
*	功能说明: 温湿度检测
*	形     参: temp->温度数据，humi->湿度数据
*	返 回 值: 0,正常;1,读取失败 
*	备     注：temp:温度值(范围:0~50°)，humi:湿度值(范围:20%~90%)
*********************************************************************************************************
*/
uint8_t DHT11_Read_Data(uint8_t *buf)    
{        
	uint8_t i;
	
	DHT11_Rst();
	if(DHT11_Check()==0)
	{	
		for(i=0;i<5;i++)//读取40位数据
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
	for(i=0;i<5;i++)//读取40位数据
	{
		buf[i]=DHT11_Read_Byte();			
	}
	if(buf[4] != ((uint8_t)(buf[0]+buf[1]+buf[2]+buf[3])))
			printf("Read Date Check Error!\r\n");
	else	printf("H:%d.%d; T:%d.%d.\r\n",buf[0],buf[1],buf[2],buf[3]);
}

