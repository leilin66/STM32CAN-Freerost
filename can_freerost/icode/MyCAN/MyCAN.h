/*
 * MyCAN.h
 *
 *  Created on: 2026Äê5ÔÂ20ÈÕ
 *      Author: 123
 */

#ifndef MYCAN_MYCAN_H_
#define MYCAN_MYCAN_H_

void MyCAN_Tx(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *TxMsgArray, uint8_t TxDataArray[][8], uint32_t *TxMailbox, uint8_t Leng);
HAL_StatusTypeDef MyCAN_Rx(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData);
void MyCAN_RxPrintf(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData);

#endif /* MYCAN_MYCAN_H_ */
