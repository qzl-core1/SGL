#ifndef __SGL_DRAW_H
#define __SGL_DRAW_H
#include "sgl.h"

void sgl_set_gram(uint8_t* gram);
void sgl_draw_pixel(int16_t x, int16_t y, uint8_t mode);
void sgl_draw_fastHLine(int16_t x, int16_t y, int16_t length);
void sgl_draw_fastVLine(int16_t x, int16_t y, int16_t length);
void  sgl_draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t with);
void sgl_draw_fill_Circle(int16_t x, int16_t y, int16_t r);
void sgl_draw_string(int16_t x, int16_t y, int16_t font_height,int16_t font_width , char *str, uint8_t* font_src);
void sgl_draw_bmp(int16_t x, int16_t y, int16_t bmp_height,int16_t bmp_width , uint8_t* bmp_src);

#endif
