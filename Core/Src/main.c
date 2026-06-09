/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @version     V1.0
 * @date        2022-8-01
 * @brief       STM32F429 阿波罗V2验收工程(带LCD屏幕版)
 * @license     Copyright (c) 2020-2032, 正点原子(广州)电子信息科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 V2 F429开发板
 *
 ****************************************************************************************************
 */

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "./Middlewares/SYSTEM/sys/sys.h"
#include "./Middlewares/SYSTEM/usart/usart.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./BSP/Inc/bsp_led.h"
#include "./BSP/Inc/bsp_lcd.h"
#include "./BSP/Inc/bsp_key.h"
#include "./BSP/Inc/bsp_sdram.h"
#include "./BSP/Inc/bsp_24cxx.h"
#include "./BSP/Inc/bsp_adc.h"
#include "./BSP/Inc/bsp_remote.h"
#include "./BSP/Inc/bsp_ap3216c.h"
#include "./BSP/Inc/bsp_st480mc.h"
#include "./BSP/Inc/bsp_sh3001.h"
#include "./BSP/Inc/bsp_pcf8574.h"
#include "./App/Inc/app_test.h"

/**
 * @brief       主函数
 * @param       无
 * @retval      无
 */
int main(void)
{
    HAL_Init();                                     /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8);            /* 设置时钟,180MHz */
    delay_init(180);                                /* 延时初始化 */
    usart_init(115200);                             /* 串口初始化为115200 */
    bsp_led_init();                                 /* 初始化LED */
    bsp_key_init();                                 /* 初始化按键 */
    pcf8574_init();                                 /* 初始化PCF8574(蜂鸣器IO扩展) */
    sdram_init();                                   /* SDRAM初始化 */
    bsp_lcd_init();                                 /* 初始化LCD */

    APP_Test_Run();                                 /* 执行验收测试 */

    while (1)
    {
        LED0_TOGGLE();
        delay_ms(1000);
    }
}
