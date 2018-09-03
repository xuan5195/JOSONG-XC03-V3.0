#ifndef __BSP_SHOW_H
#define __BSP_SHOW_H	

#define Led_SCLK_ON()   	GPIO_SetBits(GPIOA, GPIO_Pin_12 );  	   		//Lcd_SCLK
#define Led_SCLK_OFF()  	GPIO_ResetBits(GPIOA, GPIO_Pin_12 ); 	   		//

#define Led_DAT_ON()   		GPIO_SetBits(GPIOA, GPIO_Pin_11 );  	   		//Lcd_DATA
#define Led_DAT_OFF()  		GPIO_ResetBits(GPIOA, GPIO_Pin_11 ); 	   		//

void Display_GPIO_Configuration(void);
//void Display_Show(u8 Display_ShowDate1,u8 Display_ShowDate2);
//void LED_Show(u16 Led_ShowDate);
void Show(u8 FirstDat,u8 SecondDat);
void WriteFaulArray(u8 Detection_TempFaultData,u8 Detection_FireFaultData,u8 Detection_PressureData);
void WriteTimeCableNo(u8 Detection_TimeCableData);
		 				    
#endif
