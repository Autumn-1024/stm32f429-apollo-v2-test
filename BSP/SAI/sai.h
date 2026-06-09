/**
 ****************************************************************************************************
 * @file        sai.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       SAI ��������
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

#ifndef __SAI_H
#define __SAI_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/******************************************************************************************/
/* SAI1 ���� ���� */

#define SAI1_CLK_GPIO_PORT              GPIOE
#define SAI1_CLK_GPIO_PIN               GPIO_PIN_2
#define SAI1_CLK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

#define SAI1_SCK_GPIO_PORT              GPIOE
#define SAI1_SCK_GPIO_PIN               GPIO_PIN_5
#define SAI1_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

#define SAI1_FSA_GPIO_PORT              GPIOE
#define SAI1_FSA_GPIO_PIN               GPIO_PIN_4
#define SAI1_FSA_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

#define SAI1_SDA_GPIO_PORT              GPIOE
#define SAI1_SDA_GPIO_PIN               GPIO_PIN_6
#define SAI1_SDA_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

#define SAI1_SDB_GPIO_PORT              GPIOE
#define SAI1_SDB_GPIO_PIN               GPIO_PIN_3
#define SAI1_SDB_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

/* SAI1��ض��� */
#define SAI1_SAI_CLK_ENABLE()           do{ __HAL_RCC_SAI1_CLK_ENABLE(); }while(0)    /* SAI1ʱ��ʹ�� */

#define SAI1_TX_DMASx                   DMA2_Stream3
#define SAI1_TX_DMASx_Channel           DMA_CHANNEL_0
#define SAI1_TX_DMASx_IRQHandler        DMA2_Stream3_IRQHandler
#define SAI1_TX_DMASx_IRQ               DMA2_Stream3_IRQn
#define SAI1_TX_DMASx_FLAG              DMA_FLAG_TCIF3_7
#define SAI1_TX_DMA_CLK_ENABLE()        do{ __HAL_RCC_DMA2_CLK_ENABLE(); }while(0)   /* SAIA TX DMAʱ��ʹ�� */


/******************************************************************************************/
extern SAI_HandleTypeDef g_sai1_a_handle;         /* SAI1 Block A��� */

extern void (*sai1_tx_callback)(void);            /* sai tx�ص�����ָ�� */

void sai1_saia_init(uint8_t mode, uint8_t cpol, uint8_t datalen);
uint8_t sai1_samplerate_set(uint32_t samplerate);
void sai1_tx_dma_init(uint8_t* buf0, uint8_t *buf1, uint16_t num, uint8_t width);
void sai1_saia_dma_enable(void);
void sai1_play_start(void); 
void sai1_play_stop(void); 

#endif
