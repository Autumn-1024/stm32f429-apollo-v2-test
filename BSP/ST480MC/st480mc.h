/**
 ****************************************************************************************************
 * @file        st480mc.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       ST480MC ��������
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

#ifndef __ST480MC_H
#define __ST480MC_H

#include "./Middlewares/SYSTEM/sys/sys.h"

/******************************************************************************************/

#define ST480MC_ADDR            0X0C    /* ST480MC IIC������ַ(A0 = 0ʱ) */
#define ST480MC_RESET           0XF0    /* ST480MC ��λ���� */
#define ST480MC_READ_REG        0X50    /* ST480MC ���Ĵ��� */
#define ST480MC_WRITE_REG       0X60    /* ST480MC д�Ĵ��� */
#define ST480MC_READ_DATA       0X4F    /* ST480MC ��ȡȫ������(zxyt) */
#define ST480MC_BURST_MODE      0X1F    /* ST480MC ͻ��ģʽ��ȡ����(zxyt) */
#define ST480MC_SINGLE_MODE     0X3F    /* ST480MC ���β�����ȡ����(zxyt) */

#define ST480MC_SENS_XY         667     /* X,Y ��������; ��λ:  LSB/Gauss */
#define ST480MC_SENS_Z          400     /* Z   ��������  ��λ:  LSB/Gauss */

/******************************************************************************************/

uint8_t st480mc_init(void);                                                                             /* ��ʼ�� */
uint8_t st480mc_read_nbytes(uint8_t addr, uint8_t length, uint8_t *buf);                                /* ��N�ֽ�,���Է������� */
uint16_t st480mc_read_register(uint8_t reg);                                                            /* ���Ĵ��� */
uint8_t st480mc_write_register(uint8_t reg, uint16_t data);                                             /* д�Ĵ��� */

uint8_t st480mc_read_magdata(int16_t *pmagx, int16_t *pmagy, int16_t *pmagz);                           /* ��ȡ����ԭʼ����(����) */
uint8_t st480mc_read_temperature(float *ptemp);                                                         /* ��ȡ�¶ȴ�����ֵ(����,��) */
uint8_t st480mc_read_magdata_average(int16_t *pmagx, int16_t *pmagy, int16_t *pmagz, uint8_t times);    /* ��ȡ����ԭʼ����(ƽ��ֵ) */

#endif






