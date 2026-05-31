/*
 * usart1.c
 *
 *  Created on: Oct 20, 2021
 *      Author: Administrator
 */

#include "myusart.h"

uint8_t USART1_RX_BUF[USART1_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART1_RX_STA=0;//接收状态标记//bit15：接收完成标志，bit14：接收到0x0d，bit13~0：接收到的有效字节数目
uint8_t USART1_NewData;//当前串口中断接收的1个字节数据的缓存

//USB串口通信，使用USART1的printf函数
//调用方法：USART1_printf("123"); //向USART1发送字符123
//void USART1_printf (char *fmt, ...){
//    char buff[USART1_REC_LEN+1];//用于存放转换后的数据 [长度]
//    uint16_t i=0;
//    va_list arg_ptr;
//    va_start(arg_ptr, fmt);
//    vsnprintf(buff, USART1_REC_LEN+1, fmt,  arg_ptr);//数据转换
//    i=strlen(buff);//得出数据长度
//    if(strlen(buff)>USART1_REC_LEN)i=USART1_REC_LEN;//如果长度大于最大值，则长度等于最大值（多出部分忽略）
//    HAL_UART_Transmit(&huart1,(uint8_t  *)buff,i,0xffff);//串口发送函数（串口号，内容，数量，溢出时间）
//    va_end(arg_ptr);
//}

void USART1_printf(char *fmt, ...)
{
    char buff[USART1_REC_LEN + 1];
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vsnprintf(buff, USART1_REC_LEN + 1, fmt, arg_ptr);
    va_end(arg_ptr);

    // 用 fputc 逐字节发送，而不是 HAL_UART_Transmit
    for (uint16_t i = 0; buff[i] != '\0' && i < USART1_REC_LEN; i++)
    {
        fputc(buff[i], NULL);  // 调用你自己写的快速 fputc
    }
}

void  HAL_UART_RxCpltCallback(UART_HandleTypeDef  *huart){//串口中断回调函数
	if(huart ==&huart1){//判断中断来源（串口1：USB转串口）
//		USART1_printf("%c",USART1_NewData); //把收到的数据以 a符号变量 发送回电脑
		if((USART1_RX_STA&0x8000)==0){//接收未完成
		   if(USART1_RX_STA&0x4000){//接收到了0x0d
			   if(USART1_NewData!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
			   else USART1_RX_STA|=0x8000;   //接收完成了
		   }else{ //还没收到0X0D
			   if(USART1_NewData==0x0d)USART1_RX_STA|=0x4000;
			   else{
				  USART1_RX_BUF[USART1_RX_STA&0X3FFF]=USART1_NewData; //将收到的数据放入数组
				  USART1_RX_STA++;  //数据长度计数加1
				  if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收
			   }
		   }
		}
		HAL_UART_Receive_IT(&huart1,(uint8_t *)&USART1_NewData,1); //开启接收中断
    }
}

//int fputc(int ch, FILE *f)
//{
//    while((huart1.Instance->SR & USART_SR_TXE) == 0);
//    huart1.Instance->DR = *(uint8_t*)&ch;
//    return ch;
//}

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 部分程序代码复制自网络开源资料 如有侵权请联系我们处理
 * 洋桃电子原创程序代码部分均未声明版权 可自由复制使用 我们不对代码做任何担保
*********************************************************************************************/

