/*
 * adc.c
 *
 *  Created on: Oct 21, 2021
 *      Author: Administrator
 */

#include "adc.h"

uint16_t ADC_DATA_BUFF1[ADC_LOOP1_MARK];//ADC数值存放
uint16_t ADC_DATA_BUFF2[ADC_LOOP2_MARK];//ADC数值存放

uint16_t ADC_Read(ADC_HandleTypeDef* hadc){ //ADC采集程序（参数：ADC的句柄）
	HAL_ADC_Start(hadc);//开始ADC采集
	HAL_ADC_PollForConversion(hadc,500);//等待采集结束
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(hadc),HAL_ADC_STATE_REG_EOC)){//读取ADC完成标志位
		return HAL_ADC_GetValue(hadc);//读出ADC数值
	}
	return 0xFFFF;//当读取ADC失败时则返回0xFFFF
}

//----------------------以下是平均值滤波算法函数-----------------------//

uint16_t ADCgetavg1(uint16_t adc){ //【一级ADC滤波】读1次ADC值放入以往数组，去掉最大最小值后取平均值。
    uint16_t maxA,minA;
    uint32_t avg;
    uint8_t jA;
    avg=0;//初始状态ADC值为0
    maxA=0;minA=0xffff;//给出最大最小初始值，防止检查最大最小值时出错
    for(jA=0;jA<(ADC_LOOP1_MARK-1);jA++){//将ADC以往数据寄存器的数据位置向左移1组，以空出[0]位存放新的值
        ADC_DATA_BUFF1[(ADC_LOOP1_MARK-1)-jA] =  ADC_DATA_BUFF1[(ADC_LOOP1_MARK-2)-jA]; //寄存器的数据位置向左移1组
    }
    ADC_DATA_BUFF1[0]=adc;//[0]位存放新的ADC值
    for(jA=0;jA<ADC_LOOP1_MARK;jA++){//检查寄存器中的最大最小值，存入maxA和minA。
        if(maxA<=ADC_DATA_BUFF1[jA])maxA=ADC_DATA_BUFF1[jA];//求最大值
        if(minA>=ADC_DATA_BUFF1[jA])minA=ADC_DATA_BUFF1[jA];//求最小是
    }
    for(jA=0;jA<ADC_LOOP1_MARK;jA++){//除去最大最小值之后相加
        if(ADC_DATA_BUFF1[jA]!=maxA &&  ADC_DATA_BUFF1[jA]!=minA){//当值不是最大最小值时执行
            avg+=ADC_DATA_BUFF1[jA];//其余的值相加
        }
        if(ADC_DATA_BUFF1[jA]==maxA)maxA=0xFFFF;//如果最大值出现则清除最大值，防止多个值相同时重复处理
        if(ADC_DATA_BUFF1[jA]==minA)minA=0xFFFF;//如果最小值出现则清除最小值，防止多个值相同时重复处理
    }
    minA=avg/ADC_LOOP1_MARK;//相加后的值去掉后2位，得到稳定的值
    return  minA;          //返回平均值
}

uint16_t ADCgetavg2(uint16_t adc){ //【第二级ADC滤波】读10次一级滤波后的数据，只有10次都相同才返回最终电量数据值
	uint8_t jA;
	uint8_t cou=0;
	for(jA=1;jA<(ADC_LOOP2_MARK-1);jA++){//将ADC以往数据寄存器的数据位置向左移1组，以空出[1]位存放新的值   （注意[ADC_LOOP2_MARK-1]位用于存放最终返回数值）
		ADC_DATA_BUFF2[(ADC_LOOP2_MARK-1)-jA] = ADC_DATA_BUFF2[(ADC_LOOP2_MARK-2)-jA];//  寄存器的数据位置向左移1组
	}
	ADC_DATA_BUFF2[0]=adc;//[0]位存放新的ADC值
	for(jA=0;jA<=ADC_LOOP2_MARK-2;jA++){//比对从[0]到[ADC_LOOP2_MARK-2]
		if(ADC_DATA_BUFF2[jA]==adc){//比对数据是否相同
			cou++;//每相同一次，计数加1
			if(cou>=ADC_LOOP2_MARK-2){//当所有读出的数据都相同
				ADC_DATA_BUFF2[ADC_LOOP2_MARK-1] = adc;//将最新数据返回
				return adc;//将最新数据返回
			}
		}else{//当比对数据不同时
			cou=0;//计数清0
		}
	}
	return ADC_DATA_BUFF2[ADC_LOOP2_MARK-1];//当比对数据不同时返回之前未更新的数据
}

uint16_t ADC_to_Voltage(uint16_t adc){//ADC电压计算（参数：ADC值，返回：电压值，单位：mV）
	return Voltage/4095*adc;//Voltage是单片机Vref引脚参数电压，在adc.h文件设置（4095是12位ADC最大值常数）
}

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 部分程序代码复制自网络开源资料 如有侵权请联系我们处理
 * 洋桃电子原创程序代码部分均未声明版权 可自由复制使用 我们不对代码做任何担保
*********************************************************************************************/





