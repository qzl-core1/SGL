#ifndef __SGL_H
#define __SGL_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "sgl_core/sgl_obj.h"
#include "sgl_core/sgl_driver.h"
#include "sgl_core/sgl_draw.h"
#include "sgl_core/sgl_anim.h"
#include "sgl_core/sgl_timer.h"

/* 这里属于用户头文件区域 */
#include "debug.h"
#include "tick.h"
/* 这里属于用户头文件区域 */




//屏幕长宽
#define SGL_SCREEN_H 240     //y轴大小
#define SGL_SCREEN_W 240    //x轴大小


#define DEBUG_ENABLE  0 //开启日志打印功能,1则开启,在32行的宏替换中加入自己printf函数

/* 如果开启,则添加自己的printf函数 */
#define sgl_log debug_printf

/* 定义刷新率,单位是帧 */
#define SGL_FPS 100  

/* 色深设置,单位是bit */
#define Color_depth 16



/* 单色屏幕由于硬件限制,没法局部刷新,所以必须需要一个屏幕缓冲的 */
#if Color_depth==1
/* 为屏幕存放多少行缓冲 */
#define SGL_LOACL_BUF_RATING SGL_SCREEN_H
/* 屏幕所需要的缓冲 */
#define SGL_BUF_SIZE SGL_LOACL_BUF_RATING*SGL_SCREEN_W/8 //除8是因为一个像素才一个Bit
#endif

/* 16位色的屏幕 */
#if Color_depth==16
/* 为屏幕存放多少行缓冲,最大是一个屏幕的高度-->SGL_SCREEN_W */
#define SGL_LOACL_BUF_RATING 80
/* 屏幕所需要的缓冲 */
#define SGL_BUF_SIZE SGL_LOACL_BUF_RATING*SGL_SCREEN_W*2//乘上2是因为一个像素点两个字节
#endif



/* 添加一个获取时间戳的函数 */
#define sgl_get_Timestamp get_tick

/* 添加一个内存申请函数,默认使用malloc */
#define sgl_malloc malloc

/* 添加一个内存释放函数,默认是free */
#define sgl_free free

void sgl_init(void);
void sgl_run(void);
uint32_t sgl_tick_stampe_interval(uint32_t pre_tick);

#endif
