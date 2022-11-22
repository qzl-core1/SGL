#include "sgl_driver.h"

uint8_t buf1[SGL_BUF_SIZE];
uint8_t buf2[SGL_BUF_SIZE];
volatile static sgl_gram_mange_t sgl_gram_mange;//屏幕显存管理
volatile static sgl_gram_t* sgl_drawing_gram;//渲染显存
volatile static sgl_gram_t* sgl_transmit_gram;//发送显存



static void sgl_screen_init(void)//屏幕设备初始化
{
    LCD_Init();
}

static void sgl_gram_init(uint8_t* buf_one, uint8_t* buf_two)
{
    sgl_gram_mange.sgl_screen_start_x = 0;
    sgl_gram_mange.sgl_screen_start_y = 0;

    sgl_gram_mange.sgl_screen_end_x = SGL_SCREEN_W - 1;
    sgl_gram_mange.sgl_screen_end_y = SGL_LOACL_BUF_RATING - 1;


    sgl_gram_mange.buf1.gram_address = buf_one;
    sgl_gram_mange.buf1.gram_state = gram_draw_finish;
    sgl_gram_mange.num++;

    if(buf_two == NULL)//只使用一个显存块
    {
        sgl_transmit_gram = &sgl_gram_mange.buf1;
        sgl_drawing_gram = &sgl_gram_mange.buf1;
        sgl_gram_mange.buf2.gram_address = NULL;
        sgl_gram_mange.buf2.gram_state = gram_not_exist;
    }
    else
    {
        sgl_transmit_gram = &sgl_gram_mange.buf2;//显存2为发送显存
        sgl_drawing_gram = &sgl_gram_mange.buf1;//显存1为渲染显存
        sgl_gram_mange.buf2.gram_address = buf_two;
        sgl_gram_mange.buf2.gram_state = gram_transmit_finish;
        sgl_gram_mange.num++;
    }

    sgl_set_gram(sgl_drawing_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, sgl_gram_mange.sgl_screen_start_y, sgl_gram_mange.sgl_screen_end_x, sgl_gram_mange.sgl_screen_end_y);
}


inline void sgl_transmit_gram_ready(void)//发送显存完成设置
{
    sgl_transmit_gram->gram_state = gram_transmit_finish;
}

inline void sgl_draw_gram_ready(void)//渲染显存完成设置
{
    sgl_drawing_gram->gram_state = gram_draw_finish;
}

void sgl_screen_flushing(uint8_t* buf, int16_t sgl_screen_start_x, int16_t sgl_screen_start_y, int16_t sgl_screen_end_x, int16_t sgl_screen_end_y)
{
    /* 这里写入你的刷新屏幕的接口 */
//    st7789_flushing(buf, sgl_screen_start_x, sgl_screen_start_y, sgl_screen_end_x, sgl_screen_end_y);
    st7789_flushing_DMA(buf, sgl_screen_start_x, sgl_screen_start_y, sgl_screen_end_x, sgl_screen_end_y);
    /* 这里写入你的刷新屏幕的接口 */
//    sgl_transmit_gram_ready();//如果是双显存模式,请将这个函数放在DMA里面调用,然后注释此行
}







/* sgl_run末尾调用 */
void sgl_flushing_ready(void)
{
    int tmp_y_start = sgl_gram_mange.sgl_screen_start_y;
    int tmp_y_end = sgl_gram_mange.sgl_screen_end_y;

    if(sgl_gram_mange.num == 1)//一个显存块
    {
        sgl_gram_mange.sgl_screen_start_y += SGL_LOACL_BUF_RATING;//渲染起始向下偏移
        sgl_gram_mange.sgl_screen_end_y += SGL_LOACL_BUF_RATING;//渲染其实向下偏移

        if(sgl_gram_mange.sgl_screen_start_y >= SGL_SCREEN_H)//到底了,回到顶部
        {
            sgl_gram_mange.sgl_screen_start_y = 0;
            sgl_gram_mange.sgl_screen_end_y = SGL_LOACL_BUF_RATING - 1;
        }

        if(sgl_gram_mange.sgl_screen_end_y > SGL_SCREEN_H - 1)
        {
            sgl_gram_mange.sgl_screen_end_y = SGL_SCREEN_H - 1;
        }

        sgl_screen_flushing(sgl_transmit_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, tmp_y_start, sgl_gram_mange.sgl_screen_end_x, tmp_y_end);
        sgl_set_gram(sgl_drawing_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, sgl_gram_mange.sgl_screen_start_y, sgl_gram_mange.sgl_screen_end_x, sgl_gram_mange.sgl_screen_end_y);    //设置为渲染显存的地址,传递给sgl_draw模块
    }
    else if(sgl_gram_mange.num == 2)//双显存模式
    {
        sgl_gram_mange.sgl_screen_start_y += SGL_LOACL_BUF_RATING;//渲染起始向下偏移
        sgl_gram_mange.sgl_screen_end_y += SGL_LOACL_BUF_RATING;//渲染其实向下偏移

        if(sgl_gram_mange.sgl_screen_start_y >= SGL_SCREEN_H)//到底了,回到顶部
        {
            sgl_gram_mange.sgl_screen_start_y = 0;
            sgl_gram_mange.sgl_screen_end_y = SGL_LOACL_BUF_RATING - 1;
        }

        if(sgl_gram_mange.sgl_screen_end_y > SGL_SCREEN_H - 1)
        {
            sgl_gram_mange.sgl_screen_end_y = SGL_SCREEN_H - 1;
        }


        if(sgl_gram_mange.buf1.gram_state == gram_draw_finish)//显存1完成渲染
        {
            while(sgl_gram_mange.buf2.gram_state != gram_transmit_finish);//等待显存2完成传输(DMA中断里面调用sgl_transmit_gram_ready())

            sgl_drawing_gram = &sgl_gram_mange.buf2;//将显存2设置为渲染显存
            sgl_drawing_gram->gram_state = gram_in_draw;//设置渲染显存的状态为正在渲染
            sgl_set_gram(sgl_drawing_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, sgl_gram_mange.sgl_screen_start_y, sgl_gram_mange.sgl_screen_end_x, sgl_gram_mange.sgl_screen_end_y);    //设置为渲染显存的地址,传递给sgl_draw模块


            sgl_transmit_gram = &sgl_gram_mange.buf1;//设置显存1为发送显存
            sgl_transmit_gram->gram_state = gram_in_transmit;//设置发送显存的发送状态
            sgl_screen_flushing(sgl_transmit_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, tmp_y_start, sgl_gram_mange.sgl_screen_end_x, tmp_y_end);    //将发送显存开始发送
        }
        else if(sgl_gram_mange.buf2.gram_state == gram_draw_finish)//显存2完成渲染
        {
            while(sgl_gram_mange.buf1.gram_state != gram_transmit_finish);//等待显存1发送完毕(DMA中断里面调用sgl_transmit_gram_ready())

            sgl_drawing_gram = &sgl_gram_mange.buf1;//将显存1设置为渲染显存
            sgl_drawing_gram->gram_state = gram_in_draw;//将渲染显存状态设置正在渲染
            sgl_set_gram(sgl_drawing_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, sgl_gram_mange.sgl_screen_start_y, sgl_gram_mange.sgl_screen_end_x, sgl_gram_mange.sgl_screen_end_y);    //设置为渲染显存的地址,传递给sgl_draw模块

            sgl_transmit_gram = &sgl_gram_mange.buf2;//设置显存2发送显存
            sgl_transmit_gram->gram_state = gram_in_transmit;//将发送显存的状态设置为开始发送
            sgl_screen_flushing(sgl_transmit_gram->gram_address, sgl_gram_mange.sgl_screen_start_x, tmp_y_start, sgl_gram_mange.sgl_screen_end_x, tmp_y_end);    //将发送显存开始发送
        }
    }
}


void DMA1_Channel3_IRQHandler(void)
{
    if(dma_flag_get(DMA1_FDT3_FLAG))
    {
        dma_flag_clear(DMA1_FDT3_FLAG);
        sgl_transmit_gram_ready();
    }
}



void sgl_driver_init(void)
{
    /* 屏幕初始化 */
    sgl_screen_init();

    /* 显存初始化 */
    sgl_gram_init(buf1, buf2);
//     sgl_gram_init(buf1, NULL);

}
