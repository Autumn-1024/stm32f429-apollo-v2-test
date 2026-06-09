/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-8-01
 * @brief       阿波罗无屏幕测试流程
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F429开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./USMART/usmart.h"
#include "./BSP/SDRAM/sdram.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./BSP/PCF8574/pcf8574.h"
#include "./BSP/ES8388/es8388.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/SAI/sai.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"
#include "./TEST/test.h"
#include "lwip_comm.h"
#include "lwipopts.h"


int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8);        /* 设置时钟,192Mhz */
    delay_init(180);                            /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    usmart_dev.init(90);                        /* 初始化USMART */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    sdram_init();                               /* SRAM初始化 */
    lcd_init();                                 /* 初始化LCD */
    pcf8574_init();                             /* 初始化PCF8574 */
    norflash_init();                            /* 初始化nor flash */
    my_mem_init(SRAMIN);                        /* 初始化内部内存池 */
    my_mem_init(SRAMEX);                        /* 初始化外部内存池 */
    my_mem_init(SRAMCCM);                       /* 初始化CCM内存池 */
    exfuns_init();                              /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);                    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);                    /* 挂载FLASH */
    test_system();                              /* 验收流程 */
    
    while (1)
    {
        LED0_TOGGLE();
        delay_ms(1000);
    }
}
