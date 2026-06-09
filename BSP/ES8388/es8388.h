/**
 ****************************************************************************************************
 * @file        es8388.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       ES8388 ��������
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
 
#ifndef __ES8388_H
#define __ES8388_H

#include "./Middlewares/SYSTEM/SYS/sys.h"


#define ES8388_ADDR     0x10                                            /* ES8388��������ַ,�̶�Ϊ0x10 */

/******************************************************************************************/

uint8_t es8388_init(void);                                              /* ES8388��ʼ�� */
uint8_t es8388_write_reg(uint8_t reg, uint8_t val);                     /* ES8388д�Ĵ��� */
uint8_t es8388_read_reg(uint8_t reg);                                   /* ES8388���Ĵ��� */
void es8388_sai_cfg(uint8_t fmt, uint8_t len);                          /* ����I2S����ģʽ */
void es8388_hpvol_set(uint8_t volume);                                  /* ���ö������� */
void es8388_spkvol_set(uint8_t volume);                                 /* ������������ */
void es8388_3d_set(uint8_t depth);                                      /* ����3D������ */
void es8388_adda_cfg(uint8_t dacen, uint8_t adcen);                     /* ES8388 DAC/ADC���� */
void es8388_output_cfg(uint8_t o1en, uint8_t o2en);                     /* ES8388 DAC���ͨ������ */
void es8388_mic_gain(uint8_t gain);                                     /* ES8388 MIC��������(MIC PGA����) */
void es8388_alc_ctrl(uint8_t sel, uint8_t maxgain, uint8_t mingain);    /* ES8388 ALC���� */
void es8388_input_cfg(uint8_t in);                                      /* ES8388 ADC���ͨ������ */

#endif


