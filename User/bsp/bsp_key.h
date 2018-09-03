#ifndef __BSP_KEY_H
#define __BSP_KEY_H	


#define Key1_Dat()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)		//进入
#define Key2_Dat()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)	//主一备二
#define Key3_Dat()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)	//手动
#define Key4_Dat()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)	//主二备一

#define EmerKey_Dat()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)	//急启按键

typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */
	KEY_1,					/* 1键 */
	KEY_2,					/* 2键 */
	KEY_3,					/* 3键 */
	KEY_4,					/* 4键 */
}KEY_ENUM;

void BspKey_Config(void);
u8 BspKey_Read(void);
		 				    
#endif
