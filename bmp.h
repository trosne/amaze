#ifndef _BMP_H__
#define _BMP_H__

#include <stdint.h>


void put_pixel(uint8_t* rgb, uint8_t col, uint32_t* index);

void put_pixel_color(uint8_t* rgb, uint8_t red, uint8_t green, uint8_t blue, uint32_t* index);

int write_bmp(const char *filename, uint16_t width, uint16_t height, uint8_t *rgb);

#endif /* _BMP_H__ */
