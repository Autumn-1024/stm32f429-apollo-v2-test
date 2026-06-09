/**
 ****************************************************************************************************
 * @file        sdio_sdcard.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       SD�� ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
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

#include "string.h"
#include "./Middlewares/SYSTEM/usart/usart.h"
#include "./BSP/SDIO/sdio_sdcard.h"


SD_HandleTypeDef g_sdcard_handle;             /* SD����� */
HAL_SD_CardInfoTypeDef g_sd_card_info_handle; /* SD����Ϣ�ṹ�� */


/**
 * @brief       ��ʼ��SD��
 * @param       ��
 * @retval      ����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;

    /* ��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHZ */
    g_sdcard_handle.Instance = SDIO;
    g_sdcard_handle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;                       /* ������ */
    g_sdcard_handle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;                  /* ��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDIO_CK */
    g_sdcard_handle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;           /* ����ʱ���ر�ʱ�ӵ�Դ */
    g_sdcard_handle.Init.BusWide = SDIO_BUS_WIDE_1B;                               /* 1λ������ */
    g_sdcard_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE; /* �ر�Ӳ������ */
    g_sdcard_handle.Init.ClockDiv = SDIO_TRANSF_CLK_DIV;                           /* SD����ʱ��Ƶ�����25MHZ */

    SD_Error = HAL_SD_Init(&g_sdcard_handle);
    if (SD_Error != HAL_OK)
    {
        return 1;
    }
    
    HAL_SD_GetCardInfo(&g_sdcard_handle, &g_sd_card_info_handle);                  /* ��ȡSD����Ϣ */

    SD_Error = HAL_SD_ConfigWideBusOperation(&g_sdcard_handle, SDIO_BUS_WIDE_4B);  /* ʹ�ܿ�����ģʽ */
    if (SD_Error != HAL_OK)
    {
        return 2;
    }
    
    return 0;
}

/**
 * @brief       SDIO�ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_SD_Init()����
 * @param       hsd  : SD�����
 * @retval      ��
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    if (hsd->Instance == SDIO)
    {
        GPIO_InitTypeDef gpio_init_struct;

        __HAL_RCC_SDIO_CLK_ENABLE();                        /* ʹ��SDIOʱ�� */

        SD_D0_GPIO_CLK_ENABLE();                            /* D0����IOʱ��ʹ�� */
        SD_D1_GPIO_CLK_ENABLE();                            /* D1����IOʱ��ʹ�� */
        SD_D2_GPIO_CLK_ENABLE();                            /* D2����IOʱ��ʹ�� */
        SD_D3_GPIO_CLK_ENABLE();                            /* D3����IOʱ��ʹ�� */
        SD_CLK_GPIO_CLK_ENABLE();                           /* CLK����IOʱ��ʹ�� */
        SD_CMD_GPIO_CLK_ENABLE();                           /* CMD����IOʱ��ʹ�� */

        /* SD_D0����ģʽ���� */
        gpio_init_struct.Pin = SD_D0_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* ���츴�� */
        gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* ���� */
        gpio_init_struct.Alternate = GPIO_AF12_SDIO;        /* ����ΪSDIO */
        HAL_GPIO_Init(SD_D0_GPIO_PORT, &gpio_init_struct);  /* ��ʼ�� */
        
        /* SD_D1����ģʽ���� */
        gpio_init_struct.Pin = SD_D1_GPIO_PIN;
        HAL_GPIO_Init(SD_D1_GPIO_PORT, &gpio_init_struct);  /* ��ʼ�� */
        /* SD_D2����ģʽ���� */
        gpio_init_struct.Pin = SD_D2_GPIO_PIN;
        HAL_GPIO_Init(SD_D2_GPIO_PORT, &gpio_init_struct);  /* ��ʼ�� */
        /* SD_D3����ģʽ���� */
        gpio_init_struct.Pin = SD_D3_GPIO_PIN;
        HAL_GPIO_Init(SD_D3_GPIO_PORT, &gpio_init_struct);  /* ��ʼ�� */
        /* SD_CLK����ģʽ���� */
        gpio_init_struct.Pin = SD_CLK_GPIO_PIN;
        HAL_GPIO_Init(SD_CLK_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� */
        /* SD_CMD����ģʽ���� */
        gpio_init_struct.Pin = SD_CMD_GPIO_PIN;
        HAL_GPIO_Init(SD_CMD_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� */
    }
}

/**
 * @brief       ��ȡ����Ϣ����
 * @param       cardinfo   : SD����Ϣ���
 * @retval      ����ֵ:��ȡ����Ϣ״ֵ̬
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&g_sdcard_handle, cardinfo);
    return sta;
}

/**
 * @brief       �ж�SD���Ƿ���Դ���(��д)����
 * @param       ��
 * @retval      ����ֵ:SD_TRANSFER_OK      ������ɣ����Լ�����һ�δ���
                       SD_TRANSFER_BUSY SD ����æ�������Խ�����һ�δ���
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&g_sdcard_handle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       ��SD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    sys_intx_disable();                                                                    /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_ReadBlocks(&g_sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT);  /* ���sector�Ķ����� */

    /* �ȴ�SD������ */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    sys_intx_enable();      /* �������ж� */
    return sta;
}

/**
 * @brief       дSD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    sys_intx_disable();                                                                     /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_WriteBlocks(&g_sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT);  /* ���sector��д���� */

    /* �ȴ�SD��д�� */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    sys_intx_enable();     /* �������ж� */
    
    return sta;
}
