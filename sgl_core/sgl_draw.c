#include "sgl_draw.h"

#define sgl_draw_swap(a, b) { uint16_t t = a; a = b; b = t; }

uint8_t* sgl_draw_gram;//绘制的显存

int16_t sgl_x_start;
int16_t sgl_y_start;

int16_t sgl_x_end;
int16_t sgl_y_end;

/* 设置当前显存的地址 */
inline void sgl_set_gram(uint8_t* gram, int16_t start_x, int16_t start_y, int16_t end_x, int16_t end_y)
{
    sgl_draw_gram = gram;//拿到新的显存块

    sgl_x_start = start_x;
    sgl_y_start = start_y;

    sgl_x_end = end_x;
    sgl_y_end = end_y;


    /* 清除新的内存块中的内容 */
    memset(sgl_draw_gram, 0, SGL_BUF_SIZE);
}

#if Color_depth==1
/* 画点函数 */
void sgl_draw_pixel(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, uint16_t color)
{
    /* 超过屏幕范围 */
    if(x > SGL_SCREEN_W - 2 || x < 0) return;

    if(y > SGL_SCREEN_H - 2 || y < 0) return;

    /* 超过父对象范围 */
    if(x > father_x + father_width - 2 || x < father_x) return;

    if(y > father_y + father_height - 2 || y < father_y) return;

    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;

    int16_t pos = i * SGL_SCREEN_W + x;

    if(color) sgl_draw_gram[pos] |= n;
    else
    {
        sgl_draw_gram[pos] = ~sgl_draw_gram[pos];
        sgl_draw_gram[pos] |= n;
        sgl_draw_gram[pos] = ~sgl_draw_gram[pos];
    }
}
#endif


#if Color_depth==16

inline void sgl_draw_pixel(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, uint16_t color)
{
    /* 超过屏幕范围 */
    if(x > SGL_SCREEN_W - 2 || x < 0) return;

    if(y > SGL_SCREEN_H - 2 || y < 0) return;

    /* 超过父对象范围 */
    if(x > father_x + father_width - 2 || x < father_x) return;

    if(y > father_y + father_height - 2 || y < father_y) return;

    /* 不在本次绘制范围内 */
    if(x < sgl_x_start || x > sgl_x_end) return;

    if(y < sgl_y_start || y > sgl_y_end) return;

    /* y要减去来自gram移动时产生的偏移 */
    uint32_t pos = (y - sgl_y_start) * SGL_SCREEN_W * 2 + x * 2;

    sgl_draw_gram[pos] = color >> 8;
    sgl_draw_gram[pos + 1] = (uint8_t)(color & 0x00ff);
}

#endif

/* 快速画水平线 */
inline void sgl_draw_fastHLine(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t length, uint16_t color)
{
    // Bounds check
    int16_t x0 = x;

    do
    {
        sgl_draw_pixel(x, y, father_x, father_y, father_width, father_height, color);
        x++;
    }
    while(x0 + length >= x);
}


/* 快速画垂直线 */
inline void sgl_draw_fastVLine(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t length, uint16_t color)
{
    int16_t y0 = y;

    do
    {
        sgl_draw_pixel(x, y, father_x, father_y, father_width, father_height, color);
        y++;
    }
    while(y0 + length >= y);
}

/* 画线函数 */
void  sgl_draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, uint8_t with, uint16_t color)
{
    int32_t   dx;
    int32_t   dy;
    int16_t   dx_sym;
    int16_t   dy_sym;
    int32_t   dx_x2;
    int32_t   dy_x2;
    int32_t   di;

    int32_t   wx, wy;
    int32_t   draw_a, draw_b;


    if(with == 0) return;

    if(with > 50) with = 50;

    dx = x1 - x0;
    dy = y1 - y0;

    wx = with / 2;
    wy = with - wx - 1;


    if(dx > 0)
    {
        dx_sym = 1;
    }
    else
    {
        if(dx < 0)
        {
            dx_sym = -1;
        }
        else
        {
            wx = x0 - wx;

            if(wx < 0) wx = 0;

            wy = x0 + wy;

            while(1)
            {
                x0 = wx;

                if(y0 > y1)
                    sgl_draw_swap(y0, y1);

                sgl_draw_fastVLine(x0, y0, father_x, father_y, father_width, father_height, y1 - y0, color);

                if(wx >= wy) break;

                wx++;
            }

            return;
        }
    }

    if(dy > 0)
    {
        dy_sym = 1;
    }
    else
    {
        if(dy < 0)
        {
            dy_sym = -1;
        }
        else
        {
            wx = y0 - wx;

            if(wx < 0) wx = 0;

            wy = y0 + wy;

            while(1)
            {
                y0 = wx;

                if(x0 > x1)
                    sgl_draw_swap(x0, x1);

                sgl_draw_fastHLine(x0, y0, father_x, father_y, father_width, father_height, x1 - x0, color);

                if(wx >= wy) break;

                wx++;
            }

            return;
        }
    }


    dx = dx_sym * dx;
    dy = dy_sym * dy;


    dx_x2 = dx * 2;
    dy_x2 = dy * 2;


    if(dx >= dy)
    {
        di = dy_x2 - dx;

        while(x0 != x1)
        {
            draw_a = y0 - wx;

            if(draw_a < 0) draw_a = 0;

            draw_b = y0 + wy;

            if(draw_a > draw_b)
                sgl_draw_swap(draw_a, draw_b);

            sgl_draw_fastVLine(x0, draw_a, father_x, father_y, father_width, father_height, draw_b - draw_a, color);

            x0 += dx_sym;

            if(di < 0)
            {
                di += dy_x2;
            }
            else
            {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }

        draw_a = y0 - wx;

        if(draw_a < 0) draw_a = 0;

        draw_b = y0 + wy;

        if(draw_a > draw_b)
            sgl_draw_swap(draw_a, draw_b);

        sgl_draw_fastVLine(x0, draw_a, father_x, father_y, father_width, father_height, draw_b - draw_a, color);
    }
    else
    {
        di = dx_x2 - dy;

        while(y0 != y1)
        {
            draw_a = x0 - wx;

            if(draw_a < 0) draw_a = 0;

            draw_b = x0 + wy;

            if(draw_a > draw_b)
                sgl_draw_swap(draw_a, draw_b);

            sgl_draw_fastHLine(draw_a, y0, father_x, father_y, father_width, father_height, draw_b - draw_a, color);

            y0 += dy_sym;

            if(di < 0)
            {
                di += dx_x2;
            }
            else
            {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }

        draw_a = x0 - wx;

        if(draw_a < 0) draw_a = 0;

        draw_b = x0 + wy;

        if(draw_a > draw_b)
            sgl_draw_swap(draw_a, draw_b);

        sgl_draw_fastHLine(draw_a, y0, father_x, father_y, father_width, father_height, draw_b - draw_a, color);
    }

}

/* 用户无需关心 */
static void sgl_draw_fillCircle_helper(int16_t x0, int16_t y0, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t r, uint8_t corner, int16_t delta, uint16_t color)
{

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        if(corner & 0x1)
        {
            sgl_draw_fastVLine(x0 + x, y0 - y, father_x, father_y, father_width, father_height, 2 * y + delta, color);
            sgl_draw_fastVLine(x0 + y, y0 - x, father_x, father_y, father_width, father_height, 2 * x + delta, color);
        }

        if(corner & 0x2)
        {
            sgl_draw_fastVLine(x0 - x, y0 - y, father_x, father_y, father_width, father_height, 2 * y + delta, color);
            sgl_draw_fastVLine(x0 - y, y0 - x, father_x, father_y, father_width, father_height, 2 * x + delta, color);
        }
    }
}

/* 画实心圆的函数 */
void sgl_draw_fill_Circle(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t r, uint16_t color)
{
    sgl_draw_fastVLine(x, y - r,  father_x, father_y, father_width, father_height, 2 * r, color);
    sgl_draw_fillCircle_helper(x, y,  father_x, father_y, father_width, father_height, r, 3, 0, color);
}

/* 绘制单个字符 */
void sgl_draw_font(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t font_height, int16_t font_width, uint8_t chr, uint8_t* font_src, uint16_t color)
{
    int x0 = x, y0 = y;
    int i, j;
    int font_size_offset = ((font_height - 1) / 8 + 1) * font_width; //计算一个字符的偏移量
    uint8_t tmp;

    if(font_height <= 8)//高度小于8
    {
        for(i = 0; i < font_width; i++)
        {
            tmp = font_src[(chr - '!') * font_size_offset + i]; //ACSII中'!'是第一个字符

            for(j = 0; j < font_height ; j++)
            {
                if(tmp & 0x01) sgl_draw_pixel(x0, y0, father_x, father_y, father_width, father_height, color);

                tmp >>= 1;
                y0++;
            }

            x0++;
            y0 = y;//回到最初行
        }
    }
    else if(font_height > 8)//大于8行
    {
        int m = 0;
        int height_tmp = 0;
        int height_byte = (font_height - 1) / 8 + 1; //获取高度所站的字节数
        int height_count = 0;

        for(i = 0; i < font_width; i++)
        {
            height_tmp = font_height;

            for(j = 0 ; j < height_byte ; j++)//每次把一个列对应的全部绘制
            {
                tmp = font_src[(chr - '!') * font_size_offset + j * font_width + i]; //获取字符的该列的字节

                if(height_tmp / 8 >= 1) height_count = 8; //遍历高度大于8则绘制8
                else height_count = font_height % 8; //不足8则绘制剩余高度

                for(m = 0; m < height_count; m++)
                {
                    if(tmp & 0x01) sgl_draw_pixel(x0, y0, father_x, father_y, father_width, father_height, color);

                    tmp >>= 1;
                    y0++;
                }

                height_tmp = height_tmp - height_count;//扫描高度减少扫描的个数个
            }

            x0++;
            y0 = y;//一个列全部扫描完毕,回到最初行
        }
    }
}


/* 绘制字符串 */
void sgl_draw_string(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t font_height, int16_t font_width, char *str, uint8_t* font_src, uint16_t color)
{
    while (*str != '\0')
    {
        sgl_draw_font(x, y, father_x, father_y, father_width, father_height, font_height, font_width, *str, font_src, color);
        x += font_width;
        str++;
    }
}

/* 绘制图片 */
void sgl_draw_bmp(int16_t x, int16_t y, int16_t father_x, int16_t father_y, int16_t father_width, int16_t father_height, int16_t bmp_height, int16_t bmp_width, uint8_t* bmp_src, uint16_t color)
{
    int x0 = x, y0 = y;
    int i, j;
    #if Color_depth == 1
    int m = 0;
    uint8_t tmp;
    int height_tmp = 0;
    int height_byte = (bmp_height - 1) / 8 + 1; //获取高度所站的字节数
    int height_count = 0;

    for(i = 0; i < bmp_width; i++)
    {
        height_tmp = bmp_height;

        for(j = 0 ; j < height_byte ; j++)//每次把一个列对应的全部绘制
        {
            tmp = bmp_src[j * bmp_width + i]; //获取该列的字节

            if(height_tmp / 8 >= 1) height_count = 8; //遍历高度大于8则绘制8
            else height_count = bmp_height % 8; //不足8则绘制剩余高度

            for(m = 0; m < height_count; m++)
            {
                if(tmp & 0x01) sgl_draw_pixel(x0, y0,  father_x, father_y, father_width, father_height, color);

                tmp >>= 1;
                y0++;
            }

            height_tmp = height_tmp - height_count;//扫描高度减少扫描的个数个
        }

        x0++;
        y0 = y;//一个列全部扫描完毕,回到最初行
    }

    #endif
    #if Color_depth == 16
    uint16_t tmp_16;
    uint32_t pos = 0;

    for(i = 0; i < bmp_height; i++)
    {
        for(j = 0 ; j < bmp_width ; j++)//每次把一个列对应的全部绘制
        {
            pos = i * bmp_width * 2 + j * 2;
            tmp_16 = ((uint16_t)bmp_src[pos]) << 8;
            tmp_16 = tmp_16 | ((uint16_t)bmp_src[pos + 1]);
            sgl_draw_pixel(x0, y0, father_x, father_y, father_width, father_height, tmp_16);
            x0++;
        }

        x0 = x;
        y0++;
    }

    #endif
}
