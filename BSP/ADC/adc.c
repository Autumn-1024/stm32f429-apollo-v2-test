/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       ADC ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ F429������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./Middlewares/SYSTEM/delay/delay.h"


ADC_HandleTypeDef g_adc_handle;                                    /* ADC��� */

/**
 * @brief       ADC��ʼ������
 * @note        ������֧��ADC1/ADC2����ͨ��, ���ǲ�֧��ADC3
 *              ����ʹ��12λ����, ADC����ʱ��=22.5M, ת��ʱ��Ϊ: �������� + 12��ADC����
 *              ��������������: 480, ��ת��ʱ�� = 492 ��ADC���� = 21.87us
 * @param       ��
 * @retval      ��
 */
void adc_init(void)
{ 
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;   /* 4��Ƶ��ADCCLK=PCLK2/4=90/4=22.5MHZ */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;             /* 12λģʽ */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;             /* �Ҷ��� */
    g_adc_handle.Init.ScanConvMode = DISABLE;                      /* ��ɨ��ģʽ */
    g_adc_handle.Init.EOCSelection = DISABLE;                      /* �ر�EOC�ж� */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                /* �ر�����ת�� */
    g_adc_handle.Init.NbrOfConversion = 1;                         /* 1��ת���ڹ��������У�Ҳ����ֻת����������1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;             /* ��ֹ����������ģʽ */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                     /* ����������ͨ����Ϊ0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;       /* �������� */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* ʹ���������� */
    g_adc_handle.Init.DMAContinuousRequests = DISABLE;             /* �ر�DMA���� */
    HAL_ADC_Init(&g_adc_handle);                                   /* ��ʼ�� */
}

/**
 * @brief       ADC�ײ��������������ã�ʱ��ʹ��
 * @note        �˺����ᱻHAL_ADC_Init()����
 * @param       hadc : ADC���
 * @retval      ��
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef gpio_init_struct;
    ADC_ADCX_CHY_CLK_ENABLE();                                      /* ʹ��ADC1ʱ�� */
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                 /* ����GPIOAʱ�� */

    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                   /* PA5 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                       /* ģ������ */
    gpio_init_struct.Pull = GPIO_NOPULL;                            /* ���������� */
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch    : ͨ��ֵ 0~18��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_18
 * @retval      ����ֵ:ת�����
 */
uint32_t adc_get_result(uint32_t ch)   
{
    ADC_ChannelConfTypeDef adc_channel;
    
    adc_channel.Channel = ch;                                   /* ͨ�� */
    adc_channel.Rank = 1;                                       /* ��1�����У�����1 */
    adc_channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;        /* ����ʱ�� */
    adc_channel.Offset = 0;
    HAL_ADC_ConfigChannel(&g_adc_handle, &adc_channel);         /* ͨ������ */

    HAL_ADC_Start(&g_adc_handle);                               /* ����ADC */

    HAL_ADC_PollForConversion(&g_adc_handle, 10);               /* ��ѯת�� */
 
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);           /* �������һ��ADC1�������ת����� */
}

/**
 * @brief       ��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
 * @param       ch      : ͨ����, 0~18
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* ��ȡtimes������ */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* ����ƽ��ֵ */
} 


