#ifndef __USBD_CONF_H
#define __USBD_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./MALLOC/malloc.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "stm32f4xx_hal_pcd.h"

#define HAL_Delay delay_ms                          /* 定义HAL_Delay使用delay_ms实现 */
#define HAL_PCD_MODULE_ENABLED                      /* 使能PCD模块,必须设置,否则将不会编译pcd相关代码 */

#define USBD_MAX_NUM_INTERFACES         1
#define USBD_MAX_NUM_CONFIGURATION      1
#define USBD_MAX_STR_DESC_SIZ           0x100
#define USBD_SUPPORT_USER_STRING        0 
#define USBD_SELF_POWERED               1
#define USBD_DEBUG_LEVEL                0

#define MSC_MEDIA_PACKET                32 * 1024     /* 定义包大小为32KB,越大,速度越快 */

/* 最大支持的设备数,3个 */
#define STORAGE_LUN_NBR                 3           /* 最大支持的设备数, 最大 STORAGE_LUN_NBR +1 个 */

/* 定义设备类型 */
#define DEVICE_FS       0
#define DEVICE_HS       1

/* Memory management macros */

/** Alias for memory allocation. */
#define USBD_malloc(x)          mymalloc(SRAMIN,x)

/** Alias for memory release. */
#define USBD_free(x)           myfree(SRAMIN,x)

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay


/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)   
#endif

#if (USBD_DEBUG_LEVEL > 2)                         
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)                         
#endif


#endif /* __USBD_CONF_H */
