/**
 ****************************************************************************************************
 * @file        nand.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       NAND FLASH ��������
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

#include <stdio.h>
#include "./BSP/NAND/nand.h"
#include "./Middlewares/SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"


NAND_HandleTypeDef g_nand_handle;     /* NAND FLASH��� */
nand_attriute nand_dev;               /* nand��Ҫ�����ṹ�� */

/**
 * @brief       ��ʼ��NAND FLASH
 * @param       ��
 * @retval      ��
 */
uint8_t nand_init(void)
{
    FMC_NAND_PCC_TimingTypeDef comspacetiming, attspacetiming;
                                              
    g_nand_handle.Instance = FMC_NAND_DEVICE;
    g_nand_handle.Init.NandBank = FMC_NAND_BANK3;                          /* NAND����BANK3�� */
    g_nand_handle.Init.Waitfeature = FMC_NAND_PCC_WAIT_FEATURE_DISABLE;    /* �رյȴ����� */
    g_nand_handle.Init.MemoryDataWidth = FMC_NAND_PCC_MEM_BUS_WIDTH_8;     /* 8λ���ݿ��� */
    g_nand_handle.Init.EccComputation = FMC_NAND_ECC_DISABLE;              /* ��ʹ��ECC */
    g_nand_handle.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_2048BYTE;      /* ECCҳ��СΪ2k */
    g_nand_handle.Init.TCLRSetupTime = 0;                                  /* ����TCLR(tCLR=CLE��RE����ʱ)=(TCLR+TSET+2)*THCLK,THCLK=1/192M=5.2ns */
    g_nand_handle.Init.TARSetupTime = 1;                                   /* ����TAR(tAR=ALE��RE����ʱ)=(TAR+TSET+2)*THCLK,THCLK=1/192M=5.2n */
   
    comspacetiming.SetupTime = 2;           /* ����ʱ�� */
    comspacetiming.WaitSetupTime = 3;       /* �ȴ�ʱ�� */
    comspacetiming.HoldSetupTime = 2;       /* ����ʱ�� */
    comspacetiming.HiZSetupTime = 1;        /* ����̬ʱ�� */
    
    attspacetiming.SetupTime = 2;           /* ����ʱ�� */
    attspacetiming.WaitSetupTime = 3;       /* �ȴ�ʱ�� */
    attspacetiming.HoldSetupTime = 2;       /* ����ʱ�� */
    attspacetiming.HiZSetupTime = 1;        /* ����̬ʱ�� */
    
    HAL_NAND_Init(&g_nand_handle, &comspacetiming, &attspacetiming); 
    nand_reset();                           /* ��λNAND */
    delay_ms(100);
    nand_dev.id = nand_readid();            /* ��ȡID */
    nand_modeset(4);                        /* ����ΪMODE4,����ģʽ */

    if (nand_dev.id == MT29F16G08ABABA)     /* NANDΪMT29F16G08ABABA */
    {
        nand_dev.page_totalsize = 4320;     /* nandһ��page���ܴ�С������spare���� */
        nand_dev.page_mainsize = 4096;      /* nandһ��page����Ч��������С */
        nand_dev.page_sparesize = 224;      /* nandһ��page��spare����С */
        nand_dev.block_pagenum = 128;       /* nandһ��block��������page��Ŀ */
        nand_dev.plane_blocknum = 2048;     /* nandһ��plane��������block��Ŀ */
        nand_dev.block_totalnum = 4096;     /* nand����block��Ŀ */
    }
    else if (nand_dev.id == MT29F4G08ABADA) /* NANDΪMT29F4G08ABADA */
    {
        nand_dev.page_totalsize = 2112;     /* nandһ��page���ܴ�С������spare���� */
        nand_dev.page_mainsize = 2048;      /* nandһ��page����Ч��������С */
        nand_dev.page_sparesize = 64;       /* nandһ��page��spare����С */
        nand_dev.block_pagenum = 64;        /* nandһ��block��������page��Ŀ */
        nand_dev.plane_blocknum = 2048;     /* nandһ��plane��������block��Ŀ */
        nand_dev.block_totalnum = 4096;     /* nand����block��Ŀ */
    }
    else return 1;                          /* ���󣬷��� */

    return 0;
}

/**
 * @brief       NAND FALSH�ײ�����,�������ã�ʱ��ʹ��
 * @note        �˺����ᱻHAL_nand_init()����
 * @param       hnand  : nand���
 * @retval      ��
 */
void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_FMC_CLK_ENABLE();                       /* ʹ��FMCʱ�� */
    __HAL_RCC_GPIOD_CLK_ENABLE();                     /* ʹ��GPIODʱ�� */
    __HAL_RCC_GPIOE_CLK_ENABLE();                     /* ʹ��GPIOEʱ�� */
    __HAL_RCC_GPIOG_CLK_ENABLE();                     /* ʹ��GPIOGʱ�� */

    gpio_init_struct.Pin = NAND_RB_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;          /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;              /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;         /* ���� */
    HAL_GPIO_Init(NAND_RB_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = GPIO_PIN_9;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;          /* ���� */
    gpio_init_struct.Pull = GPIO_NOPULL;              /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;         /* ���� */
    gpio_init_struct.Alternate = GPIO_AF12_FMC;       /* ����ΪFMC */
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);  

    /* ��ʼ��PD0,1,4,5,11,12,14,15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | \
                           GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_struct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);

    /* ��ʼ��PE7,8,9,10*/
    gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
}

/**
 * @brief       ����NAND�ٶ�ģʽ
 * @param       mode    : 0~5, ��ʾ�ٶ�ģʽ
 * @retval      0,�ɹ�; ����,ʧ��
 */
uint8_t nand_modeset(uint8_t mode)
{   
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_FEATURE; /* ���������������� */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0X01;        /* ��ַΪ0X01,����mode */
    *(volatile uint8_t *)NAND_ADDRESS = mode;                      /* P1����,����mode */
    *(volatile uint8_t *)NAND_ADDRESS = 0;
    *(volatile uint8_t *)NAND_ADDRESS = 0;
    *(volatile uint8_t *)NAND_ADDRESS = 0;

    if (nand_wait_for_ready() == NSTA_READY)
    {
        return 0;        /* �ɹ� */
    }
    else return 1;       /* ʧ�� */
}

/**
 * @brief       ��ȡNAND FLASH��ID
 * @note        ��ͬ��NAND���в�ͬ��������Լ���ʹ�õ�NAND FALSH�����ֲ�����д����
 * @param       ��
 * @retval      NAND FLASH��IDֵ
 */
uint32_t nand_readid(void)
{
    uint8_t deviceid[5]; 
    uint32_t id;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = nand_readID; /* ���Ͷ�ȡID���� */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0X00;

    /* IDһ����5���ֽ� */
    deviceid[0] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[1] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[2] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[3] = *(volatile uint8_t *)NAND_ADDRESS;
    deviceid[4] = *(volatile uint8_t *)NAND_ADDRESS;

    /* þ���NAND FLASH��IDһ��5���ֽڣ�����Ϊ�˷�������ֻȡ4���ֽ����һ��32λ��IDֵ
       ����NAND FLASH�������ֲᣬֻҪ��þ���NAND FLASH����ôһ���ֽ�ID�ĵ�һ���ֽڶ���0X2C
       �������ǾͿ����������0X2C��ֻȡ�������ֽڵ�IDֵ��*/
    id = ((uint32_t)deviceid[1]) << 24 | ((uint32_t)deviceid[2]) << 16 | ((uint32_t)deviceid[3]) << 8 | deviceid[4];

    return id;
}

/**
 * @brief       ��NAND״̬
 * @param       ��
 * @retval      NAND״ֵ̬
 *              bit0:0,�ɹ�; 1,����(���/����/READ)
                bit6:0,Busy; 1,Ready
 */
uint8_t nand_readstatus(void)
{
    volatile uint8_t data = 0;

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_READSTA;  /* ���Ͷ�״̬���� */
    nand_delay(NAND_TWHR_DELAY);                                    /* �ȴ�tWHR,�ٶ�ȡ״̬�Ĵ��� */
    data = *(volatile uint8_t *)NAND_ADDRESS;                       /* ��ȡ״ֵ̬ */

    return data;
}

/**
 * @brief       �ȴ�NAND׼����
 * @param       ��
 * @retval      NSTA_TIMEOUT  �ȴ���ʱ��
 *              NSTA_READY    �Ѿ�׼����
 */
uint8_t nand_wait_for_ready(void)
{
    uint8_t status = 0;
    volatile uint32_t time = 0;

    while (1)                       /* �ȴ�ready */
    {
        status = nand_readstatus(); /* ��ȡ״ֵ̬ */

        if (status & NSTA_READY)break;

        time++;

        if (time >= 0X1FFFFFFF)
        {
            return NSTA_TIMEOUT;    /* ��ʱ */
        }
    }

    return NSTA_READY;              /* ׼���� */
}

/**
 * @brief       ��λNAND
 * @param       ��
 * @retval      0,�ɹ�; ����,ʧ��
 */
uint8_t nand_reset(void)
{
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_RESET; /* ��λNAND */

    if (nand_wait_for_ready() == NSTA_READY)
    {
        return 0;                   /* ��λ�ɹ� */
    }
    else return 1;                  /* ��λʧ�� */
}

/**
 * @brief       �ȴ�RB�ź�Ϊĳ����ƽ
 * @param       rb      : 0,�ȴ�RB==0;
 *                        1,�ȴ�RB==1;
 * @retval      0,�ɹ�; 1,��ʱ
 */
uint8_t nand_waitrb(volatile uint8_t rb)
{
    volatile uint32_t time = 0;

    while (time < 0X1FFFFFF)
    {
        time++;

        if (NAND_RB == rb)
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       NAND��ʱ
 * @note        һ��i++������Ҫ4ns
 * @param       i       : �ȴ���ʱ��
 * @retval      ��
 */
void nand_delay(volatile uint32_t i)
{
    while (i > 0)
    {
        i--;
    }
}

/**
 * @brief       ��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���)
 * @param       pagenum         : Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
 * @param       *pbuffer        : ָ�����ݴ洢��
 * @param       numbyte_to_read : ��ȡ�ֽ���(���ܿ�ҳ��)
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_readpage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read)
{
    volatile uint16_t i = 0;
    uint8_t res = 0;
    uint8_t eccnum = 0;     /* ��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc */
    uint8_t eccstart = 0;   /* ��һ��ECCֵ�����ĵ�ַ��Χ */
    uint8_t errsta = 0;
    uint8_t *p;

    *(volatile uint8_t *)(NAND_ADDRESS|NAND_CMD) = NAND_AREA_A;
    /* ���͵�ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_TRUE1;

    /*�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
       ��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
       �����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
       ��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶�������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
       ���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������ */
    res = nand_waitrb(0);                                                               /* �ȴ�RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;                                                            /* ��ʱ�˳� */
    }

    /* ����2�д����������ж�NAND�Ƿ�׼���õ� */
    res = nand_waitrb(1);                                                               /* �ȴ�RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;                                                            /* ��ʱ�˳� */
    }

    if (numbyte_to_read % NAND_ECC_SECTOR_SIZE)                                         /* ����NAND_ECC_SECTOR_SIZE����������������ECCУ�� */
    { 
        /* ��ȡNAND FLASH�е�ֵ */
        for (i = 0;i < numbyte_to_read; i++)
        {
            *(volatile uint8_t *)pbuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
        }
    }
    else
    {
        eccnum = numbyte_to_read / NAND_ECC_SECTOR_SIZE;                                /* �õ�ecc������� */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;
        p = pbuffer;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank2_3->PCR3 |= 1 << 6;                                                /* ʹ��ECCУ�� */

            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)                                  /* ��ȡNAND_ECC_SECTOR_SIZE������ */
            {
                *(volatile uint8_t *)pbuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
            }

            while (!(FMC_Bank2_3 -> SR3 & (1 << 6)));                                   /* �ȴ�FIFO�� */

            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3 -> ECCR3;                  /* ��ȡӲ��������ECCֵ */
            FMC_Bank2_3 -> PCR3 &= ~(1 << 6);                                           /* ��ֹECCУ�� */
        } 

        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;                               /* ��spare����0X10λ�ÿ�ʼ��ȡ֮ǰ�洢��eccֵ */
        nand_delay(NAND_TRHW_DELAY);                                                    /* �ȴ�tRHW */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X05;                          /* �����ָ�� */
        /* ���͵�ַ */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0XE0;                          /* ��ʼ������ */

        nand_delay(NAND_TWHR_DELAY);                                                    /* �ȴ�tWHR */
        pbuffer = (uint8_t *)&nand_dev.ecc_rdbuf[eccstart];
  
        for (i = 0; i < 4 * eccnum; i++)                                                /* ��ȡ�����ECCֵ */
        {
            *(volatile uint8_t *)pbuffer++ = *(volatile uint8_t *)NAND_ADDRESS;
        }
  
        for (i = 0; i < eccnum; i++)                                                    /* ����ECC */
        {
            if (nand_dev.ecc_rdbuf[i + eccstart] != nand_dev.ecc_hdbuf[i + eccstart])   /* �����,��ҪУ�� */
            {
                printf("err hd,rd:0x%x,0x%x\r\n", nand_dev.ecc_hdbuf[i + eccstart], nand_dev.ecc_rdbuf[i + eccstart]); 
                printf("eccnum,eccstart:%d,%d\r\n", eccnum, eccstart);
                printf("PageNum,ColNum:%d,%d\r\n", pagenum, colnum);
                res = nand_ecc_correction(p + NAND_ECC_SECTOR_SIZE * i, nand_dev.ecc_rdbuf[i + eccstart], nand_dev.ecc_hdbuf[i + eccstart]);/* ECCУ�� */

                if (res)
                {
                    errsta = NSTA_ECC2BITERR;                                           /* ���2BIT������ECC���� */
                }
                else
                {
                    errsta = NSTA_ECC1BITERR;                                           /* ���1BIT ECC���� */
                }
            }
        }
    }

    if (nand_wait_for_ready() != NSTA_READY)
    {
        errsta = NSTA_ERROR;    /* ʧ�� */
    }

    return errsta;              /* �ɹ� */
}

/**
 * @brief       ��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���),���Ա�(FTL����ʱ��Ҫ)
 * @param       pagenum         : Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
 * @param       cmpval          : Ҫ�Աȵ�ֵ,��uint32_tΪ��λ
 * @param       numbyte_to_read : ��ȡ����(��4�ֽ�Ϊ��λ,���ܿ�ҳ��)
 * @param       numbyte_equal   : �ӳ�ʼλ�ó�����CmpValֵ��ͬ�����ݸ���
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_readpagecomp(uint32_t pagenum, uint16_t colnum, uint32_t cmpval, uint16_t numbyte_to_read, uint16_t *numbyte_equal)
{
    uint16_t i = 0;
    uint8_t res = 0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_A;
    /* ���͵�ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_TRUE1;

    /**
     * �������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
     * ��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
     * �����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
     * ��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶�������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
     * ���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
     */
    res = nand_waitrb(0);                                   /* �ȴ�RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;                                /* ��ʱ�˳� */
    }

    /* ����2�д����������ж�NAND�Ƿ�׼���õ� */
    res = nand_waitrb(1);                                   /* �ȴ�RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;                                /* ��ʱ�˳� */
    }

    for (i = 0; i < numbyte_to_read; i++)                   /* ��ȡ����,ÿ�ζ�4�ֽ� */
    {
        if (*(volatile uint32_t *)NAND_ADDRESS != cmpval)break;  /* ������κ�һ��ֵ,��cmpval�����,���˳�. */
    }

    *numbyte_equal = i;                                     /* ��cmpvalֵ��ͬ�ĸ��� */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                                  /* ʧ�� */
    }

    return 0;                                               /* �ɹ� */
}

/**
 * @brief       ��NANDһҳ��д��ָ�����ֽڵ�����(main����spare��������ʹ�ô˺���)
 * @param       pagenum         : Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
 * @param       pbuffer         : ָ�����ݴ洢��
 * @param       numbyte_to_write: Ҫд����ֽ�������ֵ���ܳ�����ҳʣ���ֽ���������
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_writepage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    volatile uint16_t i = 0;  
    uint8_t res = 0;
    uint8_t eccnum = 0;                                                 /* ��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc */
    uint8_t eccstart = 0;                                               /* ��һ��ECCֵ�����ĵ�ַ��Χ */

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    /* ���͵�ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    nand_delay(NAND_TADL_DELAY);                                        /* �ȴ�tADL */

    if (numbyte_to_write % NAND_ECC_SECTOR_SIZE)                        /* ����NAND_ECC_SECTOR_SIZE����������������ECCУ�� */
    {  
        for (i = 0; i < numbyte_to_write; i++)                          /* д������ */
        {
            *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
        }
    }
    else
    {
        eccnum = numbyte_to_write / NAND_ECC_SECTOR_SIZE;               /* �õ�ecc������� */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank2_3 -> PCR3 |= 1 << 6;                              /* ʹ��ECCУ�� */

            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)                  /* д��NAND_ECC_SECTOR_SIZE������ */
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
            }
            while (!(FMC_Bank2_3 -> SR3 & (1 << 6)));                   /* �ȴ�FIFO�� */

            nand_dev.ecc_hdbuf[res+eccstart] = FMC_Bank2_3 -> ECCR3;    /* ��ȡӲ��������ECCֵ */

            FMC_Bank2_3 -> PCR3 &= ~(1 << 6);                           /* ��ֹECCУ�� */
        }

        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;               /*����д��ECC��spare����ַ */
        nand_delay(NAND_TADL_DELAY);                                    /* �ȴ�tADL */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X85;          /* ���дָ�� */

        /* ���͵�ַ */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        nand_delay(NAND_TADL_DELAY);                                    /* �ȴ�tADL */
        pbuffer = (uint8_t *)&nand_dev.ecc_hdbuf[eccstart];

        for (i = 0; i < eccnum; i++)                                    /* д��ECC */
        { 
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
            }
        }
    }
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1; 
    delay_us(NAND_TPROG_DELAY);                                         /* �ȴ�tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                                              /* ʧ�� */
    }

    return 0;                                                           /* �ɹ� */
}

/**
 * @brief       ��NANDһҳ�е�ָ����ַ��ʼ,д��ָ�����ȵĺ㶨����
 * @param       pagenum         : Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
 * @param       cval            : Ҫд���ָ������
 * @param       numbyte_to_write: Ҫд����ֽ���(��4�ֽ�Ϊ��λ)
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_write_pageconst(uint32_t pagenum, uint16_t colnum, uint32_t cval, uint16_t numbyte_to_write)
{
    uint16_t i = 0;  
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    /* ���͵�ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);

    nand_delay(NAND_TADL_DELAY);                /* �ȴ�tADL */

    for (i = 0; i < numbyte_to_write; i++)      /* д������,ÿ��д4�ֽ� */
    {
        *(volatile uint32_t *)NAND_ADDRESS = cval;
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1; 
    delay_us(NAND_TPROG_DELAY);                 /* �ȴ�tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                      /* ʧ��  */
    }

    return 0;                                   /* �ɹ� */
}

/**
 * @brief       ��һҳ���ݿ�������һҳ,��д��������
 * @note        Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
 * @param       source_pagenum  : Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       dest_pagenum    : Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_copypage_withoutwrite(uint32_t source_pagenum, uint32_t dest_pagenum)
{
    uint8_t res = 0;
    uint16_t source_block = 0, dest_block = 0;
    /* �ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane�� */
    source_block = source_pagenum / nand_dev.block_pagenum;
    dest_block = dest_pagenum / nand_dev.block_pagenum;

    if ((source_block % 2) != (dest_block % 2))
    {
        return NSTA_ERROR;  /* ����ͬһ��plane�� */
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0; /* ��������0X00 */
    /* ����Դҳ��ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)source_pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD1;/* ��������0X35 */
    /**
     *   �������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
     *   ��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
     *   �����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
     *   ��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶�������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
     *   ���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
    */
    res = nand_waitrb(0);        /* �ȴ�RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;     /* ��ʱ�˳� */
    }
    /* ����2�д����������ж�NAND�Ƿ�׼���õ� */
    res = nand_waitrb(1);        /* �ȴ�RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;    /* ��ʱ�˳� */
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2;  /* ��������0X85 */
    /* ����Ŀ��ҳ��ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)dest_pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD3;    /* ��������0X10 */
    delay_us(NAND_TPROG_DELAY);                                             /* �ȴ�tPROG  */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                                                  /* NANDδ׼���� */
    }

    return 0;                                                               /* �ɹ� */
}

/**
 * @brief       ��һҳ���ݿ�������һҳ,���ҿ���д������
 * @note        Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
 * @param       source_pagenum  : Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       dest_pagenum    : Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : ҳ���е�ַ,��Χ:0~(page_totalsize-1)
 * @param       pbuffer         : Ҫд�������
 * @param       numbyte_to_write: Ҫд������ݸ���
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_copypage_withwrite(uint32_t source_pagenum, uint32_t dest_pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    uint8_t res = 0;
    volatile uint16_t i = 0;
    uint16_t source_block = 0, dest_block = 0;  
    uint8_t eccnum = 0;                                             /* ��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc */
    uint8_t eccstart = 0;                                           /* ��һ��ECCֵ�����ĵ�ַ��Χ */
    /* �ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane�� */
    source_block = source_pagenum / nand_dev.block_pagenum;
    dest_block = dest_pagenum / nand_dev.block_pagenum;

    if ((source_block % 2) != (dest_block % 2))
    {
        return NSTA_ERROR;                                           /* ����ͬһ��plane�� */
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0;  /* ��������0X00 */
    /* ����Դҳ��ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)source_pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD1;  /* ��������0X35 */

    /**
     * �������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
     * ��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
     * �����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
     * ��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶�������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
     * ���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
     */
    res = nand_waitrb(0);                                              /* �ȴ�RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;                                           /* ��ʱ�˳� */
    }

    /* ����2�д����������ж�NAND�Ƿ�׼���õ� */
    res = nand_waitrb(1);                                              /* �ȴ�RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;                                           /* ��ʱ�˳� */
    }
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2;    /* ��������0X85 */
    /* ����Ŀ��ҳ��ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS  |NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)dest_pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 16); 
    /* ����ҳ���е�ַ */
    nand_delay(NAND_TADL_DELAY);                                        /* �ȴ�tADL */

    if (numbyte_to_write % NAND_ECC_SECTOR_SIZE)                        /* ����NAND_ECC_SECTOR_SIZE����������������ECCУ�� */
    {  
        for (i = 0; i < numbyte_to_write; i++)                          /* д������ */
        {
            *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
        }
    }
    else
    {
        eccnum = numbyte_to_write / NAND_ECC_SECTOR_SIZE;               /* �õ�ecc������� */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank2_3 -> PCR3 |= 1 << 6;                              /* ʹ��ECCУ�� */

            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)                  /* д��NAND_ECC_SECTOR_SIZE������ */
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
            }

            while (!(FMC_Bank2_3 -> SR3 & (1<<6)));                     /* �ȴ�FIFO�� */

            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3 -> ECCR3;  /* ��ȡӲ��������ECCֵ */

            FMC_Bank2_3 -> PCR3 &= ~(1 << 6);                           /* ��ֹECCУ�� */
        }

        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;               /* ����д��ECC��spare����ַ */
        nand_delay(NAND_TADL_DELAY);                                    /* �ȴ�tADL */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = 0X85;          /* ���дָ�� */
        /* ���͵�ַ */
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        nand_delay(NAND_TADL_DELAY);                                    /* �ȴ�tADL */

        pbuffer = (uint8_t *)&nand_dev.ecc_hdbuf[eccstart];

        for (i = 0; i < eccnum; i++)                                    /* д��ECC */
        { 
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t *)NAND_ADDRESS = *(volatile uint8_t *)pbuffer++;
            }
        }
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD3;     /* ��������0X10 */
    delay_us(NAND_TPROG_DELAY);                                         /* �ȴ�tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                                              /* ʧ�� */
    }

    return 0;                                                           /* �ɹ� */ 
}

/**
 * @brief       ��ȡspare���е�����
 * @param       pagenum         : Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1)
 * @param       pbuffer         : �������ݻ�����
 * @param       numbyte_to_read : Ҫ��ȡ���ֽ���(������page_sparesize)
 * @retval      0,�ɹ�; ����,�������
 */
uint8_t nand_readspare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read)
{
    uint8_t temp = 0;
    uint8_t remainbyte = 0;
    remainbyte = nand_dev.page_sparesize - colnum;

    if (numbyte_to_read > remainbyte) 
    {
        numbyte_to_read = remainbyte;           /* ȷ��Ҫд����ֽ���������spareʣ��Ĵ�С */
    }

    temp = nand_readpage(pagenum, colnum + nand_dev.page_mainsize, pbuffer, numbyte_to_read);   /* ��ȡ���� */
    return temp;
}

/**
 * @brief       ��spare����д����
 * @param       pagenum         : Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1)
 * @param       pbuffer         : Ҫд��������׵�ַ
 * @param       numbyte_to_write: Ҫд����ֽ���(������page_sparesize)
 * @retval      0,�ɹ�; ����,ʧ��
 */
uint8_t nand_writespare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    uint8_t temp = 0;
    uint8_t remainbyte = 0;

    remainbyte = nand_dev.page_sparesize - colnum;

    if (numbyte_to_write > remainbyte)
    {
        numbyte_to_write = remainbyte;          /* ȷ��Ҫ��ȡ���ֽ���������spareʣ��Ĵ�С */
    }

    temp = nand_writepage(pagenum, colnum + nand_dev.page_mainsize, pbuffer, numbyte_to_write); /* ��ȡ */

    return temp;
}

/**
 * @brief       ����һ����
 * @param       blocknum        : Ҫ������BLOCK���,��Χ:0-(block_totalnum-1)
 * @retval      0,�����ɹ�; ����,����ʧ��
 */
uint8_t nand_eraseblock(uint32_t blocknum)
{
    if (nand_dev.id == MT29F16G08ABABA)
    {
        blocknum <<= 7;             /* �����ַת��Ϊҳ��ַ */
    }
    else if (nand_dev.id == MT29F4G08ABADA)
    {
        blocknum <<= 6;
    }

    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_ERASE0;
    /* ���Ϳ��ַ */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)blocknum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(blocknum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(blocknum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_ERASE1;

    delay_ms(NAND_TBERS_DELAY);     /* �ȴ������ɹ� */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;          /* ʧ�� */
    }

    return 0;                       /* �ɹ� */
}

/**
 * @brief       ȫƬ����NAND FLASH
 * @param       ��
 * @retval      ��
 */
void nand_erasechip(void)
{
    uint8_t status;
    uint16_t i = 0;

    for (i = 0; i < nand_dev.block_totalnum; i++)                     /* ѭ���������еĿ� */
    {
        status = nand_eraseblock(i);

        if (status)
        {
            printf("Erase %d block fail!!��������Ϊ%d\r\n",i,status);  /* ����ʧ�� */
        }
    }
}

/**
 * @brief       ��ȡECC������λ/ż��λ
 * @param       oe      : 0,ż��λ; 1,����λ
 * @param       eccval  : �����eccֵ
 * @retval      ������eccֵ(���16λ)
 */
uint16_t nand_ecc_get_oe(uint8_t oe, uint32_t eccval)
{
    uint8_t i;
    uint16_t ecctemp = 0;

    for (i = 0; i < 24; i++)
    {
        if ((i % 2) == oe)
        {
            if ((eccval >> i) & 0X01)
            {
                ecctemp += 1 << (i >> 1); 
            }
        }
    }

    return ecctemp;
}

/**
 * @brief       ECCУ������
 * @param       data_buf    : ���ݻ�����
 * @param       eccrd       : ��ȡ����, ԭ�������ECCֵ
 * @param       ecccl       : ��ȡ����ʱ, Ӳ�������ECCֵ
 * @retval      0,����������; ����,ECC����(�д���2��bit�Ĵ���,�޷��ָ�)
 */
uint8_t nand_ecc_correction(uint8_t* data_buf, uint32_t eccrd, uint32_t ecccl)
{
    uint16_t eccrdo, eccrde, eccclo, ecccle;
    uint16_t eccchk = 0;
    uint16_t errorpos = 0;
    uint32_t bytepos = 0;
    eccrdo = nand_ecc_get_oe(1, eccrd);     /* ��ȡeccrd������λ */
    eccrde = nand_ecc_get_oe(0, eccrd);     /* ��ȡeccrd��ż��λ */
    eccclo = nand_ecc_get_oe(1, ecccl);     /* ��ȡecccl������λ */
    ecccle = nand_ecc_get_oe(0, ecccl);     /* ��ȡecccl��ż��λ */
    eccchk = eccrdo^eccrde^eccclo^ecccle;

    if (eccchk == 0XFFF)                    /* ȫ1,˵��ֻ��1bit ECC���� */
    {
        errorpos = eccrdo^eccclo; 
        printf("errorpos:%d\r\n", errorpos); 
        bytepos = errorpos / 8; 
        data_buf[bytepos]^=1 << (errorpos % 8);
    }
    else                                    /* ����ȫ1,˵��������2bit ECC����,�޷��޸� */
    {
        printf("2bit ecc error or more\r\n");
        return 1;
    } 

    return 0;
}


