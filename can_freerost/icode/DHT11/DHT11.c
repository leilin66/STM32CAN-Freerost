#include "stm32f4xx_hal.h"



// 微秒延时函数
void DWT_Delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    us *= (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < us);
}

#include "dht11.h"

// 引脚定义
#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_6

// 引脚模式切换
static void DHT11_Mode_Out(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_Mode_In(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// 复位DHT11，检查是否存在
static uint8_t DHT11_Reset(void) {
    uint8_t retry = 0;
    DHT11_Mode_Out();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(25);                    // 改成25ms，确保超过18ms
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    DWT_Delay_us(40);                 // 改成40us
    DHT11_Mode_In();
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        if (++retry > 100) return 1;
        DWT_Delay_us(1);
    }
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        if (++retry > 100) return 1;
        DWT_Delay_us(1);
    }
    return 0;
}

// 读取一个字节
static uint8_t DHT11_ReadByte(void)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte <<= 1;

        // 等待50us低电平结束（数据位开始的标志）
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);

        // 延时48us，跳过"0"的高电平时间（26-28us）
        DWT_Delay_us(45);

        // 45us后如果还是高电平，说明是"1"（70us高电平）
        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        {
            byte |= 1;
        }

        // 等待高电平结束（如果是"1"，等它拉低；如果是"0"，已经拉低了）
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);
    }
    return byte;
}

// 读取温湿度
uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi) {
    uint8_t buf[5] = {0};

    // 1. 主机发起通信
    if (DHT11_Reset() != 0) return 1; // 设备无响应

    // 2. 读取5个字节数据
    buf[0] = DHT11_ReadByte(); // 湿度整数
    buf[1] = DHT11_ReadByte(); // 湿度小数（通常为0）
    buf[2] = DHT11_ReadByte(); // 温度整数
    buf[3] = DHT11_ReadByte(); // 温度小数（通常为0）
    buf[4] = DHT11_ReadByte(); // 校验和

    // 3. 【关键修复】通信完成后，主机必须释放总线，回到空闲状态
    DHT11_Mode_Out();                                    // 切回输出模式
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET); // 输出高电平，释放总线

    // 4. 数据校验
    if ((buf[0] + buf[1] + buf[2] + buf[3]) != buf[4]) return 2; // 校验错

    *humi = buf[0];
    *temp = buf[2];
    return 0;
}

// 硬件测试函数：引脚高低电平交替输出
void DHT11_HardwareTest(void)
{
    DHT11_Mode_Out();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    HAL_Delay(5000);  // 5秒高电平

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(5000);  // 5秒低电平
}

