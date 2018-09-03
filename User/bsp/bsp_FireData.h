#ifndef __BSP_FireData_H
#define __BSP_FireData_H	

#define Fire_OUT1_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_0); 	   	
#define Fire_OUT1_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_0);

#define Fire_OUT2_ON()  		GPIO_SetBits(GPIOC, GPIO_Pin_12); 	   	
#define Fire_OUT2_OFF()  		GPIO_ResetBits(GPIOC, GPIO_Pin_12);

#define Fire_Test1_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_8); 	   	
#define Fire_Test1_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_8);
#define Fire_Test2_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_9); 	   	
#define Fire_Test2_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_9);
#define Fire_Test3_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_10); 	   	
#define Fire_Test3_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_10);
#define Fire_Test4_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_11); 	   	
#define Fire_Test4_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_11);
#define Fire_Test5_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_12); 	   	
#define Fire_Test5_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
#define Fire_Test6_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_13); 	   	
#define Fire_Test6_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//#define Fire_Test7_ON()  		GPIO_SetBits(GPIOD, GPIO_Pin_14); 	   	
//#define Fire_Test7_OFF()  		GPIO_ResetBits(GPIOD, GPIO_Pin_14);

#define Fire_Start1_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_7); 	   	
#define Fire_Start1_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_7);
#define Fire_Start2_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_8); 	   	
#define Fire_Start2_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_8);
#define Fire_Start3_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_9); 	   	
#define Fire_Start3_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_9);
#define Fire_Start4_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_10); 	   	
#define Fire_Start4_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_10);
#define Fire_Start5_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_11); 	   	
#define Fire_Start5_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_11);
#define Fire_Start6_ON()  		GPIO_SetBits(GPIOE, GPIO_Pin_12); 	   	
#define Fire_Start6_OFF()  		GPIO_ResetBits(GPIOE, GPIO_Pin_12);
//#define Fire_Start7_ON()  		GPIO_SetBits(GPIOA, GPIO_Pin_7); 	   	
//#define Fire_Start7_OFF()  		GPIO_ResetBits(GPIOA, GPIO_Pin_7);

void Fire_GPIO_Config(void);
void OUT_GPIO_Config(void);
u8 Test_ADC_Data(u8 TestADCCount);
void FireStart(u8 Fire_OUT_No);

#endif
