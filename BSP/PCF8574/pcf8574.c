/**
 ****************************************************************************************************
 * @file        pcf8574.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       PCF8574 ��������
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

#include "./BSP/PCF8574/pcf8574.h"
#include "./Middlewares/SYSTEM/delay/delay.h"


/**
 * @brief       ��ʼ��PCF8574
 * @param       ��
 * @retval      0, �ɹ�;
                1, ʧ��;
 */
uint8_t pcf8574_init(void)
{
    uint8_t temp = 0;
    
    GPIO_InitTypeDef gpio_init_struct;
    PCF8574_GPIO_CLK_ENABLE();                               /* ʹ��GPIOBʱ�� */

    gpio_init_struct.Pin = PCF8574_GPIO_PIN;                 /* PB12 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                 /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                     /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;                /* ���� */
    HAL_GPIO_Init(PCF8574_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��IO */

    iic_init();                                              /* IIC��ʼ�� */

    /* ���PCF8574�Ƿ���λ */
    iic_start();
    iic_send_byte(PCF8574_ADDR);                             /* д��ַ */
    temp = iic_wait_ack();                                   /* �ȴ�Ӧ��,ͨ���ж��Ƿ���ACKӦ��,���ж�PCF8574��״̬ */
    iic_stop();                                              /* ����һ��ֹͣ���� */
    pcf8574_write_byte(0XFF);                                /* Ĭ�����������IO����ߵ�ƽ */
    return temp;
}

/**
 * @brief       ��ȡPCF8574��8λIOֵ
 * @param       ��
 * @retval      ����������
 */
uint8_t pcf8574_read_byte(void)
{ 
    uint8_t temp = 0;

    iic_start();
    iic_send_byte(PCF8574_ADDR | 0X01);   /* �������ģʽ */
    iic_wait_ack();
    temp = iic_read_byte(0);
    iic_stop();                           /* ����һ��ֹͣ���� */

    return temp;
}

/**
 * @brief       ��PCF8574д��8λIOֵ
 * @param       data   : Ҫд�������
 * @retval      ��
 */
void pcf8574_write_byte(uint8_t data)
{
    iic_start();  
    iic_send_byte(PCF8574_ADDR | 0X00);   /* ����������ַ0X40,д���� */
    iic_wait_ack();
    iic_send_byte(data);                  /* �����ֽ� */
    iic_wait_ack();
    iic_stop();                           /* ����һ��ֹͣ����  */
    delay_ms(10); 
}

/**
 * @brief       ����PCF8574ĳ��IO�ĸߵ͵�ƽ
 * @param       bit    : Ҫ���õ�IO���,0~7
 * @param       sta    : IO��״̬;0��1
 * @retval      ��
 */
void pcf8574_write_bit(uint8_t bit, uint8_t sta)
{
    uint8_t data;

    data = pcf8574_read_byte();          /* �ȶ���ԭ�������� */

    if (sta == 0)
    {
        data &= ~(1 << bit);
    }
    else
    {
        data |= 1 << bit;
    }

    pcf8574_write_byte(data);            /* д���µ����� */
}

/**
 * @brief       ��ȡPCF8574��ĳ��IO��ֵ
 * @param       bit     : Ҫ��ȡ��IO���, 0~7
 * @retval      ��IO�ڵ�ֵ(״̬, 0/1)
 */
uint8_t pcf8574_read_bit(uint8_t bit)
{
    uint8_t data;

    data = pcf8574_read_byte();          /* �ȶ�ȡ���8λIO��ֵ  */

    if (data & (1 << bit))
    {
        return 1;
    }
    else 
    {
        return 0; 
    }
}  

