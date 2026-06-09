/**
 ****************************************************************************************************
 * @file        ethernet_chip.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-08-01
 * @brief       PHYоƬ��������
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
#ifndef ETHERENT_CHIP_H
#define ETHERENT_CHIP_H
#include <stdint.h>
#include "./Middlewares/SYSTEM/sys/sys.h"


#ifdef __cplusplus
 extern "C" {
#endif   

/* PHYоƬ�Ĵ���ӳ��� */ 
#define ETH_CHIP_BCR                            ((uint16_t)0x0000U)
#define ETH_CHIP_BSR                            ((uint16_t)0x0001U)
#define PHY_REGISTER2                           ((uint16_t)0x0002U)
#define PHY_REGISTER3                           ((uint16_t)0x0003U)

/* ����SCR�Ĵ�����ֵ��һ�㲻��Ҫ�޸ģ� */
#define ETH_CHIP_BCR_SOFT_RESET                 ((uint16_t)0x8000U)
#define ETH_CHIP_BCR_LOOPBACK                   ((uint16_t)0x4000U)
#define ETH_CHIP_BCR_SPEED_SELECT               ((uint16_t)0x2000U)
#define ETH_CHIP_BCR_AUTONEGO_EN                ((uint16_t)0x1000U)
#define ETH_CHIP_BCR_POWER_DOWN                 ((uint16_t)0x0800U)
#define ETH_CHIP_BCR_ISOLATE                    ((uint16_t)0x0400U)
#define ETH_CHIP_BCR_RESTART_AUTONEGO           ((uint16_t)0x0200U)
#define ETH_CHIP_BCR_DUPLEX_MODE                ((uint16_t)0x0100U) 

/* ����BSR�Ĵ�����ֵ��һ�㲻��Ҫ�޸ģ� */   
#define ETH_CHIP_BSR_100BASE_T4                 ((uint16_t)0x8000U)
#define ETH_CHIP_BSR_100BASE_TX_FD              ((uint16_t)0x4000U)
#define ETH_CHIP_BSR_100BASE_TX_HD              ((uint16_t)0x2000U)
#define ETH_CHIP_BSR_10BASE_T_FD                ((uint16_t)0x1000U)
#define ETH_CHIP_BSR_10BASE_T_HD                ((uint16_t)0x0800U)
#define ETH_CHIP_BSR_100BASE_T2_FD              ((uint16_t)0x0400U)
#define ETH_CHIP_BSR_100BASE_T2_HD              ((uint16_t)0x0200U)
#define ETH_CHIP_BSR_EXTENDED_STATUS            ((uint16_t)0x0100U)
#define ETH_CHIP_BSR_AUTONEGO_CPLT              ((uint16_t)0x0020U)
#define ETH_CHIP_BSR_REMOTE_FAULT               ((uint16_t)0x0010U)
#define ETH_CHIP_BSR_AUTONEGO_ABILITY           ((uint16_t)0x0008U)
#define ETH_CHIP_BSR_LINK_STATUS                ((uint16_t)0x0004U)
#define ETH_CHIP_BSR_JABBER_DETECT              ((uint16_t)0x0002U)
#define ETH_CHIP_BSR_EXTENDED_CAP               ((uint16_t)0x0001U)

/* PHYоƬ����״̬ */
#define  ETH_CHIP_STATUS_READ_ERROR             ((int32_t)-5)
#define  ETH_CHIP_STATUS_WRITE_ERROR            ((int32_t)-4)
#define  ETH_CHIP_STATUS_ADDRESS_ERROR          ((int32_t)-3)
#define  ETH_CHIP_STATUS_RESET_TIMEOUT          ((int32_t)-2)
#define  ETH_CHIP_STATUS_ERROR                  ((int32_t)-1)
#define  ETH_CHIP_STATUS_OK                     ((int32_t) 0)
#define  ETH_CHIP_STATUS_LINK_DOWN              ((int32_t) 1)
#define  ETH_CHIP_STATUS_100MBITS_FULLDUPLEX    ((int32_t) 2)
#define  ETH_CHIP_STATUS_100MBITS_HALFDUPLEX    ((int32_t) 3)
#define  ETH_CHIP_STATUS_10MBITS_FULLDUPLEX     ((int32_t) 4)
#define  ETH_CHIP_STATUS_10MBITS_HALFDUPLEX     ((int32_t) 5)
#define  ETH_CHIP_STATUS_AUTONEGO_NOTDONE       ((int32_t) 6)

/* PHY��ַ ---- ���û����� */
#define ETH_CHIP_ADDR                           ((uint16_t)0x0000U)
/* PHY�Ĵ��������� */
#define ETH_CHIP_PHY_COUNT                      ((uint16_t)0x001FU)

/* PHY�Զ�ʶ��״̬ */
enum PHY_AUTO
{
    PHY_AUTO_SELECT_NABLE = 0,
    PHY_AUTO_SELECT_DISABLE
};

/* ʹ��/����PHY�Զ�ѡ���� */
#define PHY_AUTO_SELECT                         PHY_AUTO_SELECT_NABLE
#define LAN8720                                 0
#define SR8201F                                 1
#define YT8512C                                 2
#define RTL8201                                 3

#if PHY_AUTO_SELECT

/* ѡ��PHYоƬ ---- ���û����� */

#define PHY_TYPE                                YT8512C

#if(PHY_TYPE == LAN8720) 
#define ETH_CHIP_PHYSCSR                        ((uint16_t)0x1F)                       /*!< tranceiver status register */
#define ETH_CHIP_SPEED_STATUS                   ((uint16_t)0x0004)                     /*!< configured information of speed: 100Mbit/s */
#define ETH_CHIP_DUPLEX_STATUS                  ((uint16_t)0x0010)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == SR8201F)
#define ETH_CHIP_PHYSCSR                        ((uint16_t)0x00)                       /*!< tranceiver status register */
#define ETH_CHIP_SPEED_STATUS                   ((uint16_t)0x2020)                     /*!< configured information of speed: 100Mbit/s */
#define ETH_CHIP_DUPLEX_STATUS                  ((uint16_t)0x0100)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == YT8512C)
#define ETH_CHIP_PHYSCSR                        ((uint16_t)0x11)                       /*!< tranceiver status register */
#define ETH_CHIP_SPEED_STATUS                   ((uint16_t)0x4010)                     /*!< configured information of speed: 100Mbit/s */
#define ETH_CHIP_DUPLEX_STATUS                  ((uint16_t)0x2000)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == RTL8201)
#define PHY_SR                                  ((uint16_t)0x10)                       /*!< tranceiver status register */
#define ETH_CHIP_SPEED_STATUS                   ((uint16_t)0x0022)                     /*!< configured information of speed: 100Mbit/s */
#define ETH_CHIP_DUPLEX_STATUS                  ((uint16_t)0x0004)                     /*!< configured information of duplex: full-duplex */
#endif /* PHY_TYPE */
#else
extern int PHY_TYPE;
extern uint16_t ETH_CHIP_PHYSCSR;
extern uint16_t ETH_CHIP_SPEED_STATUS;
extern uint16_t ETH_CHIP_DUPLEX_STATUS;
#endif



/* ���庯��ָ�� */ 
typedef int32_t  (*eth_chip_init_func)          (void); 
typedef int32_t  (*eth_chip_deinit_func)        (void);
typedef int32_t  (*eth_chip_readreg_func)       (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*eth_chip_writereg_func)      (uint32_t, uint32_t, uint32_t);
typedef int32_t  (*eth_chip_gettick_func)       (void);

/* PHY���ú����ṹ�� */ 
typedef struct 
{
    eth_chip_init_func          init;                   /* ָ��PHY��ʼ������ */ 
    eth_chip_deinit_func        deinit;                 /* ָ��PHY����ʼ������ */ 
    eth_chip_writereg_func      writereg;               /* ָ��PHYд�Ĵ������� */ 
    eth_chip_readreg_func       readreg;                /* ָ��PHY���Ĵ������� */ 
    eth_chip_gettick_func       gettick;                /* ָ����ĺ��� */ 
} eth_chip_ioc_tx_t;  

/* ע�ᵽ�������ṹ�� */
typedef struct 
{
    uint32_t            devaddr;                        /* PHY��ַ */
    uint32_t            is_initialized;                 /* �������豸�Ƿ��ʼ�� */
    eth_chip_ioc_tx_t   io;                             /* �豸���õĺ������ */
    void                *pdata;                         /* ������β� */
}eth_chip_object_t;


int32_t eth_chip_regster_bus_io(eth_chip_object_t *pobj, eth_chip_ioc_tx_t *ioctx);             /* ��IO����ע�ᵽ������� */
int32_t eth_chip_init(eth_chip_object_t *pobj);                                                 /* ��ʼ��ETH_CHIP�����������Ӳ����Դ */
int32_t eth_chip_deinit(eth_chip_object_t *pobj);                                               /* ����ʼ��ETH_CHIP����Ӳ����Դ */
int32_t eth_chip_disable_power_down_mode(eth_chip_object_t *pobj);                              /* �ر�ETH_CHIP���µ�ģʽ */
int32_t eth_chip_enable_power_down_mode(eth_chip_object_t *pobj);                               /* ʹ��ETH_CHIP���µ�ģʽ */
int32_t eth_chip_start_auto_nego(eth_chip_object_t *pobj);                                      /* �����Զ�Э�̹��� */
int32_t eth_chip_get_link_state(eth_chip_object_t *pobj);                                       /* ��ȡETH_CHIP�豸����·״̬ */
int32_t eth_chip_set_link_state(eth_chip_object_t *pobj, uint32_t linkstate);                   /* ����ETH_CHIP�豸����·״̬ */
int32_t eth_chip_enable_loop_back_mode(eth_chip_object_t *pobj);                                /* ���û���ģʽ */
int32_t eth_chip_disable_loop_back_mode(eth_chip_object_t *pobj);                               /* ���û���ģʽ */

#ifdef __cplusplus
}
#endif

#endif
