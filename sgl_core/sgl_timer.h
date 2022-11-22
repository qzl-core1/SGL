#ifndef __SGL_TIMER_H
#define __SGL_TIMER_H
#include "sgl.h"

typedef struct
{
    uint16_t act_time;//已经存在的时间
    uint16_t interval_time;//间隔时间
    void (*timer_callback)();//定时回调
    struct sgl_timer_t* next_timer;
} sgl_timer_t;

void sgl_timer_init(void);
sgl_timer_t* sgl_timer_create(uint16_t interval_time, void (*timer_callback)());
void sgl_timer_remove(sgl_timer_t* timer);
void sgl_timer_run(void);

int sgl_timer_get_sum(void);

#endif
