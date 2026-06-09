/**
 ****************************************************************************************************
 * @file        test.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       阿波罗无屏幕测试流程
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F429开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./TEST/test.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/IIC/myiic.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/REMOTE/remote.h"
#include "./BSP/ES8388/es8388.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/ST480MC/st480mc.h"
#include "./BSP/SH3001/imu.h"
#include "./BSP/SH3001/sh3001.h"
#include "./BSP/PCF8574/pcf8574.h"
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/NAND/ftl.h"
#include "./BSP/NAND/nand.h"
#include "./MALLOC/malloc.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "usbd_conf.h"
#include "./FATFS/source/diskio.h"
#include "./FATFS/source/ff.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/fonts.h"
#include "./TEXT/fonts.h"
#include "./TEXT/text.h"
#include "./APP/audioplay.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "./TEXT/text.h"
#include "./AUDIOCODEC/wav/wavplay.h"
#include <math.h>

/*************************************************************************************************/
USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
extern volatile uint8_t g_usb_state_reg;    /* USB状态 */
extern volatile uint8_t g_device_state;     /* USB连接 情况 */

/**
 * @brief       USB连接测试
 * @param       无
 * @retval      无
 */
void usb_test(void)
{
    printf("/******************************* 2，USB测试 *******************************/\r\n");
    printf("/* 注意：请将USB线连接到USB_SLAVE接口上，然后观察电脑磁盘是否有出现新的磁盘！ */\r\n");
    printf("/******************************* NEXT: KEY1 *******************************/\r\n\r\n");
    
    uint8_t offline_cnt = 0;
    uint8_t tct = 0;
    uint8_t usb_sta;
    uint8_t device_sta;
    uint16_t id;
    uint64_t card_capacity;                              /* SD卡容量 */

    if (sd_init())  /* 初始化SD卡 */
    {
        printf("SD卡初始化失败或者未插卡！！！\r\n");
    }
    else            /* SD 卡正常 */
    {
        card_capacity = (uint64_t)(g_sd_card_info_handle.LogBlockNbr) * (uint64_t)(g_sd_card_info_handle.LogBlockSize); /* 计算SD卡容量 */
        printf("SD卡正常，容量大小为：%lldM\r\n",card_capacity);
    }
    
    id = norflash_read_id();
    
    if ((id == 0) || (id == 0XFFFF))
    {
        printf("SPI_FLASH ERROR\r\n");
    }
    else   /* SPI FLASH 正常 */
    {
        printf("SPI_FLASH 正常\r\n");
    }
    if (ftl_init())
    {
        printf("NAND ERROR\r\n");
    }
    else
    {
        printf("NAND FLash Size:%dMB\r\n", nand_dev.valid_blocknum * nand_dev.block_pagenum * nand_dev.page_mainsize >> 20);
    }
        
    USBD_Init(&USBD_Device, &MSC_Desc, DEVICE_FS);   /* 初始化USB */
    USBD_RegisterClass(&USBD_Device, &USBD_MSC);    /* 添加类 */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_Storage_Interface_fops_FS);    /* 为MSC类添加回调函数 */
    USBD_Start(&USBD_Device);                       /* 开启USB */
    delay_ms(500);
    
    while(1)
    {
        delay_ms(1);

        if (usb_sta != g_usb_state_reg)             /* 状态改变了 */
        {
            if (g_usb_state_reg & 0x01)             /* 正在写 */
            {
                LED1(0);
                printf("USB Writing...\r\n");
            }

            if (g_usb_state_reg & 0x02)             /* 正在读 */
            {
                LED1(0);
                printf("USB Reading...\r\n");
            }

            if (g_usb_state_reg & 0x04)
            {
                printf("USB Write Err\r\n");
            }
            
            if (g_usb_state_reg & 0x08)
            {
                printf("USB Read  Err \r\n");
            }

            
            usb_sta = g_usb_state_reg;              /* 记录最后的状态 */
        }

        if (device_sta != g_device_state)
        {
            if (g_device_state == 1)
            {
                printf("USB连接成功！！\r\n");
            }
            else
            {
                printf("USB连接失败！！\r\n");
            }
            
            device_sta = g_device_state;
        }

        tct++;
        if (key_scan(0) == KEY1_PRES) 
        {
            USBD_Stop(&USBD_Device);
            break;        /* 当按下按键KEY1就退出 */
        }
        if (tct == 200)
        {
            tct = 0;
            LED1(1);        /* 关闭 LED1 */
            LED0_TOGGLE();  /* LED0 闪烁 */

            if (g_usb_state_reg & 0x10)
            {
                offline_cnt = 0;                    /* USB连接了,则清除offline计数器 */
                g_device_state = 1;
            }
            else    /* 没有得到轮询 */
            {
                offline_cnt++;

                if (offline_cnt > 10)
                {
                    g_device_state = 0;             /* 2s内没收到在线标记,代表USB被拔出了 */
                }
            }

            g_usb_state_reg = 0;
        }
    }
}

/**
 * @brief       LED测试
 * @param       无
 * @retval      无
 */
void led_tset()
{
    printf("/******************************* 1，LED测试 *******************************/\r\n");
    printf("/********************* 注意：观察LED0和LED1是否闪烁！*********************/\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");
    while (1)
    {
        LED0_TOGGLE();                              /* LED0翻转 */
        LED1_TOGGLE();                              /* LED1翻转 */
        delay_ms(50);
        if (key_scan(0) == KEY1_PRES) break;        /* 当按下按键KEY1就退出 */
    }  
}

/**
 * @brief       蜂鸣器测试
 * @param       无
 * @retval      无
 */
void beep_test(void)
{
    printf("/***************************** 3，蜂鸣器测试 ***************************/\r\n");
    printf("/********************* 注意：听下蜂鸣器是否鸣叫！***********************/\r\n");
    printf("/****************************** NEXT: KEY1 ****************************/\r\n\r\n");
    while (1)
    {  
        pcf8574_write_bit(BEEP_IO,1);               /* 关闭蜂鸣器 */
        delay_ms(50);
        pcf8574_write_bit(BEEP_IO,0);               /* 打开蜂鸣器 */
        delay_ms(50);
        if (key_scan(0) == KEY1_PRES) break;        /* 当按下按键KEY1就退出 */
    }   
    pcf8574_write_bit(BEEP_IO,1);                   /* 关闭蜂鸣器 */
}

/**
 * @brief       按键测试
 * @param       无
 * @retval      无
 */
void key_test(void)
{
    printf("/*************************** 4，按键测试 ******************************/\r\n");
    printf("/*********************全部按键按下，即可退出！*************************/\r\n"); 
    uint8_t key = 0;
    uint8_t status = 0;
    
    while(1)
    {
        key = key_scan(0);
        if (key)
        {
            status |= (1 <<(key - 1));
            switch (key)
            {
                case KEY0_PRES: printf("按键KEY0被按下\r\n");    break;
                case KEY1_PRES: printf("按键KEY1被按下\r\n");    break;
                case KEY2_PRES: printf("按键KEY2被按下\r\n");    break;
                case WKUP_PRES: printf("按键KEY_UP被按下\r\n");  break;
            }
        }
        if (status == 0x0F) 
        {
            printf("按键全部按下，正常退出！\r\n");
            delay_ms(100);
            return;
        }
    }
}

/**
 * @brief       红外遥控测试
 * @param       无
 * @retval      无
 */
void remote_test(void)
{
    printf("\r\n/******************************* 5，红外遥控器测试 ********************/\r\n");
    printf("/***** 注意：按下红外遥控器，串口输出对应键值，当按下Power将退出测试 *****/\r\n");
    printf("/****************************** NEXT: POWER ****************************/\r\n\r\n");
    uint8_t key , old_key = 0;
    uint8_t t = 0;
    char *str = 0;
    remote_init();
        
    while(1)
    {   
        /* 测试红外 */
        key = remote_scan();
        
        if (key)
        {
            switch (key)
            {
                case 0:
                    str = "ERROR";
                    break;

                case 69:
                    str = "POWER";
                    break;

                case 70:
                    str = "UP";
                    break;

                case 64:
                    str = "PLAY";
                    break;

                case 71:
                    str = "ALIENTEK";
                    break;

                case 67:
                    str = "RIGHT";
                    break;

                case 68:
                    str = "LEFT";
                    break;

                case 7:
                    str = "VOL-";
                    break;

                case 21:
                    str = "DOWN";
                    break;

                case 9:
                    str = "VOL+";
                    break;

                case 22:
                    str = "1";
                    break;

                case 25:
                    str = "2";
                    break;

                case 13:
                    str = "3";
                    break;

                case 12:
                    str = "4";
                    break;

                case 24:
                    str = "5";
                    break;

                case 94:
                    str = "6";
                    break;

                case 8:
                    str = "7";
                    break;

                case 28:
                    str = "8";
                    break;

                case 90:
                    str = "9";
                    break;

                case 66:
                    str = "0";
                    break;

                case 74:
                    str = "DELETE";
                    break;
            }
            if(old_key != key)
            {
                printf("%s\r\n",str);
                old_key = key;
            }
            if (key == 69) break;                       /* 当按下POWER按键就退出 */
        }
        else
        {
            delay_ms(10);
        }
        t++;

        if (t == 20)
        {
            t = 0;
            LED0_TOGGLE();    /* LED0闪烁 */
        }
    }
}

/**
 * @brief       光环境传感器测试
 * @param       无
 * @retval      无
 */
void ap3216c_test(void)
{
    uint16_t t = 0;
    uint16_t ir, als, ps;
    printf("\r\n/**************************** 6，光环境传感器测试 ************************/\r\n");
    printf("/*** 注意：可以遮挡LS1，并观察光环境传感器数值的变化，光线越亮，值越大 ***/\r\n");
    printf("/****************************** NEXT: KEY1 *******************************/\r\n\r\n");
    ap3216c_init();                                /* 初始化光环境传感器 */

    while (1)
    {
        t++;
        delay_us(20);
        if ((t % 200) == 0)
        {
            ap3216c_read_data(&ir, &ps, &als);
            printf("IR:%d  \r\n", ir);
            printf("PS:%d  \r\n", ps);
            printf("ALS:%d  \r\n", als);
            
            LED0_TOGGLE();
            delay_ms(10);
            if (key_scan(0) == KEY1_PRES) break;                /* 当按下按键KEY1就退出 */
        }
    }
}

/******************************************* 磁力计相关 ******************************************/

/* 磁力计平面校准参数 
 * 校准原理请参考(方法2):http://blog.sina.com.cn/s/blog_402c071e0102v8ie.html
 */
int16_t g_magx_offset;      /* x轴补偿值 */
int16_t g_magy_offset;      /* y轴补偿值 */

/**
 * @brief       显示磁力计原始数据
 * @param       x, y : 坐标
 * @param       title: 标题
 * @param       val  : 值
 * @retval      无
 */
void user_show_mag(uint16_t x, uint16_t y, char * title, int16_t val)
{
    char buf[20];
    sprintf(buf,"%s%d", title, val);                /* 格式化输出 */
    printf("%s\r\n",buf);
}

/**
 * @brief       显示方位角
 * @param       x, y : 坐标
 * @param       angle: 角度
 * @retval      无
 */
void user_show_angle(uint16_t x, uint16_t y, float angle)
{
    char buf[20];
    sprintf(buf,"Angle:%3.1fC", angle);             /* 格式化输出 */
    printf("方位角：%s\r\n",buf);
}

/**
 * @brief       显示温度
 * @param       x, y : 坐标
 * @param       temp : 温度
 * @retval      无
 */
void user_show_temprate(uint16_t x, uint16_t y, float temp)
{
    char buf[20];
    sprintf(buf,"Temp:%2.1fC", temp);               /* 格式化输出 */
    printf("温度：%s\r\n",buf);
}

/**
 * @brief       罗盘(磁力计)校准函数
 *   @note      这里我们使用最简单的平面校准方法.
 *              进入此函数后,请水平转动开发板至少一周(360°),转动完成后, 按WKUP键退出!
 * @param       无
 * @retval      无
 */
void compass_calibration(void)
{
    int16_t x_min = 0;
    int16_t x_max = 0;
    int16_t y_min = 0;
    int16_t y_max = 0;

    int16_t magx, magy, magz;
    uint8_t res;
    uint8_t key = 0;
    
    printf("按键KEY1退出校准磁力计\r\n");

    while (1)
    {
        key = key_scan(0);
        
        if (key == WKUP_PRES)   /* 结束校准 */
        {
            break;
        }
        
        res = st480mc_read_magdata(&magx, &magy, &magz);    /* 读取数据 */
        
        if (res == 0)
        {
            x_max = x_max < magx ? magx : x_max;            /* 记录x最大值 */
            x_min = x_min > magx ? magx : x_min;            /* 记录x最小值 */
            y_max = y_max < magy ? magy : y_max;            /* 记录y最大值 */
            y_min = y_min > magy ? magy : y_min;            /* 记录y最小值 */
        }
        
        LED0_TOGGLE();                                      /* LED0闪烁,提示程序运行 */
    }
    
    g_magx_offset = (x_max + x_min) / 2;                    /* X轴偏移量 */
    g_magy_offset = (y_max + y_min) / 2;                    /* Y轴偏移量 */
   
    /* 串口打印水平校准相关参数 */
    printf("x_min:%d\r\n", x_min);
    printf("x_max:%d\r\n", x_max);
    printf("y_min:%d\r\n", y_min);
    printf("y_max:%d\r\n", y_max);
    
    printf("g_magx_offset:%d\r\n", g_magx_offset);
    printf("g_magy_offset:%d\r\n", g_magy_offset);
}

/**
 * @brief       罗盘获取角度
 * @note        获取当前罗盘的角度(地磁角度)
 * @param       无
 * @retval      角度
 */
float compass_get_angle(void)
{
    float angle;
    int16_t magx, magy, magz;
    
    st480mc_read_magdata_average(&magx, &magy, &magz, 10);  /* 读取原始数据, 10次取平均 */
    
    magx = (magx - g_magx_offset) ;     /* 根据校准参数, 计算新的输出 */
    magy = (magy - g_magy_offset) ;     /* 根据校准参数, 计算新的输出 */

    /* 根据不同的象限情况, 进行方位角换算 */
    if ((magx > 0) && (magy > 0))
    {
        angle = (atan((double)magy / magx) * 180) / 3.14159f;
    }
    else if ((magx > 0) && (magy < 0))
    {
        angle = 360 + (atan((double)magy / magx) * 180) / 3.14159f;
    }
    else if ((magx == 0) && (magy > 0))
    {
        angle = 90;
    }
    else if ((magx == 0) && (magy < 0))
    {
        angle = 270;
    }
    else if (magx < 0)
    {
        angle = 180 + (atan((double)magy / magx) * 180) / 3.14159f;
    }
    
    if (angle > 360) angle = 360; /* 限定方位角范围 */
    if (angle < 0) angle = 0;     /* 限定方位角范围 */

    return angle;
}

/**
 * @brief       磁力计测试
 * @param       无
 * @retval      无
 */
void st480mc_test(void)
{
    printf("\r\n/******************************* 7，磁力计测试 **************************/\r\n");
    printf("/********************* 注意：观察磁力计输出数据是否正常 *****************/\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");

    uint8_t t = 0;
    uint8_t key;
    float angle;
    float temperature;
    int16_t magx, magy, magz;
    while (st480mc_init())                              /* ST480MC初始化 */
    {
        printf("ST480MC Error!!\r\n");
        delay_ms(500);
    }
    RST :
    printf("ST480MC OK!!\r\n");
    while(1)
    {
        key = key_scan(0);
        delay_ms(10);
        t++;
        
        if (t == 20)                                    /* 0.2秒左右更新一次温度/磁力计原始值 */
        {        
            angle = compass_get_angle();                /* 执行一次约150ms */
            user_show_angle(30, 130, angle);            /* 显示角度 */
      
            st480mc_read_temperature(&temperature);     /* 读取温湿度值 */
            user_show_temprate(30, 150, temperature);   /* 显示温度 */
            
            st480mc_read_magdata(&magx, &magy, &magz);  /* 读取原始数据 */
            user_show_mag(30, 170, "MagX:", magx);      /* 显示magx */
            user_show_mag(30, 190, "MagY:", magy);      /* 显示magy */
            user_show_mag(30, 210, "MagZ:", magz);      /* 显示magz */
            
            t = 0;
            LED0_TOGGLE();                              /* LED0闪烁 */
        }
        if (key == KEY0_PRES)                           /* KEY0 按下 ,执行校准 */
        {
            compass_calibration();                      /* 校准函数 */
            goto RST;                                   /* 校准完后,跳到RST, 重新显示提示信息 */
        }else if (key == KEY1_PRES) break;              /* 当按下按键KEY1就退出 */
    }
}

/**
 * @brief       六轴传感器测试
 * @param       无
 * @retval      无
 */
void sh3001_test(void)
{
    printf("\r\n/***************************** 8，六轴传感器测试 ***********************/\r\n");
    printf("/************** 注意：观察六轴传感器输出数据是否正常 *******************/\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");
    
    uint8_t t = 0;
    eulerian_angles_t e_angles;
    short acc_data[3];                          /* 加速度传感器原始数据 */
    short gyro_data[3];                         /* 陀螺仪原始数据 */
    
    imu_init();                                 /* 初始化 IMU */

    while (1)
    {
        t++;
        /* 读取传感器数据 */
        sh3001_get_imu_compdata(acc_data, gyro_data);

        /* 数据校准 */
        imu_data_calibration(&gyro_data[0], &gyro_data[1], &gyro_data[2],
                             &acc_data[0],  &acc_data[1],  &acc_data[2]);

        /* 获取欧拉角 */
        e_angles = imu_get_eulerian_angles(gyro_data[0], gyro_data[1], gyro_data[2],
                                           acc_data[0],  acc_data[1],  acc_data[2]);
        if ((t % 200) == 0)
        {
            t = 0;
            printf("\r\nPITCH:%f\r\n", e_angles.pitch);
            printf("ROLL:%f\r\n", e_angles.roll);
            printf("YAW:%f\r\n", e_angles.yaw);
            
            LED0_TOGGLE();
            delay_ms(10);
            if (key_scan(0) == KEY1_PRES) break;                /* 当按下按键KEY1就退出 */
        }
    }
}

/**
 * @brief       mp3测试
 * @param       无
 * @retval      无
 */
void wav_test(void)
{
    printf("\r\n/******************************* 9，MP3测试 *******************************/\r\n");
    printf("/* 注意：需插入SD卡，并且SD卡需创建MUSIC文件夹，将test.wav音乐文件拷贝进去 */\r\n");
    printf("/****************************** NEXT: KEY0 ******************************/\r\n\r\n");
    
    const char *mp3_path = "0:/MUSIC/TEST.wav";

    f_mount(fs[0], "0:", 1);    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);    /* 挂载SPI FLASH */
    exfuns_init();              /* FATFS 申请内存 */
    
    while (sd_init())           /* 初始化SD卡 */
    {
        printf("SD卡错误或未插入SD卡\r\n");
        delay_ms(500);
    }
    es8388_init();              /* ES8388初始化 */
    es8388_adda_cfg(1, 0);      /* 开启DAC关闭ADC */
    es8388_output_cfg(1, 1);    /* DAC选择通道输出 */
    es8388_hpvol_set(25);       /* 设置耳机音量 */
    es8388_spkvol_set(30);      /* 设置喇叭音量 */
    es8388_adda_cfg(1, 0);      /* 开启DAC关闭ADC */
    es8388_output_cfg(1, 1);    /* DAC选择通道1输出 */
    
    while(1)
    {
        wav_play_song((char*)mp3_path);
        break;                /* 当按下按键KEY1就退出 */
    }

}

/**
 * @brief       ADC测试
 * @param       无
 * @retval      无
 */
void adc_tset()
{
    printf("\r\n/******************************* 10，ADC测试 *******************************/\r\n");
    printf("/ 注意：使用杜邦线将P11处的ADC与RV1进行短接，然后调节RV1观看ADC读数变化！！ /\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");
    uint16_t adcx,t = 0;
    float temp;
    adc_init();
    while (1)
    {
        if(t++ > 100)
        {
            t = 0;
            adcx = adc_get_result_average(ADC_ADCX_CHY, 10);    /* 获取通道14的转换值，10次取平均 */
            printf("ADC原始值：%d\r\n",adcx);
            temp = (float)adcx * (3.3 / 4096);                  /* 获取计算后的带小数的实际电压值，比如3.1111 */
            printf("ADC电压值：%.2fV\r\n",temp);
        }
        delay_ms(10);
        if (key_scan(0) == KEY1_PRES) break;                    /* 当按下按键KEY1就退出 */
    }  
}


/**
 * @brief       SDRAM内存测试
 * @param       无
 * @retval      无
*/
void sdram_test(void)
{
    printf("\r\n/******************************* 11，SDRAM测试 ****************************/\r\n");
    printf("/*********************** 注意：按下KEY0测试SDRAM容量 *********************/\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");

    uint32_t i = 0;
    uint32_t temp = 0;
    uint32_t sval = 0;       /* 在地址0读到的数据 */

    while (1)
    {
        if (key_scan(0) == KEY0_PRES) 
        {
            /* 每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节 */
            for (i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
            {
                *(volatile uint32_t *)(BANK5_SDRAM_ADDR + i) = temp; 
                temp++;
            }

            /* 依次读出之前写入的数据,进行校验 */
            for (i = 0; i < 32 * 1024 * 1024; i += 16 * 1024) 
            {
                temp =*(volatile uint32_t*)(BANK5_SDRAM_ADDR + i);

                if (i == 0)
                {
                    sval = temp;
                }
                else if (temp <= sval)
                {
                    break;         /* 后面读出的数据一定要比第一次读到的数据大 */
                }

                printf("SDRAM Capacity:%dKB\r\n", (uint16_t)(temp - sval + 1) * 16);           /* 打印SDRAM容量 */
            }
        }
        else if (key_scan(0) == KEY1_PRES)
        {
            break;
        }
    }
}

/**
 * @brief       网络测试
 * @param       无
 * @retval      无
 */
void net_test(void)
{
    printf("\r\n/****************************** 12，网络测试 ****************************/\r\n");
    printf("/*************** 注意：请将可正常上网的网线插入板子的以太网接口 *************/\r\n");
    printf("/****************************** NEXT: AUTO *******************************/\r\n\r\n");
    
    while (lwip_comm_init() != 0)
    {
        printf("lwIP Init failed!!\r\n");
        printf("请插入网线！！\r\n"); 
        delay_ms(500);
        LED1_TOGGLE();
    }
    while (!ethernet_read_phy(PHY_SR))  /* 检查MCU与PHY芯片是否通信成功 */
    {
        printf("MCU与PHY芯片通信失败，请检查电路或者源码！！！！\r\n");
    }
    
    printf("网络通信成功！！\r\n");
    pcf8574_write_bit(ETH_RESET_IO,1);       /* 硬件复位PHY 必须复位，否则只要插上网线，就会有数据干扰  */
}

/
/**
 * @brief       LCD测试
 * @param       无
 * @retval      无
 */
void lcd_test(void)
{
    printf("/******************************* 13，LCD测试 *******************************/\r\n");
    printf("/* 注意：观察LCD屏幕是否显示彩色条纹和文字！*********************/\r\n");
    printf("/****************************** NEXT: KEY1 ******************************/\r\n\r\n");

    /* 清屏黑色 */
    lcd_clear(BLACK);
    delay_ms(200);

    /* 画彩色条纹 */
    lcd_fill(0, 0, lcddev.width, lcddev.height / 6, RED);
    lcd_fill(0, lcddev.height / 6, lcddev.width, lcddev.height / 6 * 2, GREEN);
    lcd_fill(0, lcddev.height / 6 * 2, lcddev.width, lcddev.height / 6 * 3, BLUE);
    lcd_fill(0, lcddev.height / 6 * 3, lcddev.width, lcddev.height / 6 * 4, YELLOW);
    lcd_fill(0, lcddev.height / 6 * 4, lcddev.width, lcddev.height / 6 * 5, CYAN);
    lcd_fill(0, lcddev.height / 6 * 5, lcddev.width, lcddev.height, MAGENTA);
    delay_ms(500);

    /* 清屏白色，显示文字 */
    lcd_clear(WHITE);
    lcd_set_cursor(0, 0);
    lcd_write_ram_prepare();

    text_show_string_middle(0, 30, "Apollo V2 F429", 24, lcddev.width, RED);
    text_show_string_middle(0, 70, "LCD Test OK!", 16, lcddev.width, BLUE);

    /* 画矩形 */
    lcd_draw_rectangle(10, 110, lcddev.width - 10, 150, RED);
    /* 画圆 */
    lcd_draw_circle(lcddev.width / 2, 200, 40, GREEN);
    /* 画线 */
    lcd_draw_line(10, 260, lcddev.width - 10, 260, BLUE);

    /* 显示LCD ID */
    lcd_show_string(10, 280, 200, 16, "LCD ID:", 16, 0);
    lcd_show_xnum(80, 280, lcddev.id, 8, 16, 0, BLACK);

    while (1)
    {
        if (key_scan(0) == KEY1_PRES) break;        /* 按下KEY1退出 */
    }

    lcd_clear(BLACK);                               /* 退出时清屏 */
}

\r\n**
 * @brief       发货测试流程
 * @param       无
 * @retval      无
 */
void test_system(void)
{
    printf("\r\n/******************************* 无屏幕验收流程 *****************************/\r\n");
    
    at24cxx_init();
    while ( at24cxx_check() )                   /* 检查24c02是否正常，需要读取24c02的一个标志判断是否进行过出厂测试 */
    {
        printf("24C02 ERROR!\r\n");
        delay_ms(100);
    }    
        
    /* LED测试 */
    led_tset();
    
    /* usb连接测试 */
    usb_test();
    
    /* 蜂鸣器测试 */
    beep_test();
    
    /* 按键测试 */
    key_test();
    
    /* 红外遥控器测试 */
    remote_test();
    
    /* 光环境测试 */
    ap3216c_test();
    
    /* 磁力计测试 */
    st480mc_test();
    
    /* 六轴传感器测试 */
    sh3001_test();
    
    /* wav测试 */
    wav_test();
    
    /* ADC测试 */
    adc_tset();
    
    /* SDRAM测试 */
    sdram_test();

    /* LCD测试 */
    lcd_test();

    /* 网络测试 */
    net_test();     
    
    printf("\r\n/******************************* 测试完成 *******************************/\r\n");
}



