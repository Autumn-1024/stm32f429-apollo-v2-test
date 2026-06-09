/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-4-20
 * @brief       SPI ��������
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
 * V1.1 20220420
 * ����SPI2��������
 *
 ****************************************************************************************************
 */
 
#ifndef __SPI_H
#define __SPI_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/******************************************************************************************/
/* SPI5 ���� ���� */

#define SPI5_SCK_GPIO_PORT              GPIOF
#define SPI5_SCK_GPIO_PIN               GPIO_PIN_7
#define SPI5_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF��ʱ��ʹ�� */

#define SPI5_MISO_GPIO_PORT             GPIOF
#define SPI5_MISO_GPIO_PIN              GPIO_PIN_8
#define SPI5_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF��ʱ��ʹ�� */

#define SPI5_MOSI_GPIO_PORT             GPIOF
#define SPI5_MOSI_GPIO_PIN              GPIO_PIN_9
#define SPI5_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF��ʱ��ʹ�� */

/* SPI5��ض��� */
#define SPI5_SPI                        SPI5
#define SPI5_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI5_CLK_ENABLE(); }while(0)    /* SPI5ʱ��ʹ�� */

/******************************************************************************************/
/* SPI2 ���� ���� */

#define SPI2_SCK_GPIO_PORT              GPIOB
#define SPI2_SCK_GPIO_PIN               GPIO_PIN_13
#define SPI2_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define SPI2_MISO_GPIO_PORT             GPIOB
#define SPI2_MISO_GPIO_PIN              GPIO_PIN_14
#define SPI2_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define SPI2_MOSI_GPIO_PORT             GPIOB
#define SPI2_MOSI_GPIO_PIN              GPIO_PIN_15
#define SPI2_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

/* SPI2��ض��� */
#define SPI2_SPI                        SPI2
#define SPI2_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI2_CLK_ENABLE(); }while(0)    /* SPI2ʱ��ʹ�� */

/******************************************************************************************/


void spi5_init(void);
void spi5_set_speed(uint8_t speed);
uint8_t spi5_read_write_byte(uint8_t txdata);

void spi2_init(void);
void spi2_set_speed(uint8_t speed);
uint8_t spi2_read_write_byte(uint8_t txdata);

#endif
