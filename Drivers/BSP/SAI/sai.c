/**
 ****************************************************************************************************
 * @file        sai.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       SAI 驱动代码
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
 *
 ****************************************************************************************************
 */

#include "./BSP/SAI/sai.h"
#include "./SYSTEM/delay/delay.h"


SAI_HandleTypeDef g_sai1_a_handle;        /* SAI1 Block A句柄 */
DMA_HandleTypeDef g_sai1_tx_dma_handle;   /* DMA发送句柄 */

/**
 * @brief       SAI1 Block A初始化, I2S,飞利浦标准
 * @param       mode    : 00,主发送器;01,主接收器;10,从发送器;11,从接收器
 * @param       cpol    : 0,时钟下降沿选通;1,时钟上升沿选通
 * @param       datalen : 数据大小,0/1,未用到,2,8位;3,10位;4,16位;5,20位;6,24位;7,32位.
 * @retval      无
 */
void sai1_saia_init(uint8_t mode, uint8_t cpol, uint8_t datalen)
{
    HAL_SAI_DeInit(&g_sai1_a_handle);                            /* 清除以前的配置 */

    g_sai1_a_handle.Instance = SAI1_Block_A;                     /* SAI1 Bock A */
    g_sai1_a_handle.Init.AudioMode = mode;                       /* 设置SAI1工作模式 */
    g_sai1_a_handle.Init.Synchro = SAI_ASYNCHRONOUS;             /* 音频模块异步 */
    g_sai1_a_handle.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;   /* 立即驱动音频模块输出 */
    g_sai1_a_handle.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;   /* 使能主时钟分频器(MCKDIV) */
    g_sai1_a_handle.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;  /* 设置FIFO阈值,1/4 FIFO */
    g_sai1_a_handle.Init.ClockSource = SAI_CLKSOURCE_PLLI2S;     /* SIA时钟源为PLL2S */
    g_sai1_a_handle.Init.MonoStereoMode = SAI_STEREOMODE;        /* 立体声模式 */
    g_sai1_a_handle.Init.Protocol = SAI_FREE_PROTOCOL;           /* 设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议) */
    g_sai1_a_handle.Init.DataSize = datalen;                     /* 设置数据大小 */
    g_sai1_a_handle.Init.FirstBit = SAI_FIRSTBIT_MSB;            /* 数据MSB位优先 */
    g_sai1_a_handle.Init.ClockStrobing = cpol;                   /* 数据在时钟的上升/下降沿选通 */
    
    /* 帧设置 */
    g_sai1_a_handle.FrameInit.FrameLength = 64;                  /* 设置帧长度为64,左通道32个SCK,右通道32个SCK. */
    g_sai1_a_handle.FrameInit.ActiveFrameLength = 32;            /* 设置帧同步有效电平长度,在I2S模式下=1/2帧长 */
    g_sai1_a_handle.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;/* FS信号为SOF信号+通道识别信号 */
    g_sai1_a_handle.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;    /* FS低电平有效(下降沿) */
    g_sai1_a_handle.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;  /* 在slot0的第一位的前一位使能FS,以匹配飞利浦标准 */

    /* SLOT设置 */
    g_sai1_a_handle.SlotInit.FirstBitOffset = 0;                 /* slot偏移(FBOFF)为0 */
    g_sai1_a_handle.SlotInit.SlotSize = SAI_SLOTSIZE_32B;        /* slot大小为32位 */
    g_sai1_a_handle.SlotInit.SlotNumber = 2;                     /* slot数为2个 */
    g_sai1_a_handle.SlotInit.SlotActive = SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;/* 使能slot0和slot1 */
    
    HAL_SAI_Init(&g_sai1_a_handle);                             /* 初始化SAI */
    __HAL_SAI_ENABLE(&g_sai1_a_handle);                         /* 使能SAI */
}

/**
 * @brief       SAI底层驱动，引脚配置，时钟使能
 * @note        此函数会被HAL_SAI_Init()调用
 * @param       hsai  : SAI句柄
 * @retval      无
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef gpio_init_struct;

    SAI1_SAI_CLK_ENABLE();                     /* 使能SAI1时钟 */
    SAI1_CLK_GPIO_CLK_ENABLE();                /* CLK引脚时钟使能 */
    SAI1_SCK_GPIO_CLK_ENABLE();                /* SCK引脚时钟使能 */
    SAI1_FSA_GPIO_CLK_ENABLE();                /* FSA引脚时钟使能 */
    SAI1_SDA_GPIO_CLK_ENABLE();                /* SDA引脚时钟使能 */
    SAI1_SDB_GPIO_CLK_ENABLE();                /* SDB引脚时钟使能 */

    gpio_init_struct.Pin = SAI1_CLK_GPIO_PIN;  
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF6_SAI1;             /* 复用为SAI */
    HAL_GPIO_Init(SAI1_CLK_GPIO_PORT, &gpio_init_struct);   /* 初始化CLK引脚 */
    
    gpio_init_struct.Pin = SAI1_SCK_GPIO_PIN;
    HAL_GPIO_Init(SAI1_SCK_GPIO_PORT,  &gpio_init_struct);  /* 初始化SCK引脚 */
    
    gpio_init_struct.Pin = SAI1_FSA_GPIO_PIN;
    HAL_GPIO_Init(SAI1_FSA_GPIO_PORT,  &gpio_init_struct);  /* 初始化FSA引脚 */
    
    gpio_init_struct.Pin = SAI1_SDA_GPIO_PIN;
    HAL_GPIO_Init(SAI1_SDA_GPIO_PORT,  &gpio_init_struct);  /* 初始化SDA引脚 */
    
    gpio_init_struct.Pin = SAI1_SDB_GPIO_PIN;
    HAL_GPIO_Init(SAI1_SDB_GPIO_PORT,  &gpio_init_struct);  /* 初始化SDB引脚 */
}

/**
 * SAI Block A采样率设置
 * 采样率计算公式:
 * MCKDIV!=0: Fs=SAI_CK_x/[512*MCKDIV]
 * MCKDIV==0: Fs=SAI_CK_x/256
 * SAI_CK_x=(HSE/pllm)*PLLI2SN/PLLI2SQ/(PLLI2SDIVQ+1)
 * 一般HSE=25Mhz
 * pllm:在sys_stm32_clock_init设置的时候确定，一般是25
 * PLLI2SN      : 192~432
 * PLLI2SQ      : 2~15
 * PLLI2SDIVQ   : 0~31
 * MCKDIV       : 0~15
 * SAI A分频系数表@pllm = 25, HSE = 25Mhz,即vco输入频率为1Mhz
 */
const uint16_t SAI_PSC_TBL[][5]=
{
    { 800 , 344, 7, 0, 12 },    /* 8Khz采样率 */
    { 1102, 429, 2, 18, 2 },    /* 11.025Khz采样率 */
    { 1600, 344, 7, 0, 6 },     /* 16Khz采样率 */
    { 2205, 429, 2, 18,1 },     /* 22.05Khz采样率 */
    { 3200, 344, 7, 0, 3 },     /* 32Khz采样率 */
    { 4410, 429, 2, 18, 0 },    /* 44.1Khz采样率 */
    { 4800, 344, 7, 0, 2 },     /* 48Khz采样率 */
    { 8820, 271, 2, 2, 1 },     /* 88.2Khz采样率 */
    { 9600, 344, 7, 0, 1 },     /* 96Khz采样率 */
    { 17640, 271, 2, 2, 0 },    /* 176.4Khz采样率  */
    { 19200, 344, 7, 0, 0 },    /* 192Khz采样率 */
};

/**
 * @brief       开启SAIA的DMA功能,HAL库没有提供此函数
 * @note        因此我们需要自己操作寄存器编写一个
 * @param       无
 * @retval      无
 */
void sai1_saia_dma_enable(void)
{
    uint32_t tempreg = 0;
    tempreg = SAI1_Block_A->CR1;            /* 先读出以前的设置 */
    tempreg |= 1 << 17;                     /* 使能DMA */
    SAI1_Block_A->CR1 = tempreg;            /* 写入CR1寄存器中 */
}

/**
 * @brief       设置SAI1的采样率(@MCKEN)
 * @param       samplerate  : 采样率, 单位:Hz
 * @retval      0,设置成功
 *              1,无法设置
 */
uint8_t sai1_samplerate_set(uint32_t samplerate)
{   
    uint8_t i = 0; 
    
    RCC_PeriphCLKInitTypeDef rcc_sai1_sture;  

    for (i = 0; i < (sizeof(SAI_PSC_TBL) / 10); i++)                    /* 看看改采样率是否可以支持 */
    {
        if ((samplerate / 10) == SAI_PSC_TBL[i][0])break;
    }

    if (i == (sizeof(SAI_PSC_TBL) / 10))return 1;                        /* 搜遍了也找不到 */

    rcc_sai1_sture.PeriphClockSelection = RCC_PERIPHCLK_SAI_PLLI2S;      /* 外设时钟源选择 */ 
    rcc_sai1_sture.PLLI2S.PLLI2SN = (uint32_t)SAI_PSC_TBL[i][1];         /* 设置PLLI2SN */
    rcc_sai1_sture.PLLI2S.PLLI2SQ = (uint32_t)SAI_PSC_TBL[i][2];         /* 设置PLLI2SQ */

    /* 设置PLLI2SDivQ的时候SAI_PSC_TBL[i][3]要加1，因为HAL库中会在把PLLI2SDivQ赋给寄存器DCKCFGR的时候减1 */
    rcc_sai1_sture.PLLI2SDivQ = SAI_PSC_TBL[i][3] + 1;                   /* 设置PLLI2SDIVQ */
    HAL_RCCEx_PeriphCLKConfig(&rcc_sai1_sture);                          /* 设置时钟 */
    
    __HAL_RCC_SAI_BLOCKACLKSOURCE_CONFIG(RCC_SAIACLKSOURCE_PLLI2S);      /* 设置SAI1时钟来源为PLLI2SQ */

    __HAL_SAI_DISABLE(&g_sai1_a_handle);                                /* 关闭SAI */
    g_sai1_a_handle.Init.AudioFrequency = samplerate;                   /* 设置播放频率 */
    HAL_SAI_Init(&g_sai1_a_handle);                                     /* 初始化SAI */
    sai1_saia_dma_enable();                                             /* 开启SAI的DMA功能 */
    __HAL_SAI_ENABLE(&g_sai1_a_handle);                                 /* 开启SAI */

    return 0;
}   

/**
 * @brief       SAI1 TX DMA配置
 * @note        设置为双缓冲模式,并开启DMA传输完成中断
 * @param       buf0    : M0AR地址.
 * @param       buf1    : M1AR地址.
 * @param       num     : 每次传输数据量
 * @param       width   : 位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;
 * @retval      无
 */
void sai1_tx_dma_init(uint8_t *buf0, uint8_t *buf1, uint16_t num, uint8_t width)
{ 
    uint32_t memwidth = 0, perwidth = 0;      /* 外设和存储器位宽 */

    switch (width)
    {
        case 0:         /* 8位 */
            memwidth = DMA_MDATAALIGN_BYTE;
            perwidth = DMA_PDATAALIGN_BYTE;
            break;

        case 1:         /* 16位 */
            memwidth = DMA_MDATAALIGN_HALFWORD;
            perwidth = DMA_PDATAALIGN_HALFWORD;
            break;

        case 2:         /* 32位 */
            memwidth = DMA_MDATAALIGN_WORD;
            perwidth = DMA_PDATAALIGN_WORD;
            break;
    }
    SAI1_TX_DMA_CLK_ENABLE();                                        /* 使能DMA2时钟 */
    __HAL_LINKDMA(&g_sai1_a_handle, hdmatx, g_sai1_tx_dma_handle);   /* 将DMA与SAI联系起来 */

    g_sai1_tx_dma_handle.Instance = SAI1_TX_DMASx;                   /* DMA2数据流3 */
    g_sai1_tx_dma_handle.Init.Channel = SAI1_TX_DMASx_Channel;       /* 通道0 */
    g_sai1_tx_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;      /* 存储器到外设模式 */
    g_sai1_tx_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;          /* 外设非增量模式 */
    g_sai1_tx_dma_handle.Init.MemInc = DMA_MINC_ENABLE;              /* 存储器增量模式 */
    g_sai1_tx_dma_handle.Init.PeriphDataAlignment = perwidth;        /* 外设数据长度:16/32位 */
    g_sai1_tx_dma_handle.Init.MemDataAlignment = memwidth;           /* 存储器数据长度:16/32位 */
    g_sai1_tx_dma_handle.Init.Mode = DMA_CIRCULAR;                   /* 使用循环模式 */ 
    g_sai1_tx_dma_handle.Init.Priority = DMA_PRIORITY_HIGH;          /* 高优先级 */
    g_sai1_tx_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;       /* 不使用FIFO */
    g_sai1_tx_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;          /* 存储器单次突发传输 */
    g_sai1_tx_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;       /* 外设突发单次传输 */ 
    HAL_DMA_DeInit(&g_sai1_tx_dma_handle);                           /* 先清除以前的设置 */
    HAL_DMA_Init(&g_sai1_tx_dma_handle);                             /* 初始化DMA */

    HAL_DMAEx_MultiBufferStart(&g_sai1_tx_dma_handle, (uint32_t)buf0, (uint32_t)&SAI1_Block_A->DR, (uint32_t)buf1, num);/* 开启双缓冲 */
    __HAL_DMA_DISABLE(&g_sai1_tx_dma_handle);                        /* 先关闭DMA */
    delay_us(10);                                                    /* 10us延时，防止-O2优化出问题  */

    __HAL_DMA_ENABLE_IT(&g_sai1_tx_dma_handle, DMA_IT_TC);           /* 开启传输完成中断 */
    __HAL_DMA_CLEAR_FLAG(&g_sai1_tx_dma_handle, SAI1_TX_DMASx_FLAG); /* 清除DMA传输完成中断标志位 */
    HAL_NVIC_SetPriority(SAI1_TX_DMASx_IRQ, 0, 0);                   /* DMA中断优先级 */
    HAL_NVIC_EnableIRQ(SAI1_TX_DMASx_IRQ);
}

/* SAI DMA回调函数指针 */
void (*sai1_tx_callback)(void);  /* TX回调函数 */

/**
 * @brief       SAI1 TX DMA 中断服务函数
 * @param       无
 * @retval      无
 */
void SAI1_TX_DMASx_IRQHandler(void)
{   
    if(__HAL_DMA_GET_FLAG(&g_sai1_tx_dma_handle, SAI1_TX_DMASx_FLAG) != RESET) /* DMA传输完成 */
    {
        __HAL_DMA_CLEAR_FLAG(&g_sai1_tx_dma_handle, SAI1_TX_DMASx_FLAG);       /* 清除DMA传输完成中断标志位 */
        if (sai1_tx_callback != NULL)
        {
            sai1_tx_callback();  /* 执行回调函数,读取数据等操作在这里面处理 */
        }
    }
}

/**
 * @brief       SAI开始播放
 * @param       无
 * @retval      无
 */
void sai1_play_start(void)
{
    __HAL_DMA_ENABLE(&g_sai1_tx_dma_handle);   /* 开启DMA TX传输 */
}

/**
 * @brief       SAI停止播放
 * @param       无
 * @retval      无
 */
void sai1_play_stop(void)
{   
    __HAL_DMA_DISABLE(&g_sai1_tx_dma_handle);  /* 结束播放 */
} 


