/**
 ****************************************************************************************************
 * @file        nandtester.h
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

#ifndef __NANDTESTER_H
#define __NANDTESTER_H

#include "./Middlewares/SYSTEM/sys/sys.h"

/******************************************************************************************/

uint8_t test_writepage(uint32_t pagenum, uint16_t colnum, uint16_t writebytes);
uint8_t test_readpage(uint32_t pagenum, uint16_t colnum, uint16_t readbytes);
uint8_t test_copypageandwrite(uint32_t spnum, uint32_t dpnum, uint16_t colnum, uint16_t writebytes);
uint8_t test_readspare(uint32_t pagenum, uint16_t colnum, uint16_t readbytes);
void test_readallblockinfo(uint32_t sblock);
uint8_t test_ftlwritesectors(uint32_t secx, uint16_t secsize, uint16_t seccnt);
uint8_t test_ftlreadsectors(uint32_t secx, uint16_t secsize, uint16_t seccnt);

#endif
