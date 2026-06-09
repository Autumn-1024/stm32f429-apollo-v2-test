/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-4-20
 * @brief       SPI 驱动代码
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
 * V1.0 20220420
 * 第一次发布
 * V1.1 20220420
 * 新增SPI2驱动代码
 *
 ****************************************************************************************************
 */

#include "./BSP/SPI/spi.h"


SPI_HandleTypeDef g_spi2_handle;                          /* SPI句柄 */
SPI_HandleTypeDef g_spi5_handle;                          /* SPI句柄 */

/**
 * @brief       SPI口初始化SPI
 * @note        模块的初始化代码，配置成主机模式
                这里针是对SPI5的初始化
 * @param       无
 * @retval      无
 */
void spi5_init(void)
{
    g_spi5_handle.Instance = SPI5_SPI;                     /* SP5 */
    g_spi5_handle.Init.Mode = SPI_MODE_MASTER;             /* 设置SPI工作模式，设置为主模式 */
    g_spi5_handle.Init.Direction = SPI_DIRECTION_2LINES;   /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    g_spi5_handle.Init.DataSize = SPI_DATASIZE_8BIT;       /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    g_spi5_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;    /* 串行同步时钟的空闲状态为高电平 */
    g_spi5_handle.Init.CLKPhase = SPI_PHASE_2EDGE;         /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
    g_spi5_handle.Init.NSS = SPI_NSS_SOFT;                 /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    g_spi5_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;  /* 定义波特率预分频的值:波特率预分频值为256 */
    g_spi5_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;        /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    g_spi5_handle.Init.TIMode = SPI_TIMODE_DISABLE;        /* 关闭TI模式 */
    g_spi5_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    g_spi5_handle.Init.CRCPolynomial = 7;                  /* CRC值计算的多项式 */
    HAL_SPI_Init(&g_spi5_handle);                          /* 初始化 */
    
    __HAL_SPI_ENABLE(&g_spi5_handle);                      /* 使能SPI5 */

    spi5_read_write_byte(0Xff);                            /* 启动传输 */
}

/**
 * @brief       SPI5底层驱动，时钟使能，引脚配置
 * @note        此函数会被HAL_SPI_Init()调用
 * @param       hspi:SPI句柄
 * @retval      无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct;

    SPI5_SCK_GPIO_CLK_ENABLE();                           /* SCK脚时钟使能 */
    SPI5_MISO_GPIO_CLK_ENABLE();                          /* MISO脚时钟使能 */
    SPI5_MOSI_GPIO_CLK_ENABLE();                          /* MISO脚时钟使能 */
    SPI5_SPI_CLK_ENABLE();                                /* 使能SPI5时钟 */
    
    /* PF7,8,9 */
    gpio_init_struct.Pin = SPI5_SCK_GPIO_PIN | SPI5_MISO_GPIO_PIN | SPI5_MOSI_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;              /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FAST;             /* 快速 */
    gpio_init_struct.Alternate = GPIO_AF5_SPI5;           /* 复用为SPI5 */
    HAL_GPIO_Init(SPI5_SCK_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       SPI5速度设置函数
 * @note        SPI5时钟选择来自APB2, 即PCLK2, 为90Mhz
 *              SPI速度 = PCLK2 / 2^(speed + 1)
 * @param       speed   : SPI时钟分频系数
 * @retval      无
 */
void spi5_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed));/* 判断有效性 */
    __HAL_SPI_DISABLE(&g_spi5_handle);             /* 关闭SPI */
    g_spi5_handle.Instance->CR1 &= 0XFFC7;         /* 位3-5清零，用来设置波特率 */
    g_spi5_handle.Instance->CR1 |= speed;          /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&g_spi5_handle);              /* 使能SPI */
}

/**
 * @brief       SPI5读写一个字节数据
 * @param       txdata  : 要发送的数据(1字节)
 * @retval      读取到的字节
 */
uint8_t spi5_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    HAL_SPI_TransmitReceive(&g_spi5_handle, &txdata, &rxdata, 1, 1000);

    return rxdata;                      /* 返回收到的数据 */
}

/**
 * @brief       SPI2初始化代码
 * @note        主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
void spi2_init(void)
{
    g_spi2_handle.Instance = SPI2_SPI;                     /* SPI2 */
    g_spi2_handle.Init.Mode = SPI_MODE_MASTER;             /* 设置SPI工作模式，设置为主模式 */
    g_spi2_handle.Init.Direction = SPI_DIRECTION_2LINES;   /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    g_spi2_handle.Init.DataSize = SPI_DATASIZE_8BIT;       /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    g_spi2_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;    /* 串行同步时钟的空闲状态为高电平 */
    g_spi2_handle.Init.CLKPhase = SPI_PHASE_2EDGE;         /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
    g_spi2_handle.Init.NSS = SPI_NSS_SOFT;                 /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    g_spi2_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;/* 定义波特率预分频的值:波特率预分频值为256 */
    g_spi2_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;        /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    g_spi2_handle.Init.TIMode = SPI_TIMODE_DISABLE;        /* 关闭TI模式 */
    g_spi2_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;/* 关闭硬件CRC校验 */
    g_spi2_handle.Init.CRCPolynomial = 7;                  /* CRC值计算的多项式 */
    HAL_SPI_Init(&g_spi2_handle);                          /* 初始化 */
    
    __HAL_SPI_ENABLE(&g_spi2_handle);                      /* 使能SPI2 */

    spi2_read_write_byte(0Xff);                            /* 启动传输 */
}

///**
// * @brief       SPI2底层驱动，时钟使能，引脚配置
// * @note        此函数会被HAL_SPI_Init()调用
// * @param       hspi:SPI句柄
// * @retval      无
// */
//void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    
//    SPI2_SCK_GPIO_CLK_ENABLE();                           /* SCK脚时钟使能 */
//    SPI2_MISO_GPIO_CLK_ENABLE();                          /* MISO脚时钟使能 */
//    SPI2_MOSI_GPIO_CLK_ENABLE();                          /* MOSI脚时钟使能 */
//    SPI2_SPI_CLK_ENABLE();                                /* 使能SPI2时钟 */
//    
//    /* PB13,14,15 */
//    gpio_init_struct.Pin = SPI2_SCK_GPIO_PIN | SPI2_MISO_GPIO_PIN | SPI2_MOSI_GPIO_PIN;
//    gpio_init_struct.Mode = GPIO_MODE_AF_PP;              /* 复用推挽输出 */
//    gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
//    gpio_init_struct.Speed = GPIO_SPEED_FAST;             /* 快速 */
//    gpio_init_struct.Alternate = GPIO_AF5_SPI2;           /* 复用为SPI2 */
//    HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &gpio_init_struct);
//}

/**
 * @brief       SPI2速度设置函数
 * @note        SPI2时钟选择来自APB1, 即PCLK1, 为45Mhz
 *              SPI速度 = PCLK2 / 2^(speed + 1)
 * @param       speed   : SPI时钟分频系数
 * @retval      无
 */
void spi2_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed));          /* 判断有效性 */
    __HAL_SPI_DISABLE(&g_spi2_handle);                       /* 关闭SPI */
    g_spi2_handle.Instance->CR1 &= 0XFFC7;                   /* 位3-5清零，用来设置波特率 */
    g_spi2_handle.Instance->CR1 |= speed;                    /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&g_spi2_handle);                        /* 使能SPI */
}

/**
 * @brief       SPI2 读写一个字节
 * @param       txdata:要写入的字节
 * @retval      读取到的字节
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    HAL_SPI_TransmitReceive(&g_spi2_handle, &txdata, &rxdata, 1, 1000);

    return rxdata;                      /* 返回收到的数据 */
}
