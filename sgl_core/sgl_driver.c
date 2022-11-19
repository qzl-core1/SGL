#include "sgl_driver.h"

uint8_t buf1[SGL_SCREEN_SIZE];
volatile static sgl_gram_mange_t sgl_gram_mange;
volatile static sgl_gram_t* sgl_drawing_gram;//渲染显存
volatile static sgl_gram_t* sgl_transmit_gram;//发送显存



static void sgl_screen_init(void)//屏幕设备初始化
{
    OLED_Init();
}

static void sgl_gram_init(uint8_t* buf_one,uint8_t* buf_two)
{
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
    sgl_set_gram(sgl_drawing_gram->gram_address);
}


void sgl_transmit_gram_ready(void)//发送显存完成设置
{
    sgl_transmit_gram->gram_state = gram_transmit_finish;
}

void sgl_draw_gram_ready(void)//渲染显存完成设置
{
    sgl_drawing_gram->gram_state = gram_draw_finish;
}

void sgl_screen_flushing(uint8_t* buf)
{
    /* 这里写入你的刷新屏幕的接口 */
//    ssd1306_flushing_dma(buf,SGL_SCREEN_H*SGL_SCREEN_W/8);
    ssd1306_flushing(buf);
    /* 这里写入你的刷新屏幕的接口 */
    sgl_transmit_gram_ready();//如果是双显存模式,请将这个函数放在DMA里面调用,然后注释此行
}

/* sgl_handle末尾调用 */
void sgl_flushing_ready(void)
{
    if(sgl_gram_mange.num == 1)//一个显存块
    {
        sgl_screen_flushing(sgl_transmit_gram->gram_address);
        sgl_set_gram(sgl_drawing_gram->gram_address);
        /* 单个显存就无需关心显存状态了 */
    }
    else if(sgl_gram_mange.num == 2)//双显存模式
    {
        if(sgl_gram_mange.buf1.gram_state == gram_draw_finish)//显存1完成渲染
        {
            while(sgl_gram_mange.buf2.gram_state != gram_transmit_finish);//等待显存2完成传输(DMA中断里面调用sgl_transmit_gram_ready())
            
            sgl_drawing_gram = &sgl_gram_mange.buf2;//将显存2设置为渲染显存
            sgl_drawing_gram->gram_state = gram_in_draw;//设置渲染显存的状态为正在渲染
            sgl_set_gram(sgl_drawing_gram->gram_address);//设置为渲染显存的地址,传递给sgl_draw模块
            
            
            sgl_transmit_gram = &sgl_gram_mange.buf1;//设置显存1为发送显存
            sgl_transmit_gram->gram_state = gram_in_transmit;//设置发送显存的发送状态
            sgl_screen_flushing(sgl_transmit_gram->gram_address);//将发送显存开始发送
        }
        else if(sgl_gram_mange.buf2.gram_state == gram_draw_finish)//显存2完成渲染
        {
            while(sgl_gram_mange.buf1.gram_state != gram_transmit_finish);//等待显存1发送完毕(DMA中断里面调用sgl_transmit_gram_ready())
            
            sgl_drawing_gram = &sgl_gram_mange.buf1;//将显存1设置为渲染显存
            sgl_drawing_gram->gram_state = gram_in_draw;//将渲染显存状态设置正在渲染
            sgl_set_gram(sgl_drawing_gram->gram_address);//设置为渲染显存的地址,传递给sgl_draw模块
            
            sgl_transmit_gram = &sgl_gram_mange.buf2;//设置显存2发送显存
            sgl_transmit_gram->gram_state = gram_in_transmit;//将发送显存的状态设置为开始发送
            sgl_screen_flushing(sgl_transmit_gram->gram_address);//将发送显存发送
        }
    }
}

void sgl_driver_init(void)
{
    /* 屏幕初始化 */
    sgl_screen_init();
    
    /* 显存初始化 */
    sgl_gram_init(buf1,NULL);
    
}

void DMA1_Channel3_IRQHandler(void)
{
   if(dma_flag_get(DMA1_FDT3_FLAG))
  {
    dma_flag_clear(DMA1_FDT3_FLAG);
    sgl_transmit_gram_ready();
  }
}
