#include "point.h"

#include <string.h>
#include <stdlib.h>

void point_list_set_size(point_list_t* p_list, uint16_t size)
{
  if (p_list->capacity == 0)
  {
    p_list->points = (point_t*) malloc(sizeof(point_t) * size);
  }
  else
  {
    p_list->points = (point_t*) realloc(p_list->points, sizeof(point_t) * size);
  }
  p_list->capacity = size;
  p_list->size = MIN(size, p_list->size);

}

void point_list_init(point_list_t* p_list)
{
  p_list->size = 0; 
  p_list->capacity = 0;
  point_list_set_size(p_list, 5);
}

void point_list_add(point_list_t* p_list, point_t point)
{
  if (p_list->capacity == p_list->size)
  {
    point_list_set_size(p_list, (int)(p_list->size * 1.5f) + 5);
  }
  memcpy(&p_list->points[p_list->size++], &point, sizeof(point_t));
}

void point_list_rm(point_list_t* p_list, uint16_t index)
{
  ASSERT(p_list->size > index)
  p_list->points[index] = p_list->points[--p_list->size];
}

point_t* point_list_get(point_list_t* p_list, uint16_t index)
{
  ASSERT(p_list->size > index);
  return &p_list->points[index];
}


point_t* point_list_get_deepest(point_list_t* p_list)
{
  point_t* deepest = &p_list->points[0];

  for (uint16_t i = 0; i < p_list->size; ++i)
  {
    if (p_list->points[i].depth > deepest->depth)
    {
      deepest = &p_list->points[i];
    }
  }
  return deepest;
}

point_t* point_list_get_shallowest(point_list_t* p_list)
{
  point_t* deepest = &p_list->points[0];

  for (uint16_t i = 0; i < p_list->size; ++i)
  {
    if (p_list->points[i].depth < deepest->depth)
    {
      deepest = &p_list->points[i];
    }
  }
  return deepest;
}

