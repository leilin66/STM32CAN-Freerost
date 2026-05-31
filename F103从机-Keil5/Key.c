#include "stm32f10x.h"                  // Device header
#include "Delay.h"   

	
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_0;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
}


uint8_t Key_GetFlag(void)
{

	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)==0)
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)==0);
		Delay_ms(10);
		return 1;
	}
	else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)==0)
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)==0);
		Delay_ms(10);
		return 2;
	}
	return 0;
}


