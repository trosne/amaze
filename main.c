#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MIN(x, y) ((x < y) ? x : y)
#define MAX(x, y) ((x > y) ? x : y)

#define CH_BLOCK 219

#define ASSERT(x) if ((x) == false) { printf("ERROR: LINE %i, FILE %s\n", __LINE__, __FILE__); exit(1); }

typedef struct __attribute__((__packed__))
{
  uint16_t x, y;
  uint8_t value;
} cell_t;

typedef struct
{
  uint16_t width, height;
  cell_t *cells;
} maze_t;

typedef enum
{
  DIR_UP,
  DIR_DOWN,
  DIR_LEFT,
  DIR_RIGHT
} dir_t;

typedef struct
{
  uint16_t x, y;
} point_t;

void print_maze(maze_t* p_maze)
{
  system("clear");
  printf("MAZE: w: %i, h: %i\n", p_maze->width, p_maze->height);

  
  for (uint8_t x = 0; x < p_maze->width; ++x)
  {
    cell_t* c = &p_maze->cells[x + p_maze->height * x];
    printf("OO");
  }
  printf("O\n");

  for (uint8_t y = 0; y < p_maze->height; ++y)
  {
    printf("O");
    /* print actual rows */
    for (uint8_t x = 0; x < p_maze->height; ++x)
    {
      cell_t* c = &p_maze->cells[x + p_maze->width * y];
      if (c->value != 0)
      {
        printf((c->x != 0) ? "  " : " O");
      }
      else
      {
        printf("OO");
      }
    }
    printf("\nO");
    /* print barrier */
    for (uint8_t x = 0; x < p_maze->height; ++x)
    {
      cell_t* c = &p_maze->cells[x + p_maze->width * y];
      printf((c->y == 0) ? "OO" : " O");
    }
    printf("\n");

  }

}

void connect_points(maze_t* p_maze, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  ASSERT(x1 < p_maze->width);
  ASSERT(y1 < p_maze->height);
  ASSERT(x2 < p_maze->width);
  ASSERT(y2 < p_maze->height);
  ASSERT(x1 == x2 || y1 == y2);

  uint16_t x_min = MIN(x1, x2);
  uint16_t y_min = MIN(y1, y2);

  uint16_t x_max = MAX(x1, x2);
  uint16_t y_max = MAX(y1, y2);

  if (x1 == x2)
  {
    for (uint16_t y = y_min; y < y_max; ++y)
    {
      cell_t* p_cell = &p_maze->cells[x1 + p_maze->width * y];
      p_cell->value = 1;
      p_cell->y = 1;
    }
    p_maze->cells[x1 + p_maze->width * y_max].value = 1;
  }
  else
  {
    for (uint16_t x = x_min; x < x_max; ++x)
    {
      cell_t* p_cell = &p_maze->cells[x + p_maze->width * y1];
      p_cell->value = 1;
      p_cell->x = 1;
    }
    p_maze->cells[x_max + p_maze->width * y1].value = 1;
  }

}

void generate_maze(maze_t* p_maze)
{

}



int main(int args, char **argv)
{
  if (args < 2)
  {
    printf("Too few arguments. \nUsage: amaze <width> <height>\n\n");
    return 64;
  }

  maze_t maze;
  maze.width = atoi(argv[1]);
  maze.height = atoi(argv[2]);
  maze.cells = (cell_t*) malloc(sizeof(cell_t) * maze.width * maze.height);
#if 0  
  for (uint8_t x = 0; x < maze.width; ++x)
  {
    for (uint8_t y = 0; y < maze.height; ++y)
    {
      maze.cells[y + maze.height * x].x = x;
      maze.cells[y + maze.height * x].y = y;
    }
  }
#endif
  generate_maze(&maze);

  print_maze(&maze);
  
  connect_points(&maze, 0, 3, 0, 0);
  sleep(1);
  print_maze(&maze);
  sleep(1);
  connect_points(&maze, 0, 0, 3, 0);

  print_maze(&maze);

  return 0;
}
