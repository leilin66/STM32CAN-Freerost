#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "OLED.h"
#include "Delay.h"
#include "MyCAN.h"
#include "AD.h"

#define LEDSum	4
#define TxMsgADId	0x011		//从机的ID	【0x011为光敏电阻从机	0x22为热敏电阻从机】
#define RxMsgADId	0x001		//从机接收到是否发送AD数据的指令，接收到0x001数据帧代表0x011光敏电阻从机向主机发送一次AD的测量数据	以此类推

uint8_t LEDNum;


//从机初始化
CanTxMsg TxMsgAD = {
/*	StdId	ExtId			IDE				RTR		   		DLC			Data[8]	*/
	TxMsgADId, 0x00000000, CAN_Id_Standard, CAN_RTR_Data, 	2, 			{0x00, 0x00},
};


uint16_t AD_Value;

uint8_t LEDState = 1;
uint8_t RxMsgLEDNum = 1;

int main(void)
{
	OLED_Init();
	MyCAN_Init();
	AD_Init();
	LED_Init();
	
	OLED_ShowString(1, 1, "TXID:");
	OLED_ShowHexNum(1, 6, TxMsgADId, 3);
	OLED_ShowString(2, 1, "RXID:");
	OLED_ShowString(3, 1, "Leng:");
	OLED_ShowString(4, 1, "Data:");
	
	while(1)
	{
		
		
		if(MyCAN_RxFlag == 1)
		{
			MyCAN_RxFlag = 0;
			
			if(MyCAN_RxMsg.StdId == 0x999 && MyCAN_RxMsg.IDE == CAN_Id_Standard && MyCAN_RxMsg.RTR == CAN_RTR_Remote)
			{
				CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Normal);
				LED_Set(0, 1);
			}
			
			if(MyCAN_RxMsg.StdId == 0x998 && MyCAN_RxMsg.IDE == CAN_Id_Standard && MyCAN_RxMsg.RTR == CAN_RTR_Remote)
			{
				CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Initialization);
				LED_Set(0, 0);
			}
			
			if(MyCAN_RxMsg.StdId == RxMsgADId && MyCAN_RxMsg.IDE == CAN_Id_Standard && MyCAN_RxMsg.RTR == CAN_RTR_Remote)
			{
				AD_Value = AD_GetValue();
				OLED_ShowNum(3, 10, AD_Value, 4);
				TxMsgAD.Data[0] = AD_Value >> 8;
				TxMsgAD.Data[1] = AD_Value;
				MyCAN_Transmit(&TxMsgAD);
			}
			
			
			// 判断收到的是不是 LED 控制帧（0x501 ~ 0x505）
			if(MyCAN_RxMsg.StdId >= 0x501 && MyCAN_RxMsg.StdId <= 0x505 
			   && MyCAN_RxMsg.IDE == CAN_Id_Standard 
			   && MyCAN_RxMsg.RTR == CAN_RTR_Data)    // 数据帧
			{
				LEDNum = MyCAN_RxMsg.StdId - 0x501;    // 0x501→LED0, 0x502→LED1, ...
				
				if(LEDNum == 0x04)  					// 全灭
				{
					GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
				}
				else if(LEDNum <= 0x03)					//从LED0到LED3 一个一个亮
				{
					LED_Set(LEDNum, LEDState);
					if(LEDNum > 0)
					{
						LED_Set(LEDNum - 1, 0);			//上一次LED熄灭
					}
				}
				// 更多状态（闪烁等）可以在这里扩展
			}
			
			
			if(MyCAN_RxMsg.IDE == CAN_Id_Standard)
			{
				OLED_ShowHexNum(2, 6, MyCAN_RxMsg.StdId, 8);
			}
			
			else if(MyCAN_RxMsg.IDE == CAN_Id_Extended)
			{
				OLED_ShowHexNum(2, 6, MyCAN_RxMsg.ExtId, 8);
			}
			
			
			if(MyCAN_RxMsg.RTR == CAN_RTR_Data)
			{
				OLED_ShowString(1, 10, "Data  ");
				
				OLED_ShowHexNum(3, 6, MyCAN_RxMsg.DLC, 1);
				
				OLED_ShowHexNum(4, 6, MyCAN_RxMsg.Data[0], 2);
				OLED_ShowHexNum(4, 9, MyCAN_RxMsg.Data[1], 2);
				OLED_ShowHexNum(4, 12, MyCAN_RxMsg.Data[2], 2);
				OLED_ShowHexNum(4, 15, MyCAN_RxMsg.Data[3], 2);
			}
			else if(MyCAN_RxMsg.RTR == CAN_RTR_Remote)
			{
				OLED_ShowString(1, 10, "Remote");
				
				OLED_ShowHexNum(3, 6, MyCAN_RxMsg.DLC, 1);
				
				OLED_ShowHexNum(4, 6, 0x00, 2);
				OLED_ShowHexNum(4, 9, 0x00, 2);
				OLED_ShowHexNum(4, 12, 0x00, 2);
				OLED_ShowHexNum(4, 15, 0x00, 2);
			}
			
		}
	}
	
}
