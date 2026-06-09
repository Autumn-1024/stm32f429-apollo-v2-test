/**
 ****************************************************************************************************
 * @file        ap3216c.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       AP3216C ��������
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

#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/IIC/myiic.h"
#include "./Middlewares/SYSTEM/delay/delay.h"


/**
 * @brief       ��ʼ��AP3216C
 * @param       ��
 * @retval      0, �ɹ�;
                1, ʧ��;
 */
uint8_t ap3216c_init(void)
{
    uint8_t temp = 0;

    iic_init();                             /* ��ʼ��IIC */

    ap3216c_write_one_byte(0x00, 0X04);     /* ��λAP3216C */
    delay_ms(50);                           /* AP33216C��λ����10ms */
    ap3216c_write_one_byte(0x00, 0X03);     /* ����ALS��PS+IR */
    temp = ap3216c_read_one_byte(0X00);     /* ��ȡ�ո�д��ȥ��0X03 */

    if (temp == 0X03)return 0;              /* AP3216C���� */
    else return 1;                          /* AP3216Cʧ�� */
} 

/**
 * @brief       ��ȡAP3216C������
 * @note        ��ȡԭʼ���ݣ�����ALS,PS��IR
 *              ���ͬʱ��ALS,IR+PS�Ļ��������ݶ�ȡ��ʱ����Ҫ����112.5ms
 * @param       ir      : IR������ֵָ��
 * @param       ps      : PS������ֵָ��
 * @param       als     : ALS������ֵָ��
 * @retval      ��
 */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6];
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        buf[i] = ap3216c_read_one_byte(0X0A + i);           /* ѭ����ȡ���д��������� */
    }

    if (buf[0] & 0X80)
    {
        *ir = 0;                                            /* IR_OFλΪ1,��������Ч */
    }
    else 
    {
        *ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);    /* ��ȡIR������������   */
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];                /* ��ȡALS������������   */ 

    if (buf[4] & 0x40) 
    {
        *ps = 0;                                            /* IR_OFλΪ1,��������Ч */
    }
    else
    {
        *ps = ((uint16_t)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);  /* ��ȡPS������������ */
    }
}

/**
 * @brief       AP3216Cд��һ���ֽ�
 * @param       reg     : �Ĵ�����ַ
 * @param       data    : Ҫд�������
 * @retval      0, �ɹ�;
                1, ʧ��;
 */
uint8_t ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    iic_start();
    iic_send_byte(AP3216C_ADDR | 0X00);     /* ����������ַ+д���� */

    if (iic_wait_ack())                     /* �ȴ�Ӧ�� */
    {
        iic_stop();
        return 1;
    }

    iic_send_byte(reg);                     /* д�Ĵ�����ַ */
    iic_wait_ack();                         /* �ȴ�Ӧ�� */
    iic_send_byte(data);                    /* �������� */

    if (iic_wait_ack())                     /* �ȴ�ACK */
    {
        iic_stop();
        return 1;
    }

    iic_stop();
    return 0;
}

/**
 * @brief       AP3216C��ȡһ���ֽ�
 * @param       reg     : �Ĵ�����ַ
 * @retval      ����������
 */
uint8_t ap3216c_read_one_byte(uint8_t reg)
{
    uint8_t res;

    iic_start();
    iic_send_byte(AP3216C_ADDR | 0X00);     /* ����������ַ+д���� */
    iic_wait_ack();                         /* �ȴ�Ӧ�� */
    iic_send_byte(reg);                     /* д�Ĵ�����ַ */
    iic_wait_ack();                         /* �ȴ�Ӧ�� */
    iic_start();                
    iic_send_byte(AP3216C_ADDR | 0X01);     /* ����������ַ+������ */
    iic_wait_ack();                         /* �ȴ�Ӧ�� */
    res = iic_read_byte(0);                 /* ������,����nACK */
    iic_stop();                             /* ����һ��ֹͣ���� */

    return res;  
}


