/**
 ****************************************************************************************************
 * @file        sdio_sdcard.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       SD�� ��������
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

#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/******************************************************************************************/
/* SDIO���ź���: SD_D0 ~ SD_D3/SD_CLK/SD_CMD ���� ���� 
 * ���ʹ������������SDIO���ź���,�޸�����д���弴������.
 */

#define SD_D0_GPIO_PORT                GPIOC
#define SD_D0_GPIO_PIN                 GPIO_PIN_8
#define SD_D0_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

#define SD_D1_GPIO_PORT                GPIOC
#define SD_D1_GPIO_PIN                 GPIO_PIN_9
#define SD_D1_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

#define SD_D2_GPIO_PORT                GPIOC
#define SD_D2_GPIO_PIN                 GPIO_PIN_10
#define SD_D2_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

#define SD_D3_GPIO_PORT                GPIOC
#define SD_D3_GPIO_PIN                 GPIO_PIN_11
#define SD_D3_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

#define SD_CLK_GPIO_PORT               GPIOC
#define SD_CLK_GPIO_PIN                GPIO_PIN_12
#define SD_CLK_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

#define SD_CMD_GPIO_PORT               GPIOD
#define SD_CMD_GPIO_PIN                GPIO_PIN_2
#define SD_CMD_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)    /* ����IO��ʱ��ʹ�� */

/******************************************************************************************/

#define SD_TIMEOUT             ((uint32_t)100000000)                                  /* ��ʱʱ�� */
#define SD_TRANSFER_OK         ((uint8_t)0x00)
#define SD_TRANSFER_BUSY       ((uint8_t)0x01)

/* ���� SD_HandleTypeDef ����ĺ꣬���ڿ��ټ������� */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr)*((__Handle__)->SdCard.LogBlockSize))>>0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr)*((__Handle__)->SdCard.LogBlockSize))>>10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr)*((__Handle__)->SdCard.LogBlockSize))>>20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr)*((__Handle__)->SdCard.LogBlockSize))>>30)

/*  
 *  SD����ʱ�ӷ�Ƶ������HAL������Ч�ʵͣ������ײ������磨��SD��ʱ��/�������дSD��ʱ��
 *  ʹ��4bitģʽʱ���轵��SDIOʱ��Ƶ�ʣ����ú��Ϊ 1��SDIOʱ��Ƶ�ʣ�48/( SDIO_TRANSF_CLK_DIV + 2 ) = 16M * 4bit = 64Mbps 
 *  ʹ��1bitģʽʱ���ú�SDIO_TRANSF_CLK_DIV��Ϊ 0��SDIOʱ��Ƶ�ʣ�48/( SDIO_TRANSF_CLK_DIV + 2 ) = 24M * 1bit = 24Mbps 
 */
#define  SDIO_TRANSF_CLK_DIV        1   
/******************************************************************************************/

extern SD_HandleTypeDef        g_sdcard_handle;                                        /* SD����� */
extern HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;                                  /* SD����Ϣ�ṹ�� */

uint8_t sd_init(void);
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo);
uint8_t get_sd_card_state(void);
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);

#endif
