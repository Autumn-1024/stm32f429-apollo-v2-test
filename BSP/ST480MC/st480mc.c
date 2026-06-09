/**
 ****************************************************************************************************
 * @file        st480mc.c
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

#include "./BSP/IIC/myiic.h"
#include "./BSP/ST480MC/st480mc.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./Middlewares/SYSTEM/usart/usart.h"
#include <math.h>


/**
 * @brief       ��ST480MC��ȡN�ֽ�����
 * @note        ST480MC�������, Ҳ���øú���ʵ��(��������������, Ҳ����һ��״̬�Ĵ�����Ҫ��ȡ)
 * @param       addr  : �Ĵ�����ַ/����
 * @param       length: ��ȡ����
 * @param       buf   : ���ݴ洢buf
 * @retval      0, �����ɹ�
 *              ����, ����ʧ��
 */
uint8_t st480mc_read_nbytes(uint8_t addr, uint8_t length, uint8_t *buf)
{
    uint8_t i;

    iic_start();
    iic_send_byte((ST480MC_ADDR << 1) | 0X00);  /* IIC��ַ���λ��0, ��ʾд�� */

    if (iic_wait_ack())
    {
        iic_stop();
        return 1;
    }

    iic_send_byte(addr);                        /* д��ַ/���� */
    iic_wait_ack();

    iic_start();
    iic_send_byte((ST480MC_ADDR << 1) | 0x01);  /* IIC��ַ���λ��1, ��ʾ��ȡ */
    iic_wait_ack();

    for (i = 0; i < length; i++)                /* ѭ����ȡ ���� */
    {
        buf[i] = iic_read_byte(1);
    }

    iic_stop();
    return 0;
}

/**
 * @brief       ST480MCд�Ĵ���
 * @param       reg  : �Ĵ�����ַ
 * @param       data : д���ֵ
 * @retval      0, �����ɹ�
 *              ����, ����ʧ��
 */
uint8_t st480mc_write_register(uint8_t reg, uint16_t data)
{
    iic_start();
    iic_send_byte((ST480MC_ADDR << 1) | 0X00);  /* IIC��ַ���λ��0, ��ʾд�� */
    if (iic_wait_ack())
    {
        iic_stop();
        return 1;
    }

    iic_send_byte(ST480MC_WRITE_REG);           /* ����д�Ĵ������� */
    iic_wait_ack();

    iic_send_byte(data >> 8);                   /* ���͸��ֽ����� */
    iic_wait_ack();

    iic_send_byte(data & 0XFF);                 /* ���͵��ֽ����� */
    iic_wait_ack();

    iic_send_byte(reg << 2);                    /* ���ͼĴ�����ַ(��2λĬ����0) */
    iic_wait_ack();

    iic_stop();

    return 0;
}

/**
 * @brief       ST480MC���Ĵ���
 * @param       reg  : �Ĵ�����ַ
 * @retval      ��ȡ����ֵ
 *              0XFFFF, ����ܱ�ʾ����
 */
uint16_t st480mc_read_register(uint8_t reg)
{
    uint8_t buf[3];
    uint8_t i;

    iic_start();
    iic_send_byte((ST480MC_ADDR << 1) | 0X00);  /* IIC��ַ���λ��0, ��ʾд�� */
    iic_wait_ack();

    iic_send_byte(ST480MC_READ_REG);            /* ���Ͷ��Ĵ������� */
    iic_wait_ack();

    iic_send_byte(reg << 2);                    /* ���ͼĴ�����ַ(��2λĬ����0) */
    iic_wait_ack();
    
    iic_start();
    iic_send_byte((ST480MC_ADDR << 1) | 0x01);  /* IIC��ַ���λ��1, ��ʾ��ȡ */
    iic_wait_ack();
    
    for (i = 0; i < 3; i++)                     /* ѭ����ȡ ���� */
    {
        buf[i] = iic_read_byte(1);
    }

    iic_stop();
    
    if (buf[0] & 0X10)
    {
        return 0XFFFF;
    }

    return ((uint16_t)buf[1] << 8) | buf[2];    /* ���ؼĴ�������(16λ) */
}

/**
 * @brief       ��ʼ��ST480MC�ӿ�
 * @param       ��
 * @retval      0, �ɹ�
 *              ����, �쳣
 */
uint8_t st480mc_init(void)
{
    uint8_t status;
    uint8_t res = 0XFF;
    uint8_t retry = 10;
    
    iic_init();
    
    while ((retry --) && res)                                   /* ��γ���, ֱ�� res == 0, ���ȵ�ST480MC�� ACK��Ӧ */
    {
        res = st480mc_read_nbytes(ST480MC_RESET, 1, &status);   /* ���͸�λ����,����״̬�Ĵ��� */
        delay_ms(20);
    }

    return res;
}

/**
 * @brief       ST480MCʹ�õ���ģʽ ��ȡһ�δ���������(ֻ�� X,Y,Z������),��ȡһ������15ms����!
 * @note        һ���Զ�ȡXYZT����Ҫʱ��Ϊ�� Tstby + Tactive + m * Tconvm + Tconvt
 *              Tstby   : ��IDLE״̬������״̬ʱ��, 250us
 *              Tactive : ����������״̬ʱ��, 8us
 *              Tconvm  : ����ת��ʱ��, (2 + 2^DIG_FILT) * 2^OSR * 0.064 ms, ���� DIG_FILTĬ��Ϊ2, OSRĬ��Ϊ0
 *              Tconvt  : �¶�ת��ʱ��, 2^OSR2 * 0.192ms, OSR2Ĭ��Ϊ0
 *              ���, һ���Զ�ȡXYZT������ʱ��Ϊ = 0.25 + 0.008 + 3 * (2 + 2^2) * 2^0 * 0.064 + 2^0 * 0.192 �� 1.6ms
 *              ����, ST480MC�и�IDLE TO DATA READY��ʱ��: Tconv, ���һ����Ҫ16ms, ����, һ��ת�������ݶ�ȡ, ����Ҫ18ms����
 *
 * @param       pmagx    : X�������ԭʼֵָ��
 * @param       pmagy    : Y�������ԭʼֵָ��
 * @param       pmagz    : Z�������ԭʼֵָ��
 * @retval      0, �ɹ�
 *              ����, �쳣
 */
uint8_t st480mc_read_magdata(int16_t *pmagx, int16_t *pmagy, int16_t *pmagz)
{
    uint8_t buf[7];

    st480mc_read_nbytes(ST480MC_SINGLE_MODE & 0XFE, 1, buf);    /* ���͵��β�������(�������¶�) */
    delay_ms(15);                                               /* ��ʱ15ms,��������ȡ��ȡ���� */
    st480mc_read_nbytes(ST480MC_READ_DATA & 0XFE, 7, buf);      /* ���Ͷ�ȡ��������(����ȡ�¶�) */

    if (buf[0] & 0X10)  /* ��ȡ�����쳣 */
    {
        return buf[0];  /* ���������� */
    }
    else
    {
        *pmagx = (short int)(buf[1] << 8) | buf[2];             /* ������� */
        *pmagy = (short int)(buf[3] << 8) | buf[4];             /* ������� */
        *pmagz = (short int)(buf[5] << 8) | buf[6];             /* ������� */
    }
    return 0;
}

/**
 * @brief       ST480MCʹ�õ���ģʽ ��ȡһ���¶ȴ�����, ��ȡһ������15ms����!
 * @note        һ���Զ�ȡXYZT����Ҫʱ��Ϊ�� Tstby + Tactive + m * Tconvm + Tconvt
 *              Tstby   : ��IDLE״̬������״̬ʱ��, 250us
 *              Tactive : ����������״̬ʱ��, 8us
 *              Tconvm  : ����ת��ʱ��, (2 + 2^DIG_FILT) * 2^OSR * 0.064 ms, ���� DIG_FILTĬ��Ϊ2, OSRĬ��Ϊ0
 *              Tconvt  : �¶�ת��ʱ��, 2^OSR2 * 0.192ms, OSR2Ĭ��Ϊ0
 *              ���, һ���Զ�ȡXYZT������ʱ��Ϊ = 0.25 + 0.008 + 3 * (2 + 2^2) * 2^0 * 0.064 + 2^0 * 0.192 �� 1.6ms
 *              ����, ST480MC�и�IDLE TO DATA READY��ʱ��: Tconv, ���һ����Ҫ16ms, ����, һ��ת�������ݶ�ȡ, ����Ҫ18ms����
 *
 * @param       ptemp   : �¶�ֵ(��)ָ��
 * @retval      0, �ɹ�
 *              ����, �쳣
 */
uint8_t st480mc_read_temperature(float *ptemp)
{
    uint8_t buf[9];

    st480mc_read_nbytes(ST480MC_SINGLE_MODE, 1, buf);    /* ���͵��β�������(���¶�) */
    delay_ms(15);                                        /* ��ʱ15ms,��������ȡ��ȡ���� */
    st480mc_read_nbytes(ST480MC_READ_DATA, 9, buf);      /* ���Ͷ�ȡ��������(���¶�) */

    if (buf[0] & 0X10)  /* ��ȡ�����쳣 */
    {
        return buf[0];  /* ���������� */
    }
    else
    {
        *ptemp = (uint16_t)(buf[1] << 8) | buf[2];      /* �õ��¶ȴ�����ԭʼֵ */
        *ptemp = (*ptemp - 46244) / 45.2f + 25;         /* ����ԭ���ṩ�ļ��㹫ʽ,����ɡ� */
    }
    return 0;
}

/**
 * @brief       ST480MC��ȡ����������(ֻ�� X,Y,Z������),��times��ȡƽ��
 * @note        ��������ʱ �� 15 * times ms
 * @param       pmagx    : X�������ԭʼֵָ��
 * @param       pmagy    : Y�������ԭʼֵָ��
 * @param       pmagz    : Z�������ԭʼֵָ��
 * @param       times    : ��ȡ���ٴ�ȡƽ��
 * @retval      0, �ɹ�
 *              ����, �쳣
 */
uint8_t st480mc_read_magdata_average(int16_t *pmagx, int16_t *pmagy, int16_t *pmagz, uint8_t times)
{
    uint8_t i = 0;
    uint8_t error_cnt = 0;
    int32_t magx = 0;
    int32_t magy = 0;
    int32_t magz = 0;

    while (i < times)                                          /* ������ȡtimes�� */
    {
        if (st480mc_read_magdata(pmagx, pmagy, pmagz) == 0)    /* ��ȡ�����Ƿ�����? */
        {
            magx += *pmagx;                                    /* �ۼ� */
            magy += *pmagy;
            magz += *pmagz;

            i++;
            error_cnt = 0;
        }
        else
        {
            error_cnt++;                                       /* ��������� */
            delay_ms(10);
            if (error_cnt > 100)                               /* ����100�γ���, ֱ�ӷ����쳣 */
            {
                return 0XFF;
            }
        }
    }

    *pmagx = magx / times;                                     /* ȡƽ��ֵ */
    *pmagy = magy / times;                                     /* ȡƽ��ֵ */
    *pmagz = magz / times;                                     /* ȡƽ��ֵ */

    return 0;
}





