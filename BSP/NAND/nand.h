/**
 ****************************************************************************************************
 * @file        nand.h
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


#ifndef _NAND_H
#define _NAND_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ���� ���� */

#define NAND_RB_GPIO_PORT               GPIOD
#define NAND_RB_GPIO_PIN                GPIO_PIN_6
#define NAND_RB_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)      /* PD��ʱ��ʹ�� */

/******************************************************************************************/

/* IO�������� */
#define NAND_RB         HAL_GPIO_ReadPin(NAND_RB_GPIO_PORT, NAND_RB_GPIO_PIN)            /* NAND Flash����/æ���� */

#define NAND_MAX_PAGE_SIZE          4096        /* ����NAND FLASH������PAGE��С��������SPARE������Ĭ��4096�ֽ� */
#define NAND_ECC_SECTOR_SIZE        512         /* ִ��ECC����ĵ�Ԫ��С��Ĭ��512�ֽ� */

/* NAND FLASH���������ʱ���� */
#define NAND_TADL_DELAY             30          /* tADL�ȴ��ӳ�,����70ns */
#define NAND_TWHR_DELAY             25          /* tWHR�ȴ��ӳ�,����60ns */
#define NAND_TRHW_DELAY             35          /* tRHW�ȴ��ӳ�,����100ns */
#define NAND_TPROG_DELAY            200         /* tPROG�ȴ��ӳ�,����ֵ200us,�����Ҫ700us */
#define NAND_TBERS_DELAY            4           /* tBERS�ȴ��ӳ�,����ֵ3.5ms,�����Ҫ10ms */

/* NAND���Խṹ�� */
typedef struct
{
    uint16_t page_totalsize;        /* ÿҳ�ܴ�С��main����spare���ܺ� */
    uint16_t page_mainsize;         /* ÿҳ��main����С */
    uint16_t page_sparesize;        /* ÿҳ��spare����С */
    uint8_t  block_pagenum;         /* ÿ���������ҳ���� */
    uint16_t plane_blocknum;        /* ÿ��plane�����Ŀ����� */
    uint16_t block_totalnum;        /* �ܵĿ����� */
    uint16_t good_blocknum;         /* �ÿ����� */
    uint16_t valid_blocknum;        /* ��Ч������(���ļ�ϵͳʹ�õĺÿ�����) */
    uint32_t id;                    /* NAND FLAS*(vu8*)(0X80000000|(1<<17))=CMD��*(vu8*)(0X80000000|(1<<17))=CMD��H ID */
    uint16_t *lut;                  /* LUT���������߼���-������ת�� */
    uint32_t ecc_hard;              /* Ӳ�����������ECCֵ */
    uint32_t ecc_hdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; /* ECCӲ������ֵ������ */
    uint32_t ecc_rdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; /* ECC��ȡ��ֵ������ */
}nand_attriute;      

extern nand_attriute nand_dev;              /* nand��Ҫ�����ṹ�� */

#define NAND_ADDRESS            0X80000000  /* nand flash�ķ��ʵ�ַ,��NCE3,��ַΪ:0X8000 0000 */
#define NAND_CMD                1 << 16     /* �������� */
#define NAND_ADDR               1 << 17     /* ���͵�ַ */

/* NAND FLASH���� */
#define nand_readID             0X90        /* ��IDָ�� */
#define NAND_FEATURE            0XEF        /* ��������ָ�� */
#define NAND_RESET              0XFF        /* ��λNAND */
#define NAND_READSTA            0X70        /* ��״̬ */
#define NAND_AREA_A             0X00   
#define NAND_AREA_TRUE1         0X30  
#define NAND_WRITE0             0X80
#define NAND_WRITE_TURE1        0X10
#define NAND_ERASE0             0X60
#define NAND_ERASE1             0XD0
#define NAND_MOVEDATA_CMD0      0X00
#define NAND_MOVEDATA_CMD1      0X35
#define NAND_MOVEDATA_CMD2      0X85
#define NAND_MOVEDATA_CMD3      0X10

/* NAND FLASH״̬ */
#define NSTA_READY              0X40        /* nand�Ѿ�׼���� */
#define NSTA_ERROR              0X01        /* nand���� */
#define NSTA_TIMEOUT            0X02        /* ��ʱ */
#define NSTA_ECC1BITERR         0X03        /* ECC 1bit���� */
#define NSTA_ECC2BITERR         0X04        /* ECC 2bit���ϴ��� */

/* NAND FLASH�ͺźͶ�Ӧ��ID�� */
#define MT29F4G08ABADA          0XDC909556  /* MT29F4G08ABADA */
#define MT29F16G08ABABA         0X48002689  /* MT29F16G08ABABA */

/******************************************************************************************/

uint8_t nand_init(void);
uint8_t nand_modeset(uint8_t mode);
uint32_t nand_readid(void);
uint8_t nand_readstatus(void);
uint8_t nand_wait_for_ready(void);
uint8_t nand_reset(void);
uint8_t nand_waitrb(volatile uint8_t rb);
void nand_delay(volatile uint32_t i);
uint8_t nand_readpage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read);
uint8_t nand_readpagecomp(uint32_t pagenum, uint16_t colnum, uint32_t cmpval, uint16_t numbyte_to_read, uint16_t *numbyte_equal);
uint8_t nand_writepage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write);
uint8_t nand_write_pageconst(uint32_t pagenum, uint16_t colnum, uint32_t cval, uint16_t numbyte_to_write);
uint8_t nand_copypage_withoutwrite(uint32_t source_pagenum, uint32_t dest_pagenum);
uint8_t nand_copypage_withwrite(uint32_t source_pagenum, uint32_t dest_pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write);
uint8_t nand_readspare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read);
uint8_t nand_writespare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write);
uint8_t nand_eraseblock(uint32_t blocknum);
void nand_erasechip(void);

uint16_t nand_ecc_get_oe(uint8_t oe, uint32_t eccval);
uint8_t nand_ecc_correction(uint8_t* data_buf, uint32_t eccrd, uint32_t ecccl);

#endif



