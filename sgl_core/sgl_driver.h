#ifndef __SGL_DRIVER_H
#define __SGL_DRIVER_H
#include "sgl.h"


/* 用户文件包含 */
#include "ssd1306.h"
/* 用户文件包含 */

//1.显存块1在发送(gram_in_transmit),显存块2渲染完毕,显存块2进入draw_finsh,等待显存块1完成while,切换显存块2发送
//2.显存块1完成发送(gram_transmit_finsh),显存块2未渲染完毕,当渲染完成直接切换
typedef enum//内存状态枚举
{
    gram_in_transmit = 0x01,//显存块正在发送
    gram_in_draw = 0x02,//显存块正在渲染
    gram_draw_finish = 0x03,//显存块渲染完毕
    gram_transmit_finish = 0x04,//显存块发送完成
    gram_not_exist = 0x05//显存不存在
    
}gram_state_type;

/* 显存块结构体 */
typedef struct
{
    uint8_t* gram_address;//显存块地址
    gram_state_type gram_state;//显存块状态
}sgl_gram_t;


/* 显存块管理结构体 */
typedef struct
{
    sgl_gram_t buf1;
    sgl_gram_t buf2;
    uint8_t num;
}sgl_gram_mange_t;

void sgl_driver_init(void);
void sgl_flushing_ready(void);

void sgl_transmit_gram_ready(void);//发送显存完成设置
void sgl_draw_gram_ready(void);//渲染显存完成设置



#endif
