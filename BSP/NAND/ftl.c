/**
 ****************************************************************************************************
 * @file        ftl.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       NAND FLASH FTL���㷨����
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
#include "./MALLOC/malloc.h"
#include "./BSP/NAND/nand.h"
#include "./Middlewares/SYSTEM/usart/usart.h"


/**
 *        ÿ����,��һ��page��spare��,ǰ�ĸ��ֽڵĺ���:
 *        ��һ���ֽ�,��ʾ�ÿ��Ƿ��ǻ���:0XFF,������;����ֵ,����.
 *        �ڶ����ֽ�,��ʾ�ÿ��Ƿ��ù�:0XFF,û��д������;0XCC,д��������.
 *        �����͵��ĸ��ֽ�,��ʾ�ÿ��������߼�����. 

 *        ÿ��page,spare��16�ֽ��Ժ���ֽں���:
 *        ��ʮ���ֽڿ�ʼ,����ÿ4���ֽ����ڴ洢һ������(��С:NAND_ECC_SECTOR_SIZE)��ECCֵ,����ECCУ��
 */

/**
 * @brief       FTL���ʼ��
 * @param       ��
 * @retval      0,����������,ʧ��
 */
uint8_t ftl_init(void)
{
    uint8_t temp;

    if (nand_init())
    {
        return 1;                                                   /* ��ʼ��NAND FLASH */
    }

    if (nand_dev.lut)
    {
        myfree(SRAMIN, nand_dev.lut);
    }

    nand_dev.lut = mymalloc(SRAMIN, (nand_dev.block_totalnum) * 2); /* ��LUT�������ڴ� */
    memset(nand_dev.lut, 0, nand_dev.block_totalnum * 2);           /* ȫ������ */

    if (!nand_dev.lut)
    {
        return 1;                                                   /* �ڴ�����ʧ��  */
    }

    temp = ftl_create_lut(1);

    if (temp) 
    {
        printf("format nand flash...\r\n");
        temp = ftl_format();                                        /* ��ʽ��NAND */

        if (temp)
        {
            printf("format failed!\r\n");
            return 2;
        }
    }
    else                                                            /* ����LUT���ɹ� */
    {
        printf("total block num: %d\r\n", nand_dev.block_totalnum);
        printf("good block num: %d\r\n", nand_dev.good_blocknum);
        printf("valid block num: %d\r\n", nand_dev.valid_blocknum);
    }

    return 0;
} 

/**
 * @brief       ���ĳһ����Ϊ����
 * @param       blocknum   : ����,��Χ:0~(block_totalnum-1)
 * @retval      ��
 */
void ftl_badblock_mark(uint32_t blocknum)
{
    uint32_t temp = 0XAAAAAAAA; /* ������mark,����ֵ��OK,ֻҪ����0XFF.����дǰ4���ֽ�,����ftl_find_unused_block������黵��.(����鱸����,������ٶ�) */
    nand_writespare(blocknum * nand_dev.block_pagenum, 0, (uint8_t *)&temp, 4);      /* �ڵ�һ��page��spare��,��һ���ֽ���������(ǰ4���ֽڶ�д) */
    nand_writespare(blocknum * nand_dev.block_pagenum + 1, 0, (uint8_t *)&temp, 4);  /* �ڵڶ���page��spare��,��һ���ֽ���������(������,ǰ4���ֽڶ�д) */
}

/**
 * @brief       ���ĳһ���Ƿ��ǻ���
 * @param       blocknum    : ����,��Χ:0~(block_totalnum-1)
 * @retval      0,�ÿ�;
 *              ����,����
 */
uint8_t ftl_check_badblock(uint32_t blocknum)
{
    uint8_t flag = 0;

    nand_readspare(blocknum * nand_dev.block_pagenum, 0, &flag, 1);         /* ��ȡ�����־ */

    if (flag == 0XFF)                                                       /* �ÿ�?,��ȡ������������ */
    {
        nand_readspare(blocknum * nand_dev.block_pagenum + 1, 0, &flag, 1); /* ��ȡ�����������־ */

        if (flag == 0XFF)
        {
            return 0;                                                       /* �ÿ� */
        }

        else return 1;                                                      /* ���� */
    }

    return 2; 
}

/**
 * @brief       ���ĳһ�����Ѿ�ʹ��
 * @param       blocknum    : ����,��Χ:0~(block_totalnum-1)
 * @retval      0,�ɹ�;
 *              ����,ʧ��
 */
uint8_t ftl_used_blockmark(uint32_t blocknum)
{
    uint8_t usedflag = 0XCC;
    uint8_t temp = 0;
    temp = nand_writespare(blocknum * nand_dev.block_pagenum, 1, (uint8_t *)&usedflag, 1); /* д����Ѿ���ʹ�ñ�־ */

    return temp;
}

/**
 * @brief       �Ӹ����Ŀ鿪ʼ�ҵ���ǰ�ҵ�һ��δ��ʹ�õĿ�(ָ������/ż��)
 * @param       sblock      : ��ʼ��,��Χ:0~(block_totalnum-1)
 * @param       flag        : 0,ż����; 1,������.
 * @retval      0XFFFFFFFF,ʧ��;
 *              ����ֵ,δʹ�ÿ��
 */
uint32_t ftl_find_unused_block(uint32_t sblock, uint8_t flag)
{
    uint32_t temp = 0;
    uint32_t blocknum = 0;

    for (blocknum = sblock + 1; blocknum > 0; blocknum--)
    {
        if (((blocknum - 1) % 2) == flag)   /* ��ż�ϸ�,�ż�� */
        {
            nand_readspare((blocknum - 1) * nand_dev.block_pagenum, 0, (uint8_t *)&temp, 4); /* �����Ƿ�ʹ�ñ�� */
            if (temp == 0XFFFFFFFF)
            {
                return (blocknum - 1);      /* �ҵ�һ���տ�,���ؿ��� */
            }
        }
    }

    return 0XFFFFFFFF;                      /* δ�ҵ������ */
}

/**
 * @brief       �������������ͬһ��plane�ڵ�δʹ�õĿ�
 * @param       sblock    �� ������,��Χ:0~(block_totalnum-1)
 * @retval      0XFFFFFFFF,ʧ��;����ֵ,δʹ�ÿ��
 */
uint32_t ftl_find_same_plane_unused_block(uint32_t sblock)
{
    static uint32_t curblock = 0XFFFFFFFF;
    uint32_t unusedblock = 0;

    if (curblock > (nand_dev.block_totalnum - 1))
    {
        curblock = nand_dev.block_totalnum - 1;                   /* ������Χ��,ǿ�ƴ����һ���鿪ʼ */
    }

    unusedblock = ftl_find_unused_block(curblock,sblock % 2);     /* �ӵ�ǰ��,��ʼ,��ǰ���ҿ����  */

    if (unusedblock == 0XFFFFFFFF && curblock < (nand_dev.block_totalnum - 1))  /* δ�ҵ�,�Ҳ��Ǵ���ĩβ��ʼ�ҵ� */
    {
        curblock = nand_dev.block_totalnum - 1;                   /* ǿ�ƴ����һ���鿪ʼ */
        unusedblock = ftl_find_unused_block(curblock,sblock % 2); /* ����ĩβ��ʼ,������һ�� */ 
    }
    if (unusedblock == 0XFFFFFFFF)
    {
        return 0XFFFFFFFF;                                        /* �Ҳ�������block */
    }

    curblock = unusedblock;                                       /* ��ǰ��ŵ���δʹ�ÿ���.�´���Ӵ˴���ʼ���� */

    return unusedblock;                                           /* �����ҵ��Ŀ���block */
}

/**
 * @brief       ��һ��������ݿ�������һ��,���ҿ���д������
 * @param       source_pagenum  : Ҫд�����ݵ�ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
 * @param       colnum          : Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
 * @param       pbuffer         : Ҫд�������
 * @param       numbyte_to_write: Ҫд����ֽ�������ֵ���ܳ�������ʣ��������С
 * @retval      0,�ɹ�;
 *              ����,ʧ��
 */
uint8_t ftl_copy_and_write_to_block(uint32_t source_pagenum, uint16_t colnum, uint8_t *pbuffer, uint32_t numbyte_to_write)
{
    uint16_t i = 0, temp = 0, wrlen;
    uint32_t source_block = 0, pageoffset = 0;
    uint32_t unusedblock = 0; 
    source_block = source_pagenum / nand_dev.block_pagenum;         /* ���ҳ���ڵĿ�� */
    pageoffset = source_pagenum % nand_dev.block_pagenum;           /* ���ҳ�����ڿ��ڵ�ƫ�� */

retry:
    unusedblock = ftl_find_same_plane_unused_block(source_block);   /* ������Դ����һ��plane��δʹ�ÿ� */

    if (unusedblock > nand_dev.block_totalnum)
    {
        return 1;                                                   /* ���ҵ��Ŀ����Ŵ��ڿ��������Ļ��϶��ǳ����� */
    }

    for (i = 0; i < nand_dev.block_pagenum; i++)                    /* ��һ��������ݸ��Ƶ��ҵ���δʹ�ÿ��� */
    {
        if (i >= pageoffset && numbyte_to_write)                    /* ����Ҫд�뵽��ǰҳ */
        { 
            if (numbyte_to_write > (nand_dev.page_mainsize - colnum)) /* Ҫд�������,�����˵�ǰҳ��ʣ������ */
            {
                wrlen = nand_dev.page_mainsize - colnum;            /* д�볤�ȵ��ڵ�ǰҳʣ�����ݳ��� */
            }
            else 
            {
                wrlen = numbyte_to_write;                           /* д��ȫ������ */
            }

            temp = nand_copypage_withwrite(source_block * nand_dev.block_pagenum + i, unusedblock * nand_dev.block_pagenum + i, colnum, pbuffer, wrlen);
            colnum = 0;                                             /* �е�ַ���� */
            pbuffer += wrlen;                                       /* д��ַƫ�� */
            numbyte_to_write -= wrlen;                              /* д�����ݼ��� */
        }

        else                                                        /* ������д��,ֱ�ӿ������� */
        {
            temp = nand_copypage_withoutwrite(source_block * nand_dev.block_pagenum + i, unusedblock * nand_dev.block_pagenum + i);
        }

        if (temp)                                                   /* ����ֵ����,�����鴦�� */
        { 
            ftl_badblock_mark(unusedblock);                         /* ���Ϊ���� */
            ftl_create_lut(1);                                      /* �ؽ�LUT�� */
            goto retry;
        }
    }

    if (i == nand_dev.block_pagenum)                                /* ������� */
    {
        ftl_used_blockmark(unusedblock);                            /* ��ǿ��Ѿ�ʹ�� */
        nand_eraseblock(source_block);                              /* ����Դ�� */
        for (i = 0; i < nand_dev.block_totalnum; i++)               /* ����LUT������unusedblock�滻source_block */
        {
            if (nand_dev.lut[i] == source_block)
            {
                nand_dev.lut[i] = unusedblock;
                break;
            }
        }
    }

    return 0;   /* �ɹ� */
}

/**
 * @brief       �߼����ת��Ϊ������� 
 * @param       lbnnum    : �߼�����
 * @retval      ��������
 */
uint16_t ftl_lbn_to_pbn(uint32_t lbnnum)
{
    uint16_t pbnno = 0;

    /* ���߼���Ŵ�����Ч������ʱ�򷵻�0XFFFF */
    if (lbnnum > nand_dev.valid_blocknum)
    {
        return 0XFFFF;
    }

    pbnno = nand_dev.lut[lbnnum];

    return pbnno;
}

/**
 * @brief       д����(֧�ֶ�����д)��FATFS�ļ�ϵͳʹ��
 * @param       pbuffer     : Ҫд�������
 * @param       sectorno    : ��ʼ������
 * @param       sectorsize  : ������С(���ܴ���NAND_ECC_SECTOR_SIZE����Ĵ�С,��������!!)
 * @param       sectorcount : Ҫд�����������
 * @retval      0,�ɹ�;
 *              ����,ʧ��
 */
uint8_t ftl_write_sectors(uint8_t *pbuffer, uint32_t sectorno, uint16_t sectorsize, uint32_t sectorcount)
{
    uint8_t flag = 0;
    uint16_t temp;
    uint32_t i = 0;
    uint16_t wsecs;                 /* дҳ��С */
    uint32_t wlen;                  /* д�볤�� */
    uint32_t lbnno;                 /* �߼���� */
    uint32_t pbnno;                 /* ������� */
    uint32_t phypageno;             /* ����ҳ�� */
    uint32_t pageoffset;            /* ҳ��ƫ�Ƶ�ַ */
    uint32_t blockoffset;           /* ����ƫ�Ƶ�ַ */
    uint32_t markdpbn = 0XFFFFFFFF; /* ����˵��������� */

    for (i = 0; i < sectorcount; i++)
    {
        lbnno = (sectorno+i) / (nand_dev.block_pagenum * (nand_dev.page_mainsize / sectorsize));/* �����߼������ź�������С������߼���� */
        pbnno = ftl_lbn_to_pbn(lbnno);                                                          /* ���߼���ת��Ϊ������ */

        if (pbnno >= nand_dev.block_totalnum)
        {
            return 1;                                                                           /* ������Ŵ���NAND FLASH���ܿ���,��ʧ��. */
        }

        blockoffset =((sectorno + i) % (nand_dev.block_pagenum * (nand_dev.page_mainsize / sectorsize))) * sectorsize; /* �������ƫ�� */
        phypageno = pbnno * nand_dev.block_pagenum + blockoffset / nand_dev.page_mainsize;                             /* ���������ҳ�� */
        pageoffset = blockoffset % nand_dev.page_mainsize;                                                             /* �����ҳ��ƫ�Ƶ�ַ  */
        temp = nand_dev.page_mainsize - pageoffset;                                                                    /* page��ʣ���ֽ��� */
        temp /= sectorsize;                                                                                            /* ��������д���sector��  */
        wsecs = sectorcount - i;                                                                                       /* ��ʣ���ٸ�sectorҪд */

        if (wsecs >= temp)
        {
            wsecs = temp;                                                               /* ���ڿ�����д���sector��,��д��temp������ */
        }

        wlen = wsecs * sectorsize;                                                      /* ÿ��дwsecs��sector */

        /* ����д���С�������ж��Ƿ�ȫΪ0XFF */
        flag = nand_readpagecomp(phypageno, pageoffset, 0XFFFFFFFF, wlen / 4, &temp);   /* ��һ��wlen/4��С������,����0XFFFFFFFF�Ա� */

        if (flag)
        {
            return 2;                                                   /* ��д���󣬻��� */
        }
        if (temp == (wlen / 4)) 
        {
            flag = nand_writepage(phypageno, pageoffset, pbuffer, wlen);/* ȫΪ0XFF,����ֱ��д���� */
        }
        else
        {
            flag = 1;                                                   /* ��ȫ��0XFF,���������� */
        }
        if (flag == 0 && (markdpbn != pbnno))                           /* ȫ��0XFF,��д��ɹ�,�ұ���˵��������뵱ǰ�����鲻ͬ */
        {
            flag = ftl_used_blockmark(pbnno);                           /* ��Ǵ˿��Ѿ�ʹ�� */
            markdpbn = pbnno;                                           /* ������,��ǿ�=��ǰ��,��ֹ�ظ���� */
        }
        if (flag)                                                       /* ��ȫΪ0XFF/���ʧ�ܣ�������д����һ���� */
        {
            temp = ((uint32_t)nand_dev.block_pagenum * nand_dev.page_mainsize - blockoffset) / sectorsize;/* ��������block��ʣ�¶��ٸ�SECTOR����д�� */
            wsecs = sectorcount - i;                                    /* ��ʣ���ٸ�sectorҪд */

            if (wsecs >= temp)
            {
                wsecs = temp;                                           /* ���ڿ�����д���sector��,��д��temp������ */
            }

            wlen = wsecs * sectorsize;                                  /* ÿ��дwsecs��sector */
            flag = ftl_copy_and_write_to_block(phypageno, pageoffset, pbuffer, wlen);  /* ����������һ��block,��д������ */
            if (flag)
            {
                return 3;                                               /* ʧ�� */
            }
        }

        i += wsecs - 1;
        pbuffer += wlen;                                                /* ���ݻ�����ָ��ƫ�� */
    }
    return 0;
} 

/**
 * @brief       ������(֧�ֶ�������)��FATFS�ļ�ϵͳʹ��
 * @param       pbuffer     : ���ݻ�����
 * @param       sectorno    : ��ʼ������
 * @param       sectorsize  : ������С
 * @param       sectorcount : Ҫд�����������
 * @retval      0,�ɹ�;
 *              ����,ʧ��
 */
uint8_t ftl_read_sectors(uint8_t *pbuffer, uint32_t sectorno, uint16_t sectorsize, uint32_t sectorcount)
{
    uint8_t flag = 0;
    uint16_t rsecs;         /* ���ζ�ȡҳ�� */
    uint32_t i = 0;
    uint32_t lbnno;         /* �߼���� */
    uint32_t pbnno;         /* ������� */
    uint32_t phypageno;     /* ����ҳ�� */
    uint32_t pageoffset;    /* ҳ��ƫ�Ƶ�ַ */
    uint32_t blockoffset;   /* ����ƫ�Ƶ�ַ */

    for (i = 0; i < sectorcount; i++)
    {
        lbnno = (sectorno + i) / (nand_dev.block_pagenum * (nand_dev.page_mainsize / sectorsize));  /* �����߼������ź�������С������߼���� */
        pbnno = ftl_lbn_to_pbn(lbnno);                                                              /* ���߼���ת��Ϊ������ */

        if (pbnno >= nand_dev.block_totalnum)
        {
            return 1;                                                                               /* ������Ŵ���NAND FLASH���ܿ���,��ʧ��. */
        }

        blockoffset = ((sectorno + i) % (nand_dev.block_pagenum * (nand_dev.page_mainsize / sectorsize))) * sectorsize; /* �������ƫ�� */
        phypageno = pbnno * nand_dev.block_pagenum + blockoffset / nand_dev.page_mainsize;          /* ���������ҳ�� */
        pageoffset = blockoffset%nand_dev.page_mainsize;                                            /* �����ҳ��ƫ�Ƶ�ַ */
        rsecs = (nand_dev.page_mainsize - pageoffset) / sectorsize;                                 /* ����һ�������Զ�ȡ����ҳ */

        if (rsecs > (sectorcount - i))
        {
            rsecs = sectorcount - i;                                                                /* ��಻�ܳ���SectorCount-i */
        }
        flag = nand_readpage(phypageno, pageoffset, pbuffer, rsecs * sectorsize);                   /* ��ȡ���� */

        if (flag == NSTA_ECC1BITERR)                                                                /* ����1bit ecc����,����Ϊ���� */
        {
            flag = nand_readpage(phypageno, pageoffset, pbuffer, rsecs * sectorsize);               /* �ض�����,�ٴ�ȷ�� */

            if (flag == NSTA_ECC1BITERR)
            {
                ftl_copy_and_write_to_block(phypageno, pageoffset, pbuffer, rsecs * sectorsize);    /* �������� */
                flag = ftl_blockcompare(phypageno / nand_dev.block_pagenum, 0XFFFFFFFF);            /* ȫ1���,ȷ���Ƿ�Ϊ���� */

                if (flag == 0)
                {
                    flag = ftl_blockcompare(phypageno / nand_dev.block_pagenum, 0X00);              /* ȫ0���,ȷ���Ƿ�Ϊ���� */
                    nand_eraseblock(phypageno / nand_dev.block_pagenum);                            /* �����ɺ�,��������� */
                }

                if (flag)                                                                           /* ȫ0/ȫ1������,�϶��ǻ�����. */
                {
                    ftl_badblock_mark(phypageno/nand_dev.block_pagenum);                            /* ���Ϊ���� */
                    ftl_create_lut(1);                                                              /* �ؽ�LUT�� */
                }

                flag = 0;
            }
        }

        if (flag == NSTA_ECC2BITERR)
        {
            flag = 0;                       /* 2bit ecc����,������(�����ǳ���д�����ݵ��µ�) */
        }
        if (flag)
        {
            return 2;                       /* ʧ�� */
        }

        pbuffer += sectorsize * rsecs;      /* ���ݻ�����ָ��ƫ�� */
        i += rsecs - 1;
    }
    return 0; 
}

/**
 * @brief       ���´���LUT��
 * @param       mode    : 0,������һ��������
 *              1,���������Ƕ�Ҫ���(������ҲҪ���)
 * @retval      0,�ɹ�;����,ʧ��
 */
uint8_t ftl_create_lut(uint8_t mode)
{
    uint32_t i;
    uint8_t buf[4];
    uint32_t lbnnum = 0;                                             /* �߼���� */

    for (i = 0; i < nand_dev.block_totalnum; i++)                    /* ��λLUT������ʼ��Ϊ��Чֵ��Ҳ����0XFFFF */
    {
        nand_dev.lut[i] = 0XFFFF;
    }

    nand_dev.good_blocknum = 0;
    for (i = 0; i < nand_dev.block_totalnum; i++)
    {
        nand_readspare(i * nand_dev.block_pagenum, 0, buf, 4);         /* ��ȡ4���ֽ� */

        if (buf[0] == 0XFF && mode)
        {
            nand_readspare(i * nand_dev.block_pagenum + 1, 0, buf, 1); /* �ÿ�,����Ҫ���2�λ����� */
        }

        if (buf[0] == 0XFF)                                            /* �Ǻÿ� */
        { 
            lbnnum = ((uint16_t)buf[3] << 8) + buf[2];                 /* �õ��߼����� */
            if (lbnnum < nand_dev.block_totalnum)                      /* �߼���ſ϶�С���ܵĿ����� */
            {
                nand_dev.lut[lbnnum] = i;                              /* ����LUT����дLBNnum��Ӧ���������� */
            }
            nand_dev.good_blocknum++;
        }
        else
        {
            printf("bad block index:%d\r\n", i);
        }
    } 
    /* LUT����������Ժ�����Ч����� */
    for (i = 0; i < nand_dev.block_totalnum; i++)
    {
        if (nand_dev.lut[i] >= nand_dev.block_totalnum)
        {
            nand_dev.valid_blocknum = i;
            break;
        }
    }

    if (nand_dev.valid_blocknum < 100)
    {
        return 2;               /* ��Ч����С��100,������.��Ҫ���¸�ʽ�� */
    }

    return 0;                   /* LUT��������� */
}

/**
 * @brief       FTL����Block��ĳ�����ݶԱ�
 * @param       blockx      : block���
 * @param       cmpval      : Ҫ��֮�Աȵ�ֵ
 * @retval      0,���ɹ�,ȫ�����;
                1,���ʧ��,�в���ȵ����
 */
uint8_t ftl_blockcompare(uint32_t blockx, uint32_t cmpval)
{
    uint8_t res;
    uint16_t i, j, k;

    for (i = 0; i < 3; i++)                             /* ����3�λ��� */
    {
        for (j = 0; j < nand_dev.block_pagenum; j++)
        {
            /* ���һ��page,����0XFFFFFFFF�Ա� */
            nand_readpagecomp(blockx * nand_dev.block_pagenum, 0, cmpval, nand_dev.page_mainsize / 4, &k); 

            if (k != (nand_dev.page_mainsize / 4))break;
        }

        if (j == nand_dev.block_pagenum)
        {
            return 0;                                   /* ���ϸ�,ֱ���˳� */
        }

        res = nand_eraseblock(blockx);

        if (res)
        {
            printf("error erase block:%d\r\n", i);
        }
        else
        { 
            if (cmpval != 0XFFFFFFFF)                   /* �����ж�ȫ1,����Ҫ��д���� */
            {
                for (k = 0; k < nand_dev.block_pagenum; k++)
                {
                    nand_write_pageconst(blockx * nand_dev.block_pagenum + k, 0, 0, nand_dev.page_mainsize / 4); /* дPAGE */
                }
            }
        }
    }

    printf("bad block checked:%d\r\n", blockx);
    return 1;
}

/**
 * @brief       FTL��ʼ��ʱ����Ѱ���л���,ʹ��:��-д-�� ��ʽ
 * @note        512M��NAND ,��ҪԼ3����ʱ��,����ɼ��
 *              ����RGB��,����Ƶ����дNAND,��������Ļ����
 * @param       ��
 * @retval      �ÿ������
 */
uint32_t ftl_search_badblock(void)
{
    uint8_t *blktbl;
    uint8_t res;
    uint32_t i, j; 
    uint32_t goodblock = 0;

    blktbl = mymalloc(SRAMIN, nand_dev.block_totalnum);  /* ����block������ڴ�,��Ӧ��:0,�ÿ�;1,����; */
    nand_erasechip();                                    /* ȫƬ���� */

    for (i = 0; i < nand_dev.block_totalnum; i++)        /* ��һ�׶μ��,���ȫ1 */
    {
        res = ftl_blockcompare(i, 0XFFFFFFFF);           /* ȫ1��� */

        if (res)
        {
            blktbl[i] = 1;                               /* ����  */
        }
        else
        { 
            blktbl[i] = 0;/* �ÿ� */

            for (j = 0; j < nand_dev.block_pagenum; j++) /* дblockΪȫ0,Ϊ����ļ��׼�� */
            {
                nand_write_pageconst(i * nand_dev.block_pagenum + j, 0, 0, nand_dev.page_mainsize / 4);
            } 
        }
    }

    for (i = 0; i < nand_dev.block_totalnum; i++)        /* �ڶ��׶μ��,���ȫ0 */
    { 
        if (blktbl[i] == 0)                              /* �ڵ�һ�׶�,û�б���ǻ����,�ſ����Ǻÿ� */
        {
            res = ftl_blockcompare(i, 0);                /* ȫ0��� */

            if (res)
            {
                blktbl[i] = 1;                           /* ��ǻ��� */
            }
            else
            {
                goodblock++; 
            }
        }
    }

    nand_erasechip();                                    /* ȫƬ���� */

    for (i = 0; i < nand_dev.block_totalnum; i++)        /* �����׶μ��,��ǻ��� */
    { 
        if (blktbl[i])
        {
            ftl_badblock_mark(i);                        /* �ǻ��� */
        }
    }

    return goodblock;                                    /* ���غÿ������ */
}

/**
 * @brief       ��ʽ��NAND �ؽ�LUT��
 * @param       ��
 * @retval      0,�ɹ�;
                ����,ʧ��
 */
uint8_t ftl_format(void)
{
    uint8_t temp;
    uint32_t i, n;
    uint32_t goodblock = 0;
    nand_dev.good_blocknum = 0;

#if FTL_USE_BAD_BLOCK_SEARCH == 1                    /* ʹ�ò�-д-���ķ�ʽ,��⻵�� */
    nand_dev.good_blocknum = FTL_SearchBadBlock();   /* ��Ѱ����.��ʱ�ܾ� */
#else                                                /* ֱ��ʹ��NAND FLASH�ĳ��������־(������,Ĭ���Ǻÿ�) */

    for (i = 0; i < nand_dev.block_totalnum; i++)
    {
        temp = ftl_check_badblock(i);                /* ���һ�����Ƿ�Ϊ���� */

        if (temp == 0)                               /* �ÿ� */
        {
            temp = nand_eraseblock(i);

            if (temp)                                /* ����ʧ��,��Ϊ���� */
            {
                printf("Bad block:%d\r\n", i);
                ftl_badblock_mark(i);                /* ����ǻ��� */
            }
            else
            {
                nand_dev.good_blocknum++;            /* �ÿ�������һ */
            }
        }
    }
#endif
    printf("good_blocknum:%d\r\n", nand_dev.good_blocknum);

    if (nand_dev.good_blocknum < 100)
    {
        return 1;                                    /*����ÿ����������100����NAND Flash���� */
    }

    goodblock = (nand_dev.good_blocknum * 93) / 100; /* %93�ĺÿ����ڴ洢���� */

    n = 0;

    for (i = 0; i < nand_dev.block_totalnum; i++)    /* �ںÿ��б�����߼�����Ϣ */
    {
        temp = ftl_check_badblock(i);                /* ���һ�����Ƿ�Ϊ���� */

        if (temp == 0)                               /* �ÿ� */
        { 
            nand_writespare(i * nand_dev.block_pagenum, 2, (uint8_t*)&n, 2);  /* д���߼����� */
            n++;                                     /* �߼����ż�1 */

            if (n == goodblock) break;               /* ȫ��������� */
        }
    } 
    if (ftl_create_lut(1))
    {
        return 2;                                    /* �ؽ�LUT��ʧ�� */
    }

    return 0;
}




