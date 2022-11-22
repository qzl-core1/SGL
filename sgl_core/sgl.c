#include "sgl.h"

static uint32_t sgl_timestampe;//当前的时间戳
static uint32_t sgl_timestampe_diff;//和上次时间戳的差值

/* 上次时间戳和这次时间戳的差值 */
uint32_t sgl_tick_stampe_interval(uint32_t pre_tick)
{
    uint32_t tick = sgl_get_Timestamp();

    if(tick >= pre_tick)
    {
        pre_tick = tick - pre_tick;
    }
    else
    {
        pre_tick = 0xffffffff - pre_tick + 1;
        pre_tick += tick;
        pre_tick = ~pre_tick;
    }

    return pre_tick;
}

/* sgl模块初始化 */
void sgl_init(void)
{
    sgl_driver_init();
    sgl_obj_init();
    sgl_anim_init();
    sgl_timestampe = sgl_get_Timestamp();
}

/* sgl循环句柄 */
void sgl_run(void)
{
    /* 获取时间差 */
    sgl_timestampe_diff = sgl_tick_stampe_interval(sgl_timestampe);

    if(sgl_timestampe_diff > 1)
    {
        /* 更换时间戳 */
        sgl_timestampe = sgl_get_Timestamp();

        /* 动画链表 */
        sgl_anim_run();

        /* 对象链表 */
        sgl_obj_run();

        /* 渲染完成设置 */
        sgl_draw_gram_ready();

        /* 显存切换 */
        sgl_flushing_ready();
    }
}
