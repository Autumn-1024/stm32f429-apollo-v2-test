/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       ADC 驱动代码
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
 * 修改说明
 * V1.0 20220420
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./Middlewares/SYSTEM/delay/delay.h"


ADC_HandleTypeDef g_adc_handle;                                    /* ADC句柄 */

/**
 * @brief       ADC初始化函数
 * @note        本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=22.5M, 转换时间为: 采样周期 + 12个ADC周期
 *              设置最大采样周期: 480, 则转换时间 = 492 个ADC周期 = 21.87us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{ 
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;   /* 4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;             /* 12位模式 */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;             /* 右对齐 */
    g_adc_handle.Init.ScanConvMode = DISABLE;                      /* 非扫描模式 */
    g_adc_handle.Init.EOCSelection = DISABLE;                      /* 关闭EOC中断 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                /* 关闭连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                         /* 1个转换在规则序列中，也就是只转换规则序列1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;             /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                     /* 不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;       /* 软件触发 */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
    g_adc_handle.Init.DMAContinuousRequests = DISABLE;             /* 关闭DMA请求 */
    HAL_ADC_Init(&g_adc_handle);                                   /* 初始化 */
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
 * @note        此函数会被HAL_ADC_Init()调用
 * @param       hadc : ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef gpio_init_struct;
    ADC_ADCX_CHY_CLK_ENABLE();                                      /* 使能ADC1时钟 */
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                 /* 开启GPIOA时钟 */

    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                   /* PA5 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                       /* 模拟输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                            /* 不带上下拉 */
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch    : 通道值 0~18，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_18
 * @retval      返回值:转换结果
 */
uint32_t adc_get_result(uint32_t ch)   
{
    ADC_ChannelConfTypeDef adc_channel;
    
    adc_channel.Channel = ch;                                   /* 通道 */
    adc_channel.Rank = 1;                                       /* 第1个序列，序列1 */
    adc_channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;        /* 采样时间 */
    adc_channel.Offset = 0;
    HAL_ADC_ConfigChannel(&g_adc_handle, &adc_channel);         /* 通道配置 */

    HAL_ADC_Start(&g_adc_handle);                               /* 开启ADC */

    HAL_ADC_PollForConversion(&g_adc_handle, 10);               /* 轮询转换 */
 
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);           /* 返回最近一次ADC1规则组的转换结果 */
}

/**
 * @brief       获取指定通道的转换值，取times次,然后平均 
 * @param       ch      : 通道号, 0~18
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* 返回平均值 */
} 


