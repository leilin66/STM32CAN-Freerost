/*
 * key.c
 *
 *  Created on: Oct 22, 2021
 *      Author: Administrator
 */

#include "key.h"
#include "../../icode/delay/delay.h"

uint8_t KEY_1(void)
{
	uint8_t a;
	a=0;//如果未进入按键处理，则返回0
	if(HAL_GPIO_ReadPin(GPIOE,KEY1_Pin)==GPIO_PIN_RESET){//读按键接口的电平
//		HAL_Delay(20);//延时去抖动
		delay_us(20000);
		if(HAL_GPIO_ReadPin(GPIOE,KEY1_Pin)==GPIO_PIN_RESET){ //读按键接口的电平
			a=1;//进入按键处理，返回1
		}
	}
	while(HAL_GPIO_ReadPin(GPIOE,KEY1_Pin)==GPIO_PIN_RESET); //等待按键松开
	return a;
}

uint8_t KEY_2(void)
{
	uint8_t a;
	a=0;//如果未进入按键处理，则返回0
	if(HAL_GPIO_ReadPin(GPIOE,KEY2_Pin)==GPIO_PIN_RESET){//读按键接口的电平
		delay_us(20000);
		if(HAL_GPIO_ReadPin(GPIOE,KEY2_Pin)==GPIO_PIN_RESET){ //读按键接口的电平
			a=1;//进入按键处理，返回1
		}
	}
	while(HAL_GPIO_ReadPin(GPIOE,KEY2_Pin)==GPIO_PIN_RESET); //等待按键松开
	return a;
}

uint8_t KEY_3(void)
{
	uint8_t a;
	a=0;//如果未进入按键处理，则返回0
	if(HAL_GPIO_ReadPin(GPIOE,KEY3_Pin)==GPIO_PIN_RESET){//读按键接口的电平
		HAL_Delay(20);//延时去抖动
		if(HAL_GPIO_ReadPin(GPIOE,KEY3_Pin)==GPIO_PIN_RESET){ //读按键接口的电平
			a=1;//进入按键处理，返回1
		}
	}
	while(HAL_GPIO_ReadPin(GPIOE,KEY3_Pin)==GPIO_PIN_RESET); //等待按键松开
	return a;
}

uint8_t KEY_4(void)
{
	uint8_t a;
	a=0;//如果未进入按键处理，则返回0
	if(HAL_GPIO_ReadPin(GPIOE,KEY4_Pin)==GPIO_PIN_RESET){//读按键接口的电平
		HAL_Delay(20);//延时去抖动
		if(HAL_GPIO_ReadPin(GPIOE,KEY4_Pin)==GPIO_PIN_RESET){ //读按键接口的电平
			a=1;//进入按键处理，返回1
		}
	}
	while(HAL_GPIO_ReadPin(GPIOE,KEY4_Pin)==GPIO_PIN_RESET); //等待按键松开
	return a;
}

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 部分程序代码复制自网络开源资料 如有侵权请联系我们处理
 * 洋桃电子原创程序代码部分均未声明版权 可自由复制使用 我们不对代码做任何担保
*********************************************************************************************/
