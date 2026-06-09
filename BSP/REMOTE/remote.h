/**
 ****************************************************************************************************
 * @file        remote.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       ����ң�ؽ��� ��������
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

#ifndef __REMOTE_H
#define __REMOTE_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/******************************************************************************************/
/* �����������ż���ʱ�� ���� */

#define REMOTE_IN_GPIO_PORT                     GPIOA
#define REMOTE_IN_GPIO_PIN                      GPIO_PIN_8
#define REMOTE_IN_GPIO_AF                       GPIO_AF1_TIM1                               /* AF����ѡ�� */
#define REMOTE_IN_GPIO_CLK_ENABLE()             do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PA��ʱ��ʹ�� */


#define REMOTE_IN_TIMX                          TIM1                       
#define REMOTE_IN_TIMX_IRQn                     TIM1_UP_TIM10_IRQn
#define REMOTE_IN_TIMX_IRQHandler               TIM1_UP_TIM10_IRQHandler
#define REMOTE_IN_TIMX_CHY                      TIM_CHANNEL_1                               /* ͨ��Y,  1<= Y <=2*/ 
#define REMOTE_IN_TIMX_CCRY                     REMOTE_IN_TIMX->CCR1
#define REMOTE_IN_TIMX_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIMX ʱ��ʹ�� */

/* TIM1 / TIM8 �ж����Ĳ����жϷ�����,��Ҫ��������,����TIM2~5��ͨ�ö�ʱ��, ����Ҫ���¶��� */
#define REMOTE_IN_TIMX_CC_IRQn                  TIM1_CC_IRQn
#define REMOTE_IN_TIMX_CC_IRQHandler            TIM1_CC_IRQHandler

/******************************************************************************************/

#define RDATA           HAL_GPIO_ReadPin(REMOTE_IN_GPIO_PORT, REMOTE_IN_GPIO_PIN)           /* ������������� */

/* ����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
 * ����ѡ�õ�ң����ʶ����Ϊ0
*/
#define REMOTE_ID       0

extern uint8_t g_remote_cnt;            /* �������µĴ��� */

/******************************************************************************************/

void remote_init(void);                 /* ���⴫��������ͷ���ų�ʼ�� */
uint8_t remote_scan(void);

#endif



