#include "stm32f4xx_hal.h"
#include "../../icode/usart/myusart.h"
#include "cmsis_os.h"
#include <stdio.h>

extern uint8_t RxMsg_ADUsartFlag;
extern uint8_t ADState;
uint16_t light_value;

void MyCAN_Tx(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *TxMsgArray, uint8_t TxDataArray[][8], uint32_t *TxMailbox, uint8_t Leng)
{
	for (int i = 0; i < Leng; i++) {
		if (HAL_CAN_AddTxMessage(hcan, &TxMsgArray[i], TxDataArray[i], &TxMailbox[i]) == HAL_OK) {
			printf("第 %d 帧发送成功, Mailbox: %lu\r\n", i + 1, TxMailbox[i]);
		} else {
			printf("第 %d 帧发送失败\r\n", i + 1);
		}
	}
}

HAL_StatusTypeDef MyCAN_Rx(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData)
{
	if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0) {
		// 取出一帧数据
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, RxHeader, RxData)== HAL_OK) {
			// 打印帧信息
			if (RxHeader->IDE == CAN_ID_STD) {
				printf("标准帧 ");
			} else {
				printf("扩展帧 ");
			}

			if (RxHeader->RTR == CAN_RTR_REMOTE) {
				printf("远程帧");
			} else {
				printf("数据帧");
			}

			printf(" ID:0x%03lX DLC:%ld 数据:", RxHeader->StdId,
					RxHeader->DLC);

			for (int i = 0; i < RxHeader->DLC; i++) {
				printf("%02X ", RxData[i]);
			}
			printf("\r\n");

			return HAL_OK;
		}
	}
	return HAL_ERROR;
}

void MyCAN_RxPrintf(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData)
{
	if (RxHeader->IDE == CAN_ID_STD) {
		printf("标准帧 ");
	} else {
		printf("扩展帧 ");
	}

	if (RxHeader->RTR == CAN_RTR_REMOTE) {
		printf("远程帧");
	} else {
		printf("数据帧");
	}

	printf(" ID:0x%03lX DLC:%ld 数据:", RxHeader->StdId, RxHeader->DLC);

	if (RxMsg_ADUsartFlag == 1) {
		RxMsg_ADUsartFlag = 0;
		light_value = (RxData[0] << 8) | RxData[1];

		// 打印还原后的值
		if(ADState == 1){
			printf("光敏值 = %d\r\n", light_value);
		}
		if (ADState == 2) {
			printf("热敏值 = %d\r\n", light_value);
		}
	}

	else {
		for (int i = 0; i < RxHeader->DLC; i++) {
			printf("%02X ", RxData[i]);
		}
	}

	printf("\r\n");
}



