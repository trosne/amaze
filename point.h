#ifndef _POINT_H__
#define _POINT_H__

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MIN(x, y) ((x < y) ? x : y)
#define MAX(x, y) ((x > y) ? x : y)

#define ASSERT(x) if ((x) == false) { printf("ERROR: LINE %i, FILE %s\n", __LINE__, __FILE__); exit(1); }

typedef struct
{
  uint16_t x, y;
} point_t;

typedef struct
{
  uint16_t size, capacity;
  point_t* points;
} point_list_t;

void point_list_set_size(point_list_t* p_list, uint16_t size);

void point_list_init(point_list_t* p_list);

void point_list_add(point_list_t* p_list, point_t point);

void point_list_rm(point_list_t* p_list, uint16_t index);

point_t point_list_get(point_list_t* p_list, uint16_t index);




#endif /* _POINT_H__ */
