/**
 ****************************************************************************************************
 * @file        ethernet.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-08-01
 * @brief       ETHERNET ��������
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
 * V1.0 20211014
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/ETHERNET/ethernet.h"
#include "./BSP/PCF8574/pcf8574.h"
#include "./BSP/ETHERNET/ethernet_chip.h"
#include "lwip_comm.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"


ETH_HandleTypeDef   g_eth_handler;                               /* ��̫����� */
ETH_DMADescTypeDef  g_eth_dma_rx_dscr_tab[ETH_RX_DESC_CNT];      /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  g_eth_dma_tx_dscr_tab[ETH_TX_DESC_CNT];      /* Ethernet Tx DMA Descriptors */

ETH_MACFilterConfigTypeDef g_eth_macfilterconfig_handler;
/**
 * @brief       ��̫��оƬ��ʼ��
 * @param       ��
 * @retval      0,�ɹ�
 *              1,ʧ��
 */
uint8_t ethernet_init(void)
{
    uint8_t macaddress[6];
    
    macaddress[0] = g_lwipdev.mac[0];
    macaddress[1] = g_lwipdev.mac[1];
    macaddress[2] = g_lwipdev.mac[2];
    macaddress[3] = g_lwipdev.mac[3];
    macaddress[4] = g_lwipdev.mac[4];
    macaddress[5] = g_lwipdev.mac[5];

    g_eth_handler.Instance = ETH;
    g_eth_handler.Init.MACAddr = macaddress;
    g_eth_handler.Init.MediaInterface = HAL_ETH_RMII_MODE;
    g_eth_handler.Init.RxDesc = g_eth_dma_rx_dscr_tab;
    g_eth_handler.Init.TxDesc = g_eth_dma_tx_dscr_tab;
    g_eth_handler.Init.RxBuffLen = 1000;
    
    if (HAL_ETH_Init(&g_eth_handler) == HAL_OK)
    {
        return 0;   /* �ɹ� */
    }
    else
    {
        return 1;  /* ʧ�� */
    }
}

/**
 * @brief       ETH�ײ�������ʱ��ʹ�ܣ���������
 *    @note     �˺����ᱻHAL_ETH_Init()����
 * @param       heth:��̫�����
 * @retval      ��
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ETH_CLK_GPIO_CLK_ENABLE();          /* ����ETH_CLKʱ�� */
    ETH_MDIO_GPIO_CLK_ENABLE();         /* ����ETH_MDIOʱ�� */
    ETH_CRS_GPIO_CLK_ENABLE();          /* ����ETH_CRSʱ�� */
    ETH_MDC_GPIO_CLK_ENABLE();          /* ����ETH_MDCʱ�� */
    ETH_RXD0_GPIO_CLK_ENABLE();         /* ����ETH_RXD0ʱ�� */
    ETH_RXD1_GPIO_CLK_ENABLE();         /* ����ETH_RXD1ʱ�� */
    ETH_TX_EN_GPIO_CLK_ENABLE();        /* ����ETH_TX_ENʱ�� */
    ETH_TXD0_GPIO_CLK_ENABLE();         /* ����ETH_TXD0ʱ�� */
    ETH_TXD1_GPIO_CLK_ENABLE();         /* ����ETH_TXD1ʱ�� */
    __HAL_RCC_ETH_CLK_ENABLE();         /* ����ETHʱ�� */

    /* ������������ RMII�ӿ�
     * ETH_MDIO -------------------------> PA2
     * ETH_MDC --------------------------> PC1
     * ETH_RMII_REF_CLK------------------> PA1
     * ETH_RMII_CRS_DV ------------------> PA7
     * ETH_RMII_RXD0 --------------------> PC4
     * ETH_RMII_RXD1 --------------------> PC5
     * ETH_RMII_TX_EN -------------------> PB11
     * ETH_RMII_TXD0 --------------------> PG13
     * ETH_RMII_TXD1 --------------------> PG14
     */

    /* PA1,2,7 */
    gpio_init_struct.Pin = ETH_CLK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ���츴�� */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* ���������� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* ���� */
    gpio_init_struct.Alternate = GPIO_AF11_ETH;             /* ����ΪETH���� */
    HAL_GPIO_Init(ETH_CLK_GPIO_PORT, &gpio_init_struct);    /* ETH_CLK����ģʽ���� */
    
    gpio_init_struct.Pin = ETH_MDIO_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);   /* ETH_MDIO����ģʽ���� */
    
    gpio_init_struct.Pin = ETH_CRS_GPIO_PIN;    
    HAL_GPIO_Init(ETH_CRS_GPIO_PORT, &gpio_init_struct);    /* ETH_CRS����ģʽ���� */

    /* PC1 */
    gpio_init_struct.Pin = ETH_MDC_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDC_GPIO_PORT, &gpio_init_struct);    /* ETH_MDC��ʼ�� */

    /* PC4 */
    gpio_init_struct.Pin = ETH_RXD0_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD0��ʼ�� */
    
    /* PC5 */
    gpio_init_struct.Pin = ETH_RXD1_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD1��ʼ�� */
    
    
    /* PB11,PG13,PG14 */
    gpio_init_struct.Pin = ETH_TX_EN_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TX_EN_GPIO_PORT, &gpio_init_struct);  /* ETH_TX_EN��ʼ�� */

    gpio_init_struct.Pin = ETH_TXD0_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD0��ʼ�� */
    
    gpio_init_struct.Pin = ETH_TXD1_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD1��ʼ�� */
    
    uint32_t regval;

    sys_intx_disable();                                     /* �ر������жϣ���λ���̲��ܱ���ϣ� */
    /* �жϿ������Ƿ��Ǿɰ汾(�ϰ忨���ص���LAN8720A�����°忨���ص���YT8512C) */
    regval = ethernet_read_phy(2);
    
    if (regval && 0xFFF == 0xFFF)                           /* �ɰ忨��LAN8720A�����Ÿ�λ */
    {
        pcf8574_write_bit(ETH_RESET_IO,1);                  /* Ӳ����λ */
        delay_ms(100);
        pcf8574_write_bit(ETH_RESET_IO,0);                  /* ��λ���� */
        delay_ms(100);
    }
    else                                                    /* �°忨��YT8512C�����Ÿ�λ */
    {
        pcf8574_write_bit(ETH_RESET_IO,0);                  /* Ӳ����λ */
        delay_ms(100);
        pcf8574_write_bit(ETH_RESET_IO,1);                  /* ��λ���� */
        delay_ms(100);
    }
    
    sys_intx_enable();                                      /* ���������ж� */
}

/**
 * @breif       ��ȡ��̫��оƬ�Ĵ���ֵ
 * @param       reg����ȡ�ļĴ�����ַ
 * @retval      regval�����ض�ȡ�ļĴ���ֵ
 */
uint32_t ethernet_read_phy(uint16_t reg)
{
    uint32_t regval;

    HAL_ETH_ReadPHYRegister(&g_eth_handler, ETH_CHIP_ADDR,reg, &regval);
    return regval;
}

/**
 * @breif       ����̫��оƬָ����ַд��Ĵ���ֵ
 * @param       reg   : Ҫд��ļĴ���
 * @param       value : Ҫд��ļĴ���
 * @retval      ��
 */
void ethernet_write_phy(uint16_t reg, uint16_t value)
{
    uint32_t temp = value;
    
    HAL_ETH_WritePHYRegister(&g_eth_handler, ETH_CHIP_ADDR,reg, temp);
}

/**
 * @breif       �������оƬ���ٶ�ģʽ
 * @param       ��
 * @retval      1:100M
                0:10M
 */
uint8_t ethernet_chip_get_speed(void)
{
    uint8_t speed;
    if(PHY_TYPE == LAN8720) 
    speed = ~((ethernet_read_phy(ETH_CHIP_PHYSCSR) & ETH_CHIP_SPEED_STATUS));         /* ��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    else if(PHY_TYPE == SR8201F)
    speed = ((ethernet_read_phy(ETH_CHIP_PHYSCSR) & ETH_CHIP_SPEED_STATUS) >> 13);    /* ��SR8201F��0�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    else if(PHY_TYPE == YT8512C)
    speed = ((ethernet_read_phy(ETH_CHIP_PHYSCSR) & ETH_CHIP_SPEED_STATUS) >> 14);    /* ��YT8512C��17�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    else if(PHY_TYPE == RTL8201)
    speed = ((ethernet_read_phy(ETH_CHIP_PHYSCSR) & ETH_CHIP_SPEED_STATUS) >> 1);     /* ��RTL8201��16�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    return speed;
}
