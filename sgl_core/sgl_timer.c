#include "sgl_timer.h"

static uint32_t sgl_timer_stamp;//sgl模块中定时器时间戳
static sgl_timer_t sgl_timer_root;//sgl定时器模块的根节点
static int16_t sgl_timer_sum;//存在的定时器数量

/* 动画函数时间戳 */
static uint32_t sgl_timer_tick_stampe_interval(uint32_t pre_tick)
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

void sgl_timer_init(void)
{
    sgl_timer_root.act_time = 0;
    sgl_timer_root.interval_time = 0;
    sgl_timer_root.timer_callback = NULL;
    sgl_timer_root.next_timer = NULL;
    sgl_timer_stamp = sgl_get_Timestamp();
}

sgl_timer_t* sgl_timer_create(uint16_t interval_time, void (*timer_callback)())
{
    if(interval_time == 0)//间隔触发时间为0
    {
        return NULL;
    }

    sgl_timer_t* timer = (sgl_timer_t*)sgl_malloc(sizeof(sgl_timer_t));

    if(timer == NULL)//定时器内存申请失败
    {
        #if DEBUG_ENABLE
        sgl_log("timer malloc fail-----sgl_timer_create\r\n");
        #endif
        return NULL;
    }

    memset(timer, 0, sizeof(sgl_timer_t)); //初始化定时器
    timer->act_time = 0;
    timer->interval_time = interval_time;
    timer->timer_callback = timer_callback;


    /* 遍历插入 */
    sgl_timer_t* ptr = (sgl_timer_t*)&sgl_timer_root;

    while(ptr->next_timer != NULL)
    {
        ptr = (sgl_timer_t*)ptr->next_timer;
    }

    ptr->next_timer = (struct sgl_timer_t*)timer;
    sgl_timer_sum++;//定时器总数+1

    return timer;
}

void sgl_timer_remove(sgl_timer_t* timer)
{
    if(timer == NULL)//空指针
    {
        return;
    }

    sgl_timer_t* ptr_pre = (sgl_timer_t*)&sgl_timer_root;
    sgl_timer_t* ptr = (sgl_timer_t*)sgl_timer_root.next_timer;

    while(ptr != NULL)
    {
        if(ptr == timer)
        {
            ptr_pre->next_timer = ptr->next_timer;
            sgl_free(ptr);
            sgl_timer_sum--;//定时器总数-1
            return;
        }

        ptr_pre = ptr;
        ptr = (sgl_timer_t*)ptr->next_timer;
    }
}

/* 返回当前定时器的数量 */
int sgl_timer_get_sum(void)
{
    return sgl_timer_sum;
}

void sgl_timer_run(void)
{
    uint32_t add_time = sgl_timer_tick_stampe_interval(sgl_timer_stamp);
    sgl_timer_stamp = sgl_get_Timestamp();//更新时间戳

    sgl_timer_t* ptr = (sgl_timer_t*)sgl_timer_root.next_timer;

    while(ptr != NULL)
    {
        sgl_timer_t* ptr_next = (sgl_timer_t*)ptr->next_timer;//下个定时器
        ptr->act_time += add_time;

        if(ptr->act_time > ptr->interval_time || ptr->act_time == ptr->interval_time)//定时器时间到
        {
            ptr->act_time = 0;

            if(ptr->timer_callback != NULL) ptr->timer_callback();

            if(ptr == NULL)//假设用户在回调中将定时器释放了
            {
                ptr = ptr_next;//将这个定时器的下个定时器付给当前的遍历节点
                continue;
            }
        }

        ptr = (sgl_timer_t*)ptr->next_timer;
    }
}

