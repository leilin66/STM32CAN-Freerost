#ifndef __MYCAN_H__
#define __MYCAN_H__

#include "stm32f10x.h"                  // Device header

extern CanRxMsg MyCAN_RxMsg;
extern uint8_t MyCAN_RxFlag;

void MyCAN_Init(void);
void MyCAN_Transmit(CanTxMsg *TxMessage);



#endif
