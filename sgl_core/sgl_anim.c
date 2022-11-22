#include "sgl_anim.h"

static sgl_anim_t sgl_anim_root;//动画根节点
static uint16_t sgl_anim_sum;//动画总数
static uint32_t sgl_anim_stamp;

static float animate_fx(float t)
{

    if(t <= 0.5f)
        return(4 * t * t * t);
    else
        return(4 * (t - 1) * (t - 1) * (t - 1) + 1);
}

/* 动画模块初始化 */
void sgl_anim_init(void)
{
    memset(&sgl_anim_root, 0, sizeof(sgl_anim_t));
    sgl_anim_root.next_anim = NULL;
    sgl_anim_root.animi_obj_callback = NULL;
    sgl_anim_root.obj = NULL;
    sgl_anim_stamp = sgl_get_Timestamp();
}

/* 动画创造函数 */
sgl_anim_t* sgl_anim_create(void* obj, float start_value, float end_value, uint32_t duration, void (*animi_obj_callback)(void*, float), float (*anim_tim_callbaclk)(float))
{
    if(obj == NULL)//绑定对象为空
    {
        #if DEBUG_ENABLE
        sgl_log("the anim have not obj-----sgl_anim_create\r\n");
        #endif
        return NULL;
    }

    if(animi_obj_callback == NULL)//作用于对象的回调函数为空
    {
        #if DEBUG_ENABLE
        sgl_log("the anim have not animi_callback-----sgl_anim_create\r\n");
        #endif
        return NULL;
    }

    if(duration == 0)//持续时间为0
    {
        #if DEBUG_ENABLE
        sgl_log("duration must not be 0,setting to 1 -----sgl_anim_create\r\n");
        #endif
        duration = 1;
    }

    sgl_anim_t* anim = (sgl_anim_t*)sig_malloc(sizeof(sgl_anim_t));//内存申请

    if(anim == NULL)
    {
        #if DEBUG_ENABLE
        sgl_log("anim malloc fail-----sgl_anim_create\r\n");
        #endif
        return NULL;
    }

    memset(anim, 0, sizeof(sgl_anim_t)); //清空内存

    /* 初始化内存值 */
    anim->obj = obj;
    anim->duration = duration;
    anim->animi_obj_callback = animi_obj_callback;

    if(anim_tim_callbaclk == NULL)//如果时间回调函数为空
    {
        #if DEBUG_ENABLE
        sgl_log("the anim_tim_callback is not exist,use sgl inside anim_tim_callback-----sgl_anim_create\r\n");
        #endif
        anim->anim_tim_callbaclk = animate_fx;
    }
    else anim->anim_tim_callbaclk = anim_tim_callbaclk;

    anim->start_value = start_value;
    anim->now_value = start_value;
    anim->end_value = end_value;
    anim->next_anim = NULL;
    anim->distance = end_value - start_value;


    sgl_anim_t* p_anim = &sgl_anim_root;

    while(p_anim->next_anim != NULL) p_anim = (sgl_anim_t*)p_anim->next_anim;

    p_anim->next_anim = (struct sgl_anim_t*)anim;

    sgl_anim_sum++;
    return anim;
}

void sgl_anim_remove(sgl_anim_t* anim)
{
    if(anim == NULL)
    {
        #if DEBUG_ENABLE
        sgl_log("the anim is not exist-----sgl_anim_remove\r\n");
        #endif
        return;
    }

    sgl_anim_t* p_anim = (sgl_anim_t*)sgl_anim_root.next_anim;
    sgl_anim_t* p_anim_pre = &sgl_anim_root;

    while(p_anim != NULL)//遍历动画链表
    {
        if(p_anim == anim)
        {
            p_anim_pre->next_anim = p_anim->next_anim;
            sig_free(p_anim);//释放内存
            sgl_anim_sum--;
            return;//退出函数
        }

        p_anim_pre = p_anim;
        p_anim = (sgl_anim_t*)p_anim->next_anim;
    }
}

/* 动画函数时间戳 */
static uint32_t sgl_anim_tick_stampe_interval(uint32_t pre_tick)
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

void sgl_anim_run(void)
{
    sgl_anim_t* p_anim = (sgl_anim_t*)sgl_anim_root.next_anim;
    sgl_anim_t* p_anim_tmp = NULL;


    uint32_t add_time = sgl_anim_tick_stampe_interval(sgl_anim_stamp);//获取与上个时间戳的间隔
    sgl_anim_stamp = sgl_get_Timestamp();//更新动画时间戳
    float p = 0;

    while(p_anim != NULL)
    {
        p_anim->act_time += add_time;

        if(p_anim->act_time > p_anim->duration)
        {
            p_anim->act_time = p_anim->duration;
        }

        p = p_anim->anim_tim_callbaclk((float)p_anim->act_time / p_anim->duration);//时间回调函数

        p_anim->now_value = p_anim->start_value + p * p_anim->distance; //计算

        p_anim->animi_obj_callback(p_anim->obj, p_anim->now_value); //将值传递给对象


        if(p_anim->act_time == p_anim->duration)//持续时间到了,移除动画
        {
            p_anim_tmp = (sgl_anim_t*)p_anim->next_anim;
            sgl_anim_remove(p_anim);
            p_anim = p_anim_tmp;
        }
        else
            p_anim = (sgl_anim_t*)p_anim->next_anim;
    }

    #if DEBUG_ENABLE
    sgl_log("sgl_anim_sum :%d-----sgl_anim_handle\r\n", sgl_anim_sum);
    #endif
}

