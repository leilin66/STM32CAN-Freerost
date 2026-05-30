/*
 * usart1.h
 *
 *  Created on: Oct 20, 2021
 *      Author: Administrator
 */

#ifndef INC_MYUSART_H_
#define INC_MYUSART_H_

#include "stm32f4xx_hal.h" //HAL库文件声明
#include <string.h>//用于字符串处理的库
#include <stdarg.h>
#include <stdlib.h>
#include "stdio.h"

extern UART_HandleTypeDef huart1;//声明USART1的HAL库结构体

#define USART1_REC_LEN  200//定义USART1最大接收字节数

extern uint8_t  USART1_RX_BUF[USART1_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern uint16_t USART1_RX_STA;//接收状态标记
extern uint8_t USART1_NewData;//当前串口中断接收的1个字节数据的缓存

void USART1_printf (char *fmt, ...);
void  HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);//串口中断回调函数声明

#endif /* INC_MYUSART_H_ */

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 部分程序代码复制自网络开源资料 如有侵权请联系我们处理
 * 洋桃电子原创程序代码部分均未声明版权 可自由复制使用 我们不对代码做任何担保
*********************************************************************************************/

