#include "stm32f10x.h"                  // Device header


void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_Initstructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
}

void LED_Set(uint8_t LED_Num, uint8_t LED_State)
{
	switch(LED_Num)
	{
		case 0:
		{
			switch(LED_State)
			{
				case 0:GPIO_SetBits(GPIOA,GPIO_Pin_1);break;
				case 1:GPIO_ResetBits(GPIOA,GPIO_Pin_1);break;
			}
			break;
		}
		
		case 1:
		{	
			switch(LED_State)
			{
				case 0:GPIO_SetBits(GPIOA,GPIO_Pin_2);break;
				case 1:GPIO_ResetBits(GPIOA,GPIO_Pin_2);break;
			}
			break;
		}
		
		case 2:
		{	
			switch(LED_State)
			{
				case 0:GPIO_SetBits(GPIOA,GPIO_Pin_3);break;
				case 1:GPIO_ResetBits(GPIOA,GPIO_Pin_3);break;
			}
			break;
		}
		case 3:
		{	
			switch(LED_State)
			{
				case 0:GPIO_SetBits(GPIOA,GPIO_Pin_4);break;
				case 1:GPIO_ResetBits(GPIOA,GPIO_Pin_4);break;
			}
			break;
		}
	}

}

void Key_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	}
}
