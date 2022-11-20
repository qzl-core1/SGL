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

/* 这里属于用户头文件区域 */
#include "debug.h"
#include "tick.h"
/* 这里属于用户头文件区域 */




//屏幕长宽
#define SGL_SCREEN_H 64     //y轴大小
#define SGL_SCREEN_W 128    //x轴大小

#define SGL_SCREEN_SIZE SGL_SCREEN_H*SGL_SCREEN_W/8

#define DEBUG_ENABLE 1//开启日志打印功能,1则开启,在32行的宏替换中加入自己printf函数

#if DEBUG_ENABLE
#define log_printf debug_printf
#endif

/* 添加一个获取时间戳的函数 */
#define sgl_get_Timestamp get_tick

/* 添加一个内存申请函数,默认使用malloc */
#define sig_malloc malloc

/* 添加一个内存释放函数,默认是free */
#define sig_free free

void sgl_init(void);
void sgl_handle(void);
uint32_t sgl_tick_stampe(uint32_t pre_tick);

#endif
