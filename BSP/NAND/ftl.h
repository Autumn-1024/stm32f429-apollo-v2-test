/**
 ****************************************************************************************************
 * @file        ftl.h
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

#ifndef __FTL_H
#define __FTL_H

#include "./Middlewares/SYSTEM/sys/sys.h"


/**
 *������������
 * �������Ϊ1,����ftl_format��ʱ��,��Ѱ����,��ʱ��(512M,3��������),�һᵼ��RGB������
 */
#define FTL_USE_BAD_BLOCK_SEARCH        0       /* �����Ƿ�ʹ�û������� */

/******************************************************************************************/

uint8_t ftl_init(void); 
void ftl_badblock_mark(uint32_t blocknum);
uint8_t ftl_check_badblock(uint32_t blocknum); 
uint8_t ftl_used_blockmark(uint32_t blocknum);
uint32_t ftl_find_unused_block(uint32_t sblock, uint8_t flag);
uint32_t ftl_find_same_plane_unused_block(uint32_t sblock);
uint8_t ftl_copy_and_write_to_block(uint32_t source_pagenum, uint16_t colnum, uint8_t *pbuffer, uint32_t numbyte_to_write);
uint16_t ftl_lbn_to_pbn(uint32_t lbnnum); 
uint8_t ftl_write_sectors(uint8_t *pbuffer, uint32_t sectorno, uint16_t sectorsize, uint32_t sectorcount);
uint8_t ftl_read_sectors(uint8_t *pbuffer, uint32_t sectorno, uint16_t sectorsize, uint32_t sectorcount);
uint8_t ftl_create_lut(uint8_t mode);
uint8_t ftl_blockcompare(uint32_t blockx, uint32_t cmpval);
uint32_t ftl_search_badblock(void);
uint8_t ftl_format(void);

#endif

