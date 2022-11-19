#ifndef __SGL_ANIM_H
#define __SGL_ANIM_H
#include "sgl.h"

typedef struct
{
    float start_value;//开始值
    float now_value;//当前值
    float end_value;//最终值
    float distance;//差值
    void* obj;//绑定对象
    void (*animi_obj_callback)(void*,float);//对象回调
    float (*anim_tim_callbaclk)(float);//时间回调
    uint16_t act_time;//已经存在的时间
    uint16_t duration;//持续时间
    struct sgl_anim_t* next_anim;
}sgl_anim_t;

void sgl_anim_init(void);
sgl_anim_t* sgl_anim_create(void* obj,float start_value,float end_value,uint32_t duration,void (*animi_obj_callback)(void*,float),float (*anim_tim_callbaclk)(float));
void sgl_anim_handle(void);

#endif
