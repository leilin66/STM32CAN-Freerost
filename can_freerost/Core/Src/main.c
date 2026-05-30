/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../icode/led/led.h"
#include "../../icode/delay/delay.h"
#include "../../icode/key/key.h"
#include "../../icode/usart/myusart.h"
#include "../../icode/adc/adc.h"
#include "../../icode/MyCAN/MyCAN.h"
#include "../../icode/DHT11/DHT11.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SensorCount 	2
#define TxMsgLedCount 	5
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t box[1] = {0};
uint32_t TxMailbox, TxMailbox1, TxMailbox2;

uint8_t CAN_IT_RxFlag;
uint8_t RxMsg_ADUsartFlag;
uint8_t ADState;
uint8_t ADprocess = 1;

uint8_t KeyNumStop;
uint8_t KeyNumLed;
uint8_t KeyStopFlag;
uint8_t KeyLedFlag;

uint8_t TimerFlag;

CAN_TxHeaderTypeDef TxMsgStartStop[] = {
{.StdId = 0x999, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_REMOTE,
		.DLC = 1, .TransmitGlobalTime = DISABLE},
{.StdId = 0x998, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_REMOTE,
		.DLC = 1, .TransmitGlobalTime = DISABLE}
};


CAN_TxHeaderTypeDef TxMsgLedArray[] = {
/*	StdId	ExtId			IDE				RTR		   DLC			Data[8]	*/
{ .StdId = 0x501, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = 1, .TransmitGlobalTime = DISABLE },

{ .StdId = 0x502, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = 1, .TransmitGlobalTime = DISABLE },

{ .StdId = 0x503, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = 1, .TransmitGlobalTime = DISABLE },

{ .StdId = 0x504, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = 1, .TransmitGlobalTime = DISABLE },

{ .StdId = 0x505, .ExtId = 0x00000000, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = 1, .TransmitGlobalTime = DISABLE },
};


uint8_t TxDataLEDArray[5][8] = {
    {0x01, 0x00, 0x00, 0x00},           // 第1帧的数据
    {0x01, 0x00, 0x00, 0x00},           // 第2帧的数据
    {0x01, 0x00, 0x00, 0x00},           // 第3帧的数据
    {0x01, 0x00, 0x00, 0x00},            // 第4帧的数据
    {0x00, 0x00, 0x00, 0x00}            // 第5帧远程帧，无数据
};



CAN_TxHeaderTypeDef	TxMsgAD1 = {
		.StdId = 0x001,
		.ExtId = 0x00000000,
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_REMOTE,
		.DLC = 1,
		.TransmitGlobalTime = DISABLE
};

CAN_TxHeaderTypeDef	TxMsgAD2 = {
		.StdId = 0x002,
		.ExtId = 0x00000000,
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_REMOTE,
		.DLC = 1,
		.TransmitGlobalTime = DISABLE
};

uint8_t TxDataAD[1][8] = {{0x00, 0x00, 0x00, 0x00}};


CAN_TxHeaderTypeDef *TxMsgADs[SensorCount] = {&TxMsgAD1, &TxMsgAD2};
uint32_t *TxMailboxes[SensorCount] = {&TxMailbox1, &TxMailbox2};
uint8_t Sensor_Index = 0;




CAN_RxHeaderTypeDef RxHeaderQueue[8];
uint8_t RxDataQueue[8][8];
uint8_t RxHead;
uint8_t RxTail;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//osThreadId_t TaskKeyHandle;
//osThreadId_t TaskSensorTxHandle;
//osThreadId_t TaskRxHandle;

osMutexId_t PrintMutexHandle;

#define PRINT_QUEUE_SIZE 32
#define PRINT_MSG_MAX_LEN 128

osMessageQueueId_t PrintQueueHandle;

typedef struct {
    char msg[PRINT_MSG_MAX_LEN];
} PrintMsg_t;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 非阻塞打印函数
void UART_Printf(const char *format, ...)
{
    PrintMsg_t printMsg;
    va_list args;
    va_start(args, format);
    vsnprintf(printMsg.msg, PRINT_MSG_MAX_LEN, format, args);
    va_end(args);

    osMessageQueuePut(PrintQueueHandle, &printMsg, 0, 0);
}

// 打印任务——用你的快速 fputc 发送
void Task_Printf(void *argument)
{
    PrintMsg_t printMsg;
    for (;;)
    {
        if (osMessageQueueGet(PrintQueueHandle, &printMsg, NULL, osWaitForever) == osOK)
        {
            char *p = printMsg.msg;
            while (*p)
            {
                while((huart1.Instance->SR & USART_SR_TXE) == 0);
                huart1.Instance->DR = *p++;
            }
        }
    }
}

void Task_KeyScan(void *argument)
{
	for (;;) {

		if (KeyStopFlag == 1) {
			KeyStopFlag = 0;
			KeyNumStop++;
			if (KeyNumStop >= 3) {
				KeyNumStop = 0;
			}
			switch (KeyNumStop) {
			case 0:
				HAL_CAN_Start(&hcan1);
				MyCAN_Tx(&hcan1, &TxMsgStartStop[0], TxDataAD, &TxMailbox, 1);
				break;
			case 1:
				HAL_CAN_Stop(&hcan1);
				MyCAN_Tx(&hcan1, &TxMsgStartStop[1], TxDataAD, &TxMailbox, 1);
				KeyNumStop = 2;
				break;
			}
		}

		if (KeyLedFlag == 1) {
			KeyLedFlag = 0;

			if (KeyNumStop == 0)  // CAN 运行时才发
					{
				MyCAN_Tx(&hcan1, &TxMsgLedArray[KeyNumLed],
						&TxDataLEDArray[KeyNumLed], &TxMailbox, 1);
				KeyNumLed++;
				if (KeyNumLed >= TxMsgLedCount) {
					KeyNumLed = 0;
				}
			}

		}

		osDelay(20);
	}
}

void Task_SensorTx(void *argument)
{
	for (;;) {
		if (KeyNumStop == 0) {
			MyCAN_Tx(&hcan1, TxMsgADs[Sensor_Index], TxDataAD,
					TxMailboxes[Sensor_Index], 1);

			Sensor_Index++;
			if (Sensor_Index >= SensorCount) {
				Sensor_Index = 0;
			}

		}

		osDelay(1000);
	}
}

void Task_CanRx(void *argument)
{
    for (;;) {
        while (RxHead != RxTail)
        {
            CAN_RxHeaderTypeDef *RxHeader = &RxHeaderQueue[RxHead];
            uint8_t *RxData = RxDataQueue[RxHead];

            if (RxHeader->IDE == CAN_ID_STD && RxHeader->RTR == CAN_RTR_DATA)
            {
                switch (RxHeader->StdId) {
                    case 0x011:
                        RxMsg_ADUsartFlag = 1;
                        ADState = 1;
                        break;
                    case 0x022:
                        RxMsg_ADUsartFlag = 1;
                        ADState = 2;
                        break;
                }
                MyCAN_RxPrintf(RxHeader, RxData);
            }

            // 移到外面！无论什么帧，都要消费掉
            RxHead = (RxHead + 1) % 8;
            osDelay(1);  // 每处理一帧让出 CPU
        }
        osDelay(10);
    }
}

int _write(int fd, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = ptr[i];
    }
    return len;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	// 初始化 DWT 用于微秒延时
//	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)box, 1);

  uint8_t msg[] = "UASRT ok\r\n";
  HAL_UART_Transmit(&huart1, msg, sizeof(msg)-1, 1000);
//      HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_BUF, 2);  // ADC_BUF 是数组名，本身就是地址
//      HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)&ADC_BUF[1], 1, DAC_ALIGN_12B_R);
//      HAL_TIM_Base_Start(&htim4);


  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

	// 创建三个任务（必须在 osKernelStart 之前）
//	const osThreadAttr_t default_attr = { .name = NULL, .priority =
//			osPriorityNormal, .stack_size = 512 };
//
//	osThreadNew(Task_KeyScan, NULL, &default_attr);
//	osThreadNew(Task_SensorTx, NULL, &default_attr);
//	osThreadNew(Task_CanRx, NULL, &default_attr);
//
//	USART_MutexHandle = osMutexNew(NULL);

//  // 创建打印队列
//  PrintQueueHandle = osMessageQueueNew(PRINT_QUEUE_SIZE, sizeof(PrintMsg_t), NULL);
//
//  // 打印任务（最低优先级）
//  const osThreadAttr_t printf_attr = { .name = "Printf", .priority = osPriorityLow, .stack_size = 512 };
//  osThreadNew(Task_Printf, NULL, &printf_attr);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

//	 创建三个任务（必须在 osKernelStart 之前）
	const osThreadAttr_t default_attr = { .name = NULL, .priority =
			osPriorityNormal, .stack_size = 512 };

	osThreadNew(Task_KeyScan, NULL, &default_attr);
	osThreadNew(Task_SensorTx, NULL, &default_attr);
	osThreadNew(Task_CanRx, NULL, &default_attr);

//	USART_MutexHandle = osMutexNew(NULL);

  // 创建打印队列
  PrintQueueHandle = osMessageQueueNew(PRINT_QUEUE_SIZE, sizeof(PrintMsg_t), NULL);

  // 打印任务（最低优先级）
  const osThreadAttr_t printf_attr = { .name = "Printf", .priority = osPriorityLow, .stack_size = 512 };
  osThreadNew(Task_Printf, NULL, &printf_attr);

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7)
	    {
	        HAL_IncTick();  // CubeMX 加的，必须保留
	    }

	if(htim == (&htim2))
		{
			TimerFlag = 1;
		}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == KEY1_Pin) {
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

		if (KEY_1()) {
			KeyStopFlag = 1;
		}

		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	}
	if (GPIO_Pin == KEY2_Pin) {
		if (KEY_2()) {
			KeyLedFlag = 1;
		}
	}
//
//	if (GPIO_Pin == KEY1_Pin)
//	    {
//	        KeyStopFlag = 1;
//	    }
//	    if (GPIO_Pin == KEY2_Pin)
//	    {
//	        KeyLedFlag = 1;
//	    }
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t RxTailNext = (RxTail + 1) % 8;
	if(RxTailNext != RxHead)					// 判断队列是否满（下一个写入位置追上了读取位置）
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeaderQueue[RxTail],RxDataQueue[RxTail]) == HAL_OK) {

//			USART1_printf("CAN ID: 0x%03lX\r\n", RxHeaderQueue[RxTail].StdId);
			RxTail = RxTailNext;
		}
	}
	else
	{
		CAN_RxHeaderTypeDef RxMsgTrash;
		uint8_t RxDataTrash[8];
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxMsgTrash, RxDataTrash);
	}

}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
