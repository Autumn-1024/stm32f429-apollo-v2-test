/**
 ****************************************************************************************************
 * @file        nandtester.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       NAND FLASH USMART���Դ���
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

#include "string.h"
#include "./BSP/NAND/ftl.h"
#include "./BSP/NAND/nand.h"
#include "./MALLOC/malloc.h"
#include "./Middlewares/SYSTEM/usart/usart.h"
#include "./BSP/NAND/nandtester.h"


/**
 * @brief       ��NANDĳһҳд��ָ����С������
 * @param       pagenum     : Ҫд���ҳ��ַ
 * @param       colnum      : Ҫд��Ŀ�ʼ�е�ַ(ҳ�ڵ�ַ)
 * @param       writebytes  : Ҫд������ݴ�С��MT29F16G���Ϊ4320��MT29F4G���Ϊ2112
 * @retval      �������
 */
uint8_t test_writepage(uint32_t pagenum, uint16_t colnum, uint16_t writebytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i = 0;
    pbuf = mymalloc(SRAMIN, 5000);

    for (i = 0; i < writebytes; i++)                          /* ׼��Ҫд�������,�������,��0��ʼ���� */
    { 
        pbuf[i] = i;
    }

    sta = nand_writepage(pagenum, colnum, pbuf, writebytes);  /* ��nandд������ */
    myfree(SRAMIN, pbuf);                                     /* �ͷ��ڴ� */

    return sta;
}

/**
 * @brief       ��ȡNANDĳһҳָ����С������
 * @param       pagenum     : Ҫ��ȡ��ҳ��ַ
 * @param       colnum      : Ҫ��ȡ�Ŀ�ʼ�е�ַ(ҳ�ڵ�ַ)
 * @param       readbytes   : Ҫ��ȡ�����ݴ�С��MT29F16G���Ϊ4320��MT29F4G���Ϊ2112
 * @retval      �������
 */
uint8_t test_readpage(uint32_t pagenum, uint16_t colnum, uint16_t readbytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i = 0;

    pbuf = mymalloc(SRAMIN, 5000);
    sta = nand_readpage(pagenum, colnum, pbuf, readbytes);             /* ��ȡ���� */

    if (sta == 0 || sta == NSTA_ECC1BITERR || sta == NSTA_ECC2BITERR)  /* ��ȡ�ɹ� */
    {
        printf("read page data is:\r\n");

        for (i = 0; i < readbytes; i++)
        {
            printf("%x ", pbuf[i]);                                    /* ���ڴ�ӡ��ȡ�������� */
        }
        printf("\r\nend\r\n");
    }

    myfree(SRAMIN, pbuf);                                              /* �ͷ��ڴ� */

    return sta;
}

/**
 * @brief       ��һҳ���ݿ���������һҳ,��д��һ��������.
 *  @note       Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�(ͬΪ����/ͬΪż��)
 * @param       spnum       : Դҳ��ַ
 * @param       epnum       : Ŀ��ҳ��ַ
 * @param       colnum      : Ҫд��Ŀ�ʼ�е�ַ(ҳ�ڵ�ַ)
 * @param       writebytes  : Ҫд������ݴ�С�����ܳ���ҳ��С
 * @retval      �������
 */
uint8_t test_copypageandwrite(uint32_t spnum, uint32_t dpnum, uint16_t colnum, uint16_t writebytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i = 0;
    pbuf = mymalloc(SRAMIN, 5000);

    for (i = 0; i < writebytes; i++)                                        /* ׼��Ҫд�������,�������,��0X80��ʼ���� */
    { 
        pbuf[i] = i + 0X80;
    }

    sta = nand_copypage_withwrite(spnum, dpnum, colnum, pbuf, writebytes);  /* ��nandд������ */
    myfree(SRAMIN, pbuf);                                                   /* �ͷ��ڴ� */
    return sta;
}
 
/**
 * @brief       ��ȡNANDĳһҳSpare��ָ����С������
 * @param       pagenum     : Ҫ��ȡ��ҳ��ַ
 * @param       colnum      : Ҫ��ȡ��spare����ʼ��ַ
 * @param       readbytes   : Ҫ��ȡ�����ݴ�С��MT29F16G���Ϊ64��MT29F4G���Ϊ224
 * @retval      �������
 */
uint8_t test_readspare(uint32_t pagenum, uint16_t colnum, uint16_t readbytes)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint16_t i = 0;
    pbuf = mymalloc(SRAMIN, 512);
    sta = nand_readspare(pagenum, colnum, pbuf, readbytes);  /* ��ȡ���� */

    if (sta == 0)                                            /* ��ȡ�ɹ� */
    { 
        printf("read spare data is:\r\n");

        for (i = 0; i < readbytes; i++)
        { 
            printf("%x ", pbuf[i]);                         /* ���ڴ�ӡ��ȡ�������� */
        }

        printf("\r\nend\r\n");
    }

    myfree(SRAMIN, pbuf);                                   /* �ͷ��ڴ� */

    return sta;
}

/**
 * @brief       ��ָ��λ�ÿ�ʼ,��ȡ����NAND,ÿ��BLOCK�ĵ�һ��page��ǰ5���ֽ�
 * @param       sblock      : ָ����ʼ��block���
 * @retval      ��
 */
void test_readallblockinfo(uint32_t sblock)
{
    uint8_t j = 0;
    uint32_t i = 0;
    uint8_t sta;
    uint8_t buffer[5];

    for (i = sblock; i < nand_dev.block_totalnum; i++)
    {
        printf("block %d info:", i);
        sta = nand_readspare(i * nand_dev.block_pagenum, 0, buffer, 5);  /* ��ȡÿ��block,��һ��page��ǰ5���ֽ� */

        if (sta)
        {
            printf("failed\r\n");
        }
        for (j = 0; j < 5; j++)
        {
            printf("%x ", buffer[j]);
        }
        printf("\r\n");
    }
}

/******************************************************************************************/
/* FTL����Դ��� */

/**
 * @brief       ��ĳ��������ʼ,д��seccnt������������
 * @param       secx        : ��ʼ���������
 * @param       secsize     : ������С
 * @param       seccnt      : Ҫд�����������
 * @retval      �������
 */
uint8_t test_ftlwritesectors(uint32_t secx, uint16_t secsize, uint16_t seccnt)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint32_t i = 0;
    pbuf = mymalloc(SRAMIN, secsize * seccnt);

    for (i = 0; i < secsize * seccnt; i++)                 /* ׼��Ҫд�������,�������,��0��ʼ���� */
    { 
        pbuf[i] = i;
    }

    sta = ftl_write_sectors(pbuf, secx, secsize, seccnt);  /* ��nandд������ */

    myfree(SRAMIN, pbuf);                                  /* �ͷ��ڴ� */

    return sta;
}

/**
 * @brief       ��ĳ��������ʼ,����seccnt������������
 * @param       secx        : ��ʼ���������
 * @param       secsize     : ������С
 * @param       seccnt      : Ҫ��ȡ����������
 * @retval      �������
 */
uint8_t test_ftlreadsectors(uint32_t secx, uint16_t secsize, uint16_t seccnt)
{
    uint8_t *pbuf;
    uint8_t sta = 0;
    uint32_t i = 0;

    pbuf = mymalloc(SRAMIN, secsize * seccnt);   
    sta = ftl_read_sectors(pbuf, secx, secsize, seccnt);  /* ��ȡ���� */

    if (sta == 0)
    {
        printf("read sec %d data is:\r\n", secx); 

        for (i = 0; i < secsize * seccnt; i++)           /* ׼��Ҫд�������,�������,��0��ʼ���� */
        { 
            printf("%x ",pbuf[i]);                       /* ���ڴ�ӡ��ȡ�������� */
        }

        printf("\r\nend\r\n");
    }

    myfree(SRAMIN, pbuf);                                /* �ͷ��ڴ� */

    return sta;
}

