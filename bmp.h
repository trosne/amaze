#ifndef _BMP_H__
#define _BMP_H__

void put_pixel(char* rgb, char col, int* index);

int write_bmp(const char *filename, int width, int height, char *rgb);

#endif /* _BMP_H__ */
