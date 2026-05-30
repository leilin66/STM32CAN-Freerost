/*
 * adc.h
 *
 *  Created on: Oct 21, 2021
 *      Author: Administrator
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#include "stm32f4xx_hal.h" //HAL库文件声明

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

#define Voltage		3300.0 //设置Vref引脚参数电压值（如3300表示3300mV=3.300V） 后缀必须有“.0”以使之浮点运算

#define ADC_LOOP1_MARK	50 //【一级滤波的采集数据数量】可选比对数量10~200（推荐50）  数值较大反应越慢
#define ADC_LOOP2_MARK	10 //【二级滤波的采集数据数量】可选比对数量10~200（推荐10）

extern uint16_t ADC_DATA_BUFF1[ADC_LOOP1_MARK];
extern uint16_t ADC_DATA_BUFF2[ADC_LOOP2_MARK];

uint16_t ADC_Read(ADC_HandleTypeDef* hadc);
uint16_t ADCgetavg1(uint16_t adc);
uint16_t ADCgetavg2(uint16_t adc);
uint16_t ADC_to_Voltage(uint16_t adc);

#endif /* ADC_ADC_H_ */
