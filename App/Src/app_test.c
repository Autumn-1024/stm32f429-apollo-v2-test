/**
 ****************************************************************************************************
 * @file        app_test.c
 * @author      ALIENTEK
 * @version     V1.0
 * @date        2022-8-01
 * @brief       验收测试逻辑
 *              11项验收测试，每项在LCD显示结果，同时串口输出
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 V2 F429开发板
 *
 ****************************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "./App/Inc/app_test.h"
#include "./Middlewares/SYSTEM/sys/sys.h"
#include "./Middlewares/SYSTEM/usart/usart.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./BSP/Inc/bsp_led.h"
#include "./BSP/Inc/bsp_key.h"
#include "./BSP/Inc/bsp_lcd.h"
#include "./BSP/Inc/bsp_sdram.h"
#include "./BSP/Inc/bsp_24cxx.h"
#include "./BSP/Inc/bsp_adc.h"
#include "./BSP/Inc/bsp_remote.h"
#include "./BSP/Inc/bsp_ap3216c.h"
#include "./BSP/Inc/bsp_st480mc.h"
#include "./BSP/Inc/bsp_sh3001.h"
#include "./BSP/Inc/bsp_pcf8574.h"
#include "./BSP/Inc/bsp_imu.h"

/******************************************************************************************/
/* 测试计数与结果显示 */

static uint16_t g_test_y = 10;     /* 当前显示Y坐标 */
static uint16_t g_test_pass = 0;   /* 通过数 */
static uint16_t g_test_fail = 0;   /* 失败数 */

/**
 * @brief       显示测试结果
 * @param       name: 测试名称
 * @param       result: 0=FAIL, 1=PASS
 * @retval      无
 */
static void test_show_result(char *name, uint8_t result)
{
    if (result)
    {
        g_test_pass++;
        lcd_show_string(10, g_test_y, 200, 16, 16, name, WHITE);
        lcd_show_string(220, g_test_y, 100, 16, 16, "PASS", GREEN);
        printf("[%s] PASS\r\n", name);
    }
    else
    {
        g_test_fail++;
        lcd_show_string(10, g_test_y, 200, 16, 16, name, WHITE);
        lcd_show_string(220, g_test_y, 100, 16, 16, "FAIL", RED);
        printf("[%s] FAIL\r\n", name);
    }
    g_test_y += 20;
}

/**
 * @brief       显示测试标题
 * @param       str: 标题字符串
 * @retval      无
 */
static void test_show_title(char *str)
{
    g_test_y += 10;
    lcd_show_string(10, g_test_y, 300, 24, 24, str, YELLOW);
    g_test_y += 30;
}

/**
 * @brief       测试1: LCD显示测试
 * @retval      无
 */
static void test_lcd(void)
{
    uint16_t i;
    uint16_t color_table[] = {WHITE, BLACK, RED, GREEN, BLUE,
                              MAGENTA, YELLOW, CYAN, BROWN, GRAY};
    char *color_name[] = {"WHITE", "BLACK", "RED", "GREEN", "BLUE",
                          "MAGENTA", "YELLOW", "CYAN", "BROWN", "GRAY"};

    printf("\r\n========== Test 1: LCD ==========\r\n");

    /* 彩色条纹 */
    for (i = 0; i < 10; i++)
    {
        lcd_fill(0, i * 32, 320, (i + 1) * 32, color_table[i]);
        lcd_show_string(10, i * 32 + 8, 200, 16, 16, color_name[i], color_table[i] == WHITE ? BLACK : WHITE);
    }
    delay_ms(1500);

    /* 黑底显示文字 */
    lcd_clear(BLACK);
    lcd_show_string(10, 10, 300, 24, 24, "STM32F429 Apollo V2", RED);
    lcd_show_string(10, 40, 300, 16, 16, "LCD TEST - ALIENTEK", GREEN);
    lcd_show_string(10, 60, 300, 16, 16, "800x480 TFTLCD (MCU)", BLUE);

    /* 画矩形 */
    lcd_draw_rectangle(10, 90, 150, 170, RED);
    lcd_draw_rectangle(170, 90, 310, 170, GREEN);

    /* 画圆 */
    lcd_draw_circle(240, 200, 50, BLUE);
    lcd_fill_circle(120, 200, 40, YELLOW);

    delay_ms(2000);

    /* 清屏准备显示结果 */
    lcd_clear(BLACK);
    lcd_show_string(10, 10, 300, 24, 24, "Acceptance Test", YELLOW);
    test_show_result("LCD Display", 1);
}

/**
 * @brief       测试2: 24C02 EEPROM测试
 * @retval      无
 */
static void test_eeprom(void)
{
    uint8_t buf[10];
    uint8_t i;
    uint8_t ret;

    printf("\r\n========== Test 2: 24C02 ==========\r\n");

    at24cxx_init();

    /* 写入测试数据 */
    for (i = 0; i < 10; i++)
    {
        at24cxx_write_one_byte(i, i + 0xA5);
    }

    /* 读回验证 */
    memset(buf, 0, 10);
    for (i = 0; i < 10; i++)
    {
        buf[i] = at24cxx_read_one_byte(i);
    }

    ret = 1;
    for (i = 0; i < 10; i++)
    {
        if (buf[i] != (i + 0xA5))
        {
            ret = 0;
            printf("  24C02 Error: addr=%d, write=0x%02X, read=0x%02X\r\n",
                   i, i + 0xA5, buf[i]);
            break;
        }
    }

    test_show_result("24C02 EEPROM", ret);
}

/**
 * @brief       测试3: LED测试
 * @retval      无
 */
static void test_led(void)
{
    uint8_t i;

    printf("\r\n========== Test 3: LED ==========\r\n");

    for (i = 0; i < 6; i++)
    {
        LED0_TOGGLE();
        LED1_TOGGLE();
        delay_ms(200);
    }

    LED0(0);
    LED1(0);

    test_show_result("LED Blink", 1);
}

/**
 * @brief       测试4: 蜂鸣器测试
 * @retval      无
 */
static void test_buzzer(void)
{
    printf("\r\n========== Test 4: Buzzer ==========\r\n");

    /* 蜂鸣器通过PCF8574 P0口控制，低电平响 */
    pcf8574_write_bit(BEEP_IO, 0);
    delay_ms(200);
    pcf8574_write_bit(BEEP_IO, 1);

    delay_ms(100);

    pcf8574_write_bit(BEEP_IO, 0);
    delay_ms(200);
    pcf8574_write_bit(BEEP_IO, 1);

    test_show_result("Buzzer (PB8)", 1);
}

/**
 * @brief       测试5: 按键测试
 * @retval      无
 */
static void test_key(void)
{
    uint8_t key;

    printf("\r\n========== Test 5: Key ==========\r\n");

    lcd_show_string(10, g_test_y, 300, 16, 16, "Press KEY0...", GRAY);
    printf("  Waiting for KEY0 press...\r\n");

    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            break;
        }
        LED0_TOGGLE();
        delay_ms(100);
    }

    LED0(0);
    test_show_result("Key Test (KEY0)", 1);
}

/**
 * @brief       测试6: 红外遥控测试
 * @retval      无
 */
static void test_remote(void)
{
    uint8_t key;
    uint32_t timeout;

    printf("\r\n========== Test 6: Remote ==========\r\n");

    remote_init();

    lcd_show_string(10, g_test_y, 300, 16, 16,
                    "Press any remote key (5s timeout)...", GRAY);
    printf("  Waiting for remote key (5s timeout)...\r\n");

    timeout = 5000;
    while (timeout > 0)
    {
        key = remote_scan();
        if (key != 0)
        {
            printf("  Remote key: 0x%02X\r\n", key);
            test_show_result("IR Remote", 1);
            return;
        }
        delay_ms(10);
        timeout -= 10;
    }

    printf("  Remote timeout - no key pressed\r\n");
    test_show_result("IR Remote", 0);
}

/**
 * @brief       测试7: AP3216C光环境传感器
 * @retval      无
 */
static void test_ap3216c(void)
{
    uint8_t ret;
    uint16_t ir, ps, als;

    printf("\r\n========== Test 7: AP3216C ==========\r\n");

    ret = ap3216c_init();

    if (ret == 0)
    {
        delay_ms(100);
        ap3216c_read_data(&ir, &ps, &als);
        printf("  AP3216C: IR=%d, PS=%d, ALS=%d\r\n", ir, ps, als);
        test_show_result("AP3216C Sensor", 1);
    }
    else
    {
        printf("  AP3216C init failed\r\n");
        test_show_result("AP3216C Sensor", 0);
    }
}

/**
 * @brief       测试8: ST480MC磁力计
 * @retval      无
 */
static void test_st480mc(void)
{
    uint8_t ret;
    int16_t magx, magy, magz;

    printf("\r\n========== Test 8: ST480MC ==========\r\n");

    ret = st480mc_init();

    if (ret == 0)
    {
        delay_ms(100);
        ret = st480mc_read_magdata(&magx, &magy, &magz);
        if (ret == 0)
        {
            printf("  ST480MC: X=%d, Y=%d, Z=%d\r\n", magx, magy, magz);
            test_show_result("ST480MC Magnetometer", 1);
        }
        else
        {
            printf("  ST480MC read data failed\r\n");
            test_show_result("ST480MC Magnetometer", 0);
        }
    }
    else
    {
        printf("  ST480MC init failed\r\n");
        test_show_result("ST480MC Magnetometer", 0);
    }
}

/**
 * @brief       测试9: SH3001六轴传感器
 * @retval      无
 */
static void test_sh3001(void)
{
    uint8_t ret;
    short acc[3], gyro[3];
    float temp;

    printf("\r\n========== Test 9: SH3001 ==========\r\n");

    ret = sh3001_init();

    if (ret == 0)
    {
        delay_ms(100);
        sh3001_get_imu_data(acc, gyro);
        temp = sh3001_get_temperature();
        printf("  SH3001: Acc=(%d,%d,%d) Gyro=(%d,%d,%d) Temp=%.1f\r\n",
               acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2], temp);
        test_show_result("SH3001 6-Axis IMU", 1);
    }
    else
    {
        printf("  SH3001 init failed\r\n");
        test_show_result("SH3001 6-Axis IMU", 0);
    }
}

/**
 * @brief       测试10: SDRAM测试
 * @retval      无
 */
static void test_sdram(void)
{
    uint32_t i;
    uint32_t *p = (uint32_t *)BANK5_SDRAM_ADDR;
    uint8_t ret;

    printf("\r\n========== Test 10: SDRAM ==========\r\n");

    /* 写入测试 */
    for (i = 0; i < 1024; i++)
    {
        p[i] = i;
    }

    /* 读回验证 */
    ret = 1;
    for (i = 0; i < 1024; i++)
    {
        if (p[i] != i)
        {
            ret = 0;
            printf("  SDRAM Error: addr=0x%08X, write=0x%08X, read=0x%08X\r\n",
                   (uint32_t)&p[i], i, p[i]);
            break;
        }
    }

    /* 测试大块数据 */
    if (ret)
    {
        for (i = 0; i < 256 * 1024; i++)
        {
            p[i] = 0xA5A5A5A5;
        }

        for (i = 0; i < 256 * 1024; i++)
        {
            if (p[i] != 0xA5A5A5A5)
            {
                ret = 0;
                printf("  SDRAM Bulk Error at 0x%08X\r\n", (uint32_t)&p[i]);
                break;
            }
        }
    }

    printf("  SDRAM: %s (1MB test)\r\n", ret ? "PASS" : "FAIL");
    test_show_result("SDRAM (0xC0000000)", ret);
}

/**
 * @brief       测试11: ADC测试
 * @retval      无
 */
static void test_adc(void)
{
    uint32_t adc_val;
    uint32_t voltage;
    char buf[40];

    printf("\r\n========== Test 11: ADC ==========\r\n");

    adc_init();

    /* 采集多次取平均 */
    adc_val = adc_get_result_average(ADC_ADCX_CHY, 10);
    voltage = adc_val * 3300 / 4095;

    printf("  ADC1 CH5: raw=%lu, voltage=%lu mV\r\n", adc_val, voltage);

    sprintf(buf, "ADC: %lu mV (%lu)", voltage, adc_val);

    if (adc_val > 0 && adc_val < 4095)
    {
        lcd_show_string(10, g_test_y, 300, 16, 16, buf, GRAY);
        g_test_y += 20;
        test_show_result("ADC (RV1, CH5)", 1);
    }
    else
    {
        test_show_result("ADC (RV1, CH5)", 0);
    }
}

/**
 * @brief       显示最终统计
 * @retval      无
 */
static void test_show_summary(void)
{
    char buf[40];

    g_test_y += 20;
    lcd_show_string(10, g_test_y, 300, 24, 24, "Test Summary", YELLOW);
    g_test_y += 30;

    sprintf(buf, "PASS: %d / %d", g_test_pass, g_test_pass + g_test_fail);
    lcd_show_string(10, g_test_y, 300, 20, 20, buf, GREEN);
    g_test_y += 25;

    if (g_test_fail == 0)
    {
        lcd_show_string(10, g_test_y, 300, 24, 24, "ALL PASSED!", GREEN);
    }
    else
    {
        sprintf(buf, "FAIL: %d", g_test_fail);
        lcd_show_string(10, g_test_y, 300, 20, 20, buf, RED);
    }

    printf("\r\n========================================\r\n");
    printf("Test Summary: PASS=%d, FAIL=%d, Total=%d\r\n",
           g_test_pass, g_test_fail, g_test_pass + g_test_fail);
    printf("========================================\r\n");
}

/**
 * @brief       运行全部验收测试
 * @retval      无
 */
void APP_Test_Run(void)
{
    g_test_y = 10;
    g_test_pass = 0;
    g_test_fail = 0;

    printf("\r\n");
    printf("========================================\r\n");
    printf("  STM32F429 Apollo V2 Acceptance Test\r\n");
    printf("  LCD Version\r\n");
    printf("========================================\r\n");

    test_lcd();             /* 1. LCD显示测试 */
    test_eeprom();          /* 2. 24C02 EEPROM测试 */
    test_led();             /* 3. LED测试 */
    test_buzzer();          /* 4. 蜂鸣器测试 */
    test_key();             /* 5. 按键测试 */
    test_remote();          /* 6. 红外遥控测试 */
    test_ap3216c();         /* 7. AP3216C光环境传感器 */
    test_st480mc();         /* 8. ST480MC磁力计 */
    test_sh3001();          /* 9. SH3001六轴传感器 */
    test_sdram();           /* 10. SDRAM测试 */
    test_adc();             /* 11. ADC测试 */

    test_show_summary();    /* 显示最终统计 */
}
