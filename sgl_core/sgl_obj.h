#ifndef __SGL_OBJ_H
#define __SGL_OBJ_H
#include "sgl.h"

typedef struct
{
    int16_t x;//一个对象的坐标x
    int16_t y;//一个对象的坐标y
    int16_t height;//对象的高度
    int16_t width;//对象的宽度
    struct sgl_obj_t* f_obj;//父对象
    void(*draw_method)(void* p);//对象的绘制方法
    void *obj_features;//对象属性的特征
    struct sgl_obj_t* next_obj;//下个节点的对象
}sgl_obj_t;

void sgl_obj_init(void);
sgl_obj_t* sgl_obj_create(sgl_obj_t* f_obj,void(*draw_method)(void*));
void sgl_obj_remove(sgl_obj_t* obj);
void sgl_obj_handle(void);


void sgl_obj_set_size(sgl_obj_t* obj,int16_t height,int16_t width);
void sgl_obj_set_pos(sgl_obj_t* obj,int16_t x,int16_t y);
void sgl_obj_set_features(sgl_obj_t* obj,void* features);


int sgl_obj_get_sum(void);

#endif
