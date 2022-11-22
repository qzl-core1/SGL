#include "sgl_obj.h"

static sgl_obj_t sgl_obj_root;//头节点对象,即屏幕本身
static int sgl_obj_sum;//对象总数,不计入头节点

/* 对象链表初始化 */
void sgl_obj_init(void)
{
    sgl_obj_root.x = 0;
    sgl_obj_root.y = 0;
    sgl_obj_root.height = SGL_SCREEN_H;
    sgl_obj_root.width = SGL_SCREEN_W;
    sgl_obj_root.draw_method = NULL;
    sgl_obj_root.f_obj = NULL;
    sgl_obj_root.obj_features = NULL;
    sgl_obj_root.next_obj = NULL;
}

/* 创造对象 */
sgl_obj_t* sgl_obj_create(sgl_obj_t* f_obj, void(*draw_method)(void*))
{
    sgl_obj_t* p_obj = (sgl_obj_t*)sig_malloc(sizeof(sgl_obj_t));
    sgl_obj_t* ptr = (sgl_obj_t*)&sgl_obj_root;

    if(p_obj == NULL)//内存申请失败
    {
        /* 加入DEBUG函数 */
        #if DEBUG_ENABLE
        sgl_log("obj malloc fail-----sgl_obj_create\r\n");
        #endif
        return NULL;
    }

    memset(p_obj, 0, sizeof(sgl_obj_t));

    if(f_obj == NULL)//父对象为空,默认使用屏幕当作父对象
    {
        p_obj->f_obj = (struct sgl_obj_t*)&sgl_obj_root;
    }
    else p_obj->f_obj = (struct sgl_obj_t *)f_obj;

    //对对象属性进行初始化
    p_obj->x = 0;
    p_obj->y = 0;
    p_obj->draw_method = draw_method;
    p_obj->obj_features = NULL;
    p_obj->next_obj = NULL;
    p_obj->height = 0;
    p_obj->width = 0;

    /* 遍历节点,插入对象 */
    while(ptr->next_obj != NULL) ptr = (sgl_obj_t *) ptr->next_obj;

    ptr->next_obj = (struct sgl_obj_t *) p_obj;

    /* 总对象数+1 */
    sgl_obj_sum++;

    return p_obj;
}

/* 移除obj对象,从内存中释放 */
void sgl_obj_remove(sgl_obj_t* obj)
{
    if(obj == NULL)
    {
        /* 加入DEBUG函数 */
        #if DEBUG_ENABLE
        sgl_log("obj is not exist-----sgl_obj_remove\r\n");
        #endif
        return;
    }


    sgl_obj_t *p = (sgl_obj_t*)sgl_obj_root.next_obj;//当前遍历的对象
    sgl_obj_t *pre = (sgl_obj_t*)&sgl_obj_root;//前一个节点的对象

    while(p != NULL)//遍历对象链表
    {
        if(p == obj)//如果是当前的对象,则直接释放
        {
            pre->next_obj = p->next_obj;//改变前一个节点的指针,指向下一个节点
            sig_free(p);//释放内存
            sgl_obj_sum--;//总对象减少1个
            return;//退出函数
        }

        pre = p;
        p = (sgl_obj_t *)p->next_obj;
    }
}

/* 对对象的长宽进行设置 */
void sgl_obj_set_size(sgl_obj_t* obj, int16_t height, int16_t width)
{
    if(obj == NULL)
    {
        /* 加入DEBUG函数 */
        #if DEBUG_ENABLE
        sgl_log("obj is not exist-----sgl_obj_set_size\r\n");
        #endif
        return;
    }

    obj->height = height;
    obj->width = width;
}

/* 对对象的坐标进行设置 */
void sgl_obj_set_pos(sgl_obj_t* obj, int16_t x, int16_t y)
{
    if(obj == NULL)
    {
        /* 加入DEBUG函数 */
        #if DEBUG_ENABLE
        sgl_log("obj is not exist-----sgl_obj_set_pos\r\n");
        #endif
        return;
    }

    obj->x = x;
    obj->y = y;
}

/* 对对象的特征进行设置 */
void sgl_obj_set_features(sgl_obj_t* obj, void* features)
{
    if(obj == NULL)
    {
        /* 加入DEBUG函数 */
        #if DEBUG_ENABLE
        sgl_log("obj is not exist-----sgl_obj_set_features\r\n");
        #endif
        return;
    }

    obj->obj_features = features;
}

/* 获取此刻对象链表中的总对象数量 */
int sgl_obj_get_sum(void)
{
    return sgl_obj_sum;
}


/* 对象链表遍历设置 */
void sgl_obj_run(void)
{
    sgl_obj_t *p = (sgl_obj_t *)sgl_obj_root.next_obj;

    while(p != NULL)
    {
        if(p->draw_method != NULL)//如果绘制方法不为空,则使用绘制方法
        {
            p->draw_method(p);
        }

        p = (sgl_obj_t *)p->next_obj;
    }

    #if DEBUG_ENABLE
    sgl_log("obj sum :%d-----sgl_obj_handle\r\n", sgl_obj_sum);
    #endif
}
