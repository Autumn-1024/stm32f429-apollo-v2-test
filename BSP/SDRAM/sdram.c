/**
 ****************************************************************************************************
 * @file        sdram.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       SDRAM ��������
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

#include "./BSP/SDRAM/sdram.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./Middlewares/SYSTEM/sys/sys.h"
#include "./BSP/LCD/lcd.h"


SDRAM_HandleTypeDef g_sdram_handle;       /* SDRAM��� */

/**
 * @brief       ��ʼ��SDRAM
 * @param       ��
 * @retval      ��
 */
void sdram_init(void)
{
    FMC_SDRAM_TimingTypeDef sdram_timing;

    g_sdram_handle.Instance = FMC_SDRAM_DEVICE;                              /* SDRAM��BANK5,6 */
    g_sdram_handle.Init.SDBank = FMC_SDRAM_BANK1;                            /* ��һ��SDRAM BANK */
    g_sdram_handle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;      /* ������ */
    g_sdram_handle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;           /* ������ */
    g_sdram_handle.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;        /* ���ݿ���Ϊ16λ */
    g_sdram_handle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;   /* һ��4��BANK */
    g_sdram_handle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;                /* CASΪ3 */
    g_sdram_handle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;/* ʧ��д���� */
    g_sdram_handle.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;            /* SDRAMʱ��=HCLK/2=192M/2=96M=10.4ns */
    g_sdram_handle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;                 /* ʹ��ͻ�� */
    g_sdram_handle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;             /* ��ͨ����ʱ */
    
    sdram_timing.LoadToActiveDelay = 2;                                      /* ����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������ */
    sdram_timing.ExitSelfRefreshDelay = 7;                                   /* �˳���ˢ���ӳ�Ϊ7��ʱ������ */
    sdram_timing.SelfRefreshTime = 6;                                        /* ��ˢ��ʱ��Ϊ6��ʱ������ */
    sdram_timing.RowCycleDelay = 6;                                          /* ��ѭ���ӳ�Ϊ6��ʱ������ */
    sdram_timing.WriteRecoveryTime = 2;                                      /* �ָ��ӳ�Ϊ2��ʱ������ */
    sdram_timing.RPDelay = 2;                                                /* ��Ԥ����ӳ�Ϊ2��ʱ������ */
    sdram_timing.RCDDelay = 2;                                               /* �е����ӳ�Ϊ2��ʱ������ */
    HAL_SDRAM_Init(&g_sdram_handle, &sdram_timing);

    sdram_initialization_sequence();                                         /* ����SDRAM��ʼ������ */

    /**
     * ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
     * COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
     * ����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=192/2=96Mhz,����Ϊ8192(2^13).
     * ����,COUNT=64*1000*96/8192-20=730
     */
    HAL_SDRAM_ProgramRefreshRate(&g_sdram_handle, 730);                      /* ����ˢ��Ƶ�� */
}

/**
 * @brief       ����SDRAM��ʼ������
 * @param       ��
 * @retval      ��
 */
void sdram_initialization_sequence(void)
{
    uint32_t temp = 0;

    /* SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM */
    sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);                /* ʱ������ʹ�� */
    delay_us(500);                                                    /* ������ʱ500us */
    sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);                      /* �����д洢��Ԥ��� */
    sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);          /* ������ˢ�´��� */

    /* ����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
     * bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
     * bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1  |                  /* ����ͻ������:1(������1/2/4/8) */
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL  |                  /* ����ͻ������:����(����������/����) */
              SDRAM_MODEREG_CAS_LATENCY_3          |                  /* ����CASֵ:3(������2/3) */
              SDRAM_MODEREG_OPERATING_MODE_STANDARD|                  /* ���ò���ģʽ:0,��׼ģʽ */
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;                   /* ����ͻ��дģʽ:1,������� */
    sdram_send_cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);              /* ����SDRAM��ģʽ�Ĵ��� */
}

/**
 * @brief       SDRAM�ײ��������������ã�ʱ��ʹ��
 * @note        �˺����ᱻHAL_SDRAM_Init()����
 * @param       hsdram:SDRAM���
 * @retval      
 */
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_FMC_CLK_ENABLE();                  /* ʹ��FMCʱ�� */
    __HAL_RCC_GPIOC_CLK_ENABLE();                /* ʹ��GPIOCʱ�� */
    __HAL_RCC_GPIOD_CLK_ENABLE();                /* ʹ��GPIODʱ�� */
    __HAL_RCC_GPIOE_CLK_ENABLE();                /* ʹ��GPIOEʱ�� */
    __HAL_RCC_GPIOF_CLK_ENABLE();                /* ʹ��GPIOFʱ�� */
    __HAL_RCC_GPIOG_CLK_ENABLE();                /* ʹ��GPIOGʱ�� */
    
    gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;  
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;     /* ���츴�� */
    gpio_init_struct.Pull = GPIO_PULLUP;         /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;    /* ���� */
    gpio_init_struct.Alternate = GPIO_AF12_FMC;  /* ����ΪFMC */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);     /* ��ʼ��PC0,2,3 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);     /* ��ʼ��PD0,1,8,9,10,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                           GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);     /* ��ʼ��PE0,1,7,8,9,10,11,12,13,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
                           GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);     /* ��ʼ��PF0,1,2,3,4,5,11,12,13,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);     /* ��ʼ��PG0,1,2,4,5,8,15 */
}

/**
 * @brief       ��SDRAM��������
 * @param       bankx:0,��BANK5�����SDRAM����ָ��
 * @param             1,��BANK6�����SDRAM����ָ��
 * @param       cmd:ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
 * @param       refresh:��ˢ�´���
 * @param       ����ֵ:0,����;1,ʧ��.
 * @retval      ģʽ�Ĵ����Ķ���
 */
uint8_t sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank = 0;
    FMC_SDRAM_CommandTypeDef command;
    
    if (bankx == 0)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    }
    else if (bankx == 1)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    command.CommandMode = cmd;                                              /* ���� */
    command.CommandTarget = target_bank;                                    /* Ŀ��SDRAM�洢���� */
    command.AutoRefreshNumber = refresh;                                    /* ��ˢ�´��� */
    command.ModeRegisterDefinition = regval;                                /* Ҫд��ģʽ�Ĵ�����ֵ */

    if (HAL_SDRAM_SendCommand(&g_sdram_handle, &command, 0X1000) == HAL_OK) /* ��SDRAM�������� */
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       ��ָ����ַ(addr+BANK5_SDRAM_ADDR)��ʼ,����д��n���ֽ�
 * @param       pbuf  : �ֽ�ָ��
 * @param       addr  : Ҫд��ĵ�ַ
 * @param       n     : Ҫд����ֽ���
 * @retval      ��
*/
void fmc_sdram_write_buffer(uint8_t *pbuf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *(volatile uint8_t*)(BANK5_SDRAM_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       ��ָ����ַ((addr+BANK5_SDRAM_ADDR))��ʼ,��������n���ֽ�
 * @param       pbuf  : �ֽ�ָ��
 * @param       addr  : Ҫ��������ʼ��ַ
 * @param       n     : Ҫд����ֽ���
 * @retval      ��
*/
void fmc_sdram_read_buffer(uint8_t *pbuf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *pbuf++ = *(volatile uint8_t*)(BANK5_SDRAM_ADDR + addr);
        addr++;
    }
}

