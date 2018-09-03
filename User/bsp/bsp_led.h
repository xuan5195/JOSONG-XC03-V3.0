#ifndef __BSP_LED_H
#define __BSP_LED_H

#define LED1_OFF()  GPIO_SetBits(GPIOC, GPIO_Pin_10)  	      //通信
#define LED1_ON()  	GPIO_ResetBits(GPIOC, GPIO_Pin_10)	   		//

#define LED2_OFF()  GPIO_SetBits(GPIOA, GPIO_Pin_15)  	      //远控一启
#define LED2_ON()  	GPIO_ResetBits(GPIOA, GPIO_Pin_15) 	   		//

#define LED3_OFF()  GPIO_SetBits(GPIOC, GPIO_Pin_7)  	       	//远控二启
#define LED3_ON()  	GPIO_ResetBits(GPIOC, GPIO_Pin_7) 	   		//

#define LED4_OFF()  GPIO_SetBits(GPIOC, GPIO_Pin_6)  	       	//主一泵动作
#define LED4_ON()  	GPIO_ResetBits(GPIOC, GPIO_Pin_6) 	   		//

#define LED5_OFF()  GPIO_SetBits(GPIOC, GPIO_Pin_11)  	      //主一故障
#define LED5_ON()  	GPIO_ResetBits(GPIOC, GPIO_Pin_11) 	   		//

#define LED6_OFF()  GPIO_SetBits(GPIOB, GPIO_Pin_3)  	       	//主二故障
#define LED6_ON()  	GPIO_ResetBits(GPIOB, GPIO_Pin_3) 	   		//

#define LED7_OFF()  GPIO_SetBits(GPIOB, GPIO_Pin_5) 	       	//主一动力电
#define LED7_ON()  	GPIO_ResetBits(GPIOB, GPIO_Pin_5) 	   		//

#define LED8_OFF()  GPIO_SetBits(GPIOA, GPIO_Pin_6) 	       	//主二动力电
#define LED8_ON()  	GPIO_ResetBits(GPIOA, GPIO_Pin_6) 	   		//

#define LED9_OFF()  GPIO_SetBits(GPIOA, GPIO_Pin_7) 	       	//主二泵动作
#define LED9_ON()  	GPIO_ResetBits(GPIOA, GPIO_Pin_7) 	   		//

#define LED10_OFF() GPIO_SetBits(GPIOB, GPIO_Pin_15)  	      //允许远程控制
#define LED10_ON()  GPIO_ResetBits(GPIOB, GPIO_Pin_15) 	   		//

#define LED11_OFF() GPIO_SetBits(GPIOC, GPIO_Pin_12)  	      //主一备二
#define LED11_ON()  GPIO_ResetBits(GPIOC, GPIO_Pin_12)	   		//

#define LED12_OFF() GPIO_SetBits(GPIOD, GPIO_Pin_2) 	       	//手动
#define LED12_ON()  GPIO_ResetBits(GPIOD, GPIO_Pin_2) 	   		//

#define LED13_OFF() GPIO_SetBits(GPIOB, GPIO_Pin_4)  	       	//主二备一
#define LED13_ON()  GPIO_ResetBits(GPIOB, GPIO_Pin_4) 	   		//

#define LED14_OFF() GPIO_SetBits(GPIOB, GPIO_Pin_14) 	       	//急启灯
#define LED14_ON()  GPIO_ResetBits(GPIOB, GPIO_Pin_14) 	   		//

void BspLed_Config(void);
void LEDALL_ON(void);
void LEDALL_OFF(void);
void LEDShow(uint8_t _uDat1,uint8_t _uDat2);

#endif


