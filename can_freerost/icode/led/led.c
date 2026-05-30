/*
 * led.c
 *
 *  Created on: Oct 22, 2021
 *      Author: Administrator
 */
#include "stm32f4xx_hal.h"
#include "led.h"


void LED_1(uint8_t a)//LED1锟斤拷锟斤拷锟斤拷锟狡猴拷锟斤拷锟斤拷0为熄锟斤拷锟斤拷锟斤拷值为锟斤拷锟斤拷锟斤拷
{
	if(a)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
}
void LED_2(uint8_t a)//LED2锟斤拷锟斤拷锟斤拷锟狡猴拷锟斤拷锟斤拷0为熄锟斤拷锟斤拷锟斤拷值为锟斤拷锟斤拷锟斤拷
{
	if(a)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
}
void LED_3(uint8_t a)//LED3锟斤拷锟斤拷锟斤拷锟狡猴拷锟斤拷锟斤拷0为熄锟斤拷锟斤拷锟斤拷值为锟斤拷锟斤拷锟斤拷
{
	if(a)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
}
void LED_4(uint8_t a)//LED4锟斤拷锟斤拷锟斤拷锟狡猴拷锟斤拷锟斤拷0为熄锟斤拷锟斤拷锟斤拷值为锟斤拷锟斤拷锟斤拷
{
	if(a)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}
void LED_ALL(uint8_t a)//LED1~4锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�4位锟斤拷1/0状态锟斤拷应4锟斤拷LED锟斤拷锟斤拷锟斤拷锟轿伙拷锟接ED1锟斤拷
{
	if(a&0x01)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	if(a&0x02)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	if(a&0x04)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	if(a&0x08)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}
void LED_1_Contrary(void){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1-HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1));
}
void LED_2_Contrary(void){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,1-HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2));
}
void LED_3_Contrary(void){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,1-HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3));
}
void LED_4_Contrary(void){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1-HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4));
}
