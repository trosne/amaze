#include "bmp.h"
#include "point.h"
#include "time.h"

#define WAIT_AND_PRINT (0)

#define STEP_WAIT (0)

#define COL_BLOCKED 0xFF

#define COL_AVAILABLE 0x00

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



static cell_t* get_cell(maze_t* p_maze, uint16_t x, uint16_t y)
{
  return &(p_maze->cells[x + p_maze->width * y]);
}

static void store_maze(const char* bmp_file, maze_t* p_maze)
{
  char rgb[(2 * p_maze->height + 1) * (2 * p_maze->width + 1) * 3];
  memset(rgb, 0, sizeof(rgb));

  uint32_t i = 0;

  for (uint8_t x = 0; x < p_maze->width; ++x)
  {
    //printf("OO");
    put_pixel(rgb, COL_BLOCKED, &i);
    put_pixel(rgb, COL_BLOCKED, &i);
  }
  //printf("O\n");
  put_pixel(rgb, COL_BLOCKED, &i);
  for (uint8_t y = 0; y < p_maze->height; ++y)
  {
    //printf("O");

    put_pixel(rgb, COL_BLOCKED, &i);
    /* print actual rows */
    for (uint8_t x = 0; x < p_maze->width; ++x)
    {
      cell_t* c = get_cell(p_maze, x, y);
      if (c->value != 0)
      {
        put_pixel(rgb, COL_AVAILABLE, &i);
        put_pixel(rgb, (c->x != 0) ? COL_AVAILABLE : COL_BLOCKED, &i);
      }
      else
      {
        put_pixel(rgb, COL_BLOCKED, &i);
        put_pixel(rgb, COL_BLOCKED, &i);
      }
    }
    put_pixel(rgb, COL_BLOCKED, &i);
    /* print barrier */
    for (uint8_t x = 0; x < p_maze->width; ++x)
    {
      cell_t* c = get_cell(p_maze, x, y);
      put_pixel(rgb, (c->y == 0) ? COL_BLOCKED : COL_AVAILABLE, &i);
      put_pixel(rgb, COL_BLOCKED, &i);
    }
    //    printf("\n");

  }
  printf("Size of array: %i, value of i: %i\n", (int)sizeof(rgb), i);
  write_bmp(bmp_file, 2 * p_maze->width + 1, 2 * p_maze->height + 1, rgb);

}

static void print_maze(maze_t* p_maze)
{
  system("clear");
  printf("MAZE: w: %i, h: %i\n", p_maze->width, p_maze->height);


  for (uint8_t x = 0; x < p_maze->width; ++x)
  {
    printf("OO");
  }
  printf("O\n");

  for (uint8_t y = 0; y < p_maze->height; ++y)
  {
    printf("O");
    /* print actual rows */
    for (uint8_t x = 0; x < p_maze->width; ++x)
    {
      cell_t* c = get_cell(p_maze, x, y);
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
    for (uint8_t x = 0; x < p_maze->width; ++x)
    {
      cell_t* c = get_cell(p_maze, x, y);
      printf((c->y == 0) ? "OO" : " O");
    }
    printf("\n");

  }

}

static void connect_points(maze_t* p_maze, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
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
      cell_t* p_cell = get_cell(p_maze, x1, y); 
      p_cell->value = 1;
      p_cell->y = 1;
    }
    get_cell(p_maze, x1, y_max)->value = 1;
  }
  else
  {
    for (uint16_t x = x_min; x < x_max; ++x)
    {
      cell_t* p_cell = &p_maze->cells[x + p_maze->width * y1];
      p_cell->value = 1;
      p_cell->x = 1;
    }

    get_cell(p_maze, x_max, y1)->value = 1;
  }

}

static bool is_free(maze_t* p_maze, point_t* p_point, dir_t dir)
{
  switch (dir)
  {
    case DIR_UP:
      return !(p_point->y + 1 == p_maze->height || get_cell(p_maze, p_point->x, p_point->y + 1)->value == 1);
    case DIR_DOWN:
      return !(p_point->y == 0 || get_cell(p_maze, p_point->x, p_point->y - 1)->value == 1);
    case DIR_LEFT:
      return !(p_point->x == 0 || get_cell(p_maze, p_point->x - 1, p_point->y)->value == 1);
    case DIR_RIGHT:
      return !(p_point->x + 1 == p_maze->width || get_cell(p_maze, p_point->x + 1, p_point->y)->value == 1);
  }
}


static bool is_dead_end(maze_t* p_maze, point_t* p_point)
{
  /* true == dead end */
  bool up     = !is_free(p_maze, p_point, DIR_UP); 
  bool down   = !is_free(p_maze, p_point, DIR_DOWN); 
  bool left   = !is_free(p_maze, p_point, DIR_LEFT); 
  bool right  = !is_free(p_maze, p_point, DIR_RIGHT); 

  return (up && down && left && right);
}

static void clear_all_dead_ends(point_list_t* p_list, maze_t* p_maze)
{
  for (uint16_t i = 0; i < p_list->size; ++i)
  {
    bool is_empty = false;
    while (is_dead_end(p_maze, &p_list->points[i]))
    {

      point_list_rm(p_list, i);
      if (p_list->size == 0 || i >= p_list->size)
      {
        is_empty = true;
        break;
      }
    }
    if (is_empty)
    {
      break;
    }
  }
}

static point_t* get_point_furthest_from(point_list_t* p_list, uint16_t x, uint16_t y)
{
  point_t* p_point = &p_list->points[0];
  uint32_t dist_sqr_max = 0; 
  for (uint16_t i = 0; i < p_list->size; ++i)
  {
    point_t* p_point_temp = &p_list->points[i];

    int dist_x = (p_point_temp->x - x);
    int dist_y = (p_point_temp->y - y);

    int dist_sqr_temp = dist_x * dist_x + dist_y * dist_y;

    if (dist_sqr_temp > dist_sqr_max)
    {
      p_point = p_point_temp;
      dist_sqr_max = dist_sqr_temp;
    }
  }
  return p_point;
}


static point_t get_point(point_t* p_base_point, dir_t dir)
{
  switch (dir)
  {
    case DIR_UP:
      return (point_t){p_base_point->x, p_base_point->y + 1};
    case DIR_DOWN:
      return (point_t){p_base_point->x, p_base_point->y - 1};
    case DIR_LEFT:
      return (point_t){p_base_point->x - 1, p_base_point->y};
    case DIR_RIGHT:
      return (point_t){p_base_point->x + 1, p_base_point->y};
  }
}

static void generate_maze_random(maze_t* p_maze)
{
  /* first point: */
  point_list_t points;
  get_cell(p_maze, 0, 0)->value = 1;

  printf("All right!\n");

  point_t initial_point = {0, 0};

  point_list_init(&points);
  point_list_add(&points, initial_point);

  while (points.size > 0)
  {
    while (true)
    {
      point_t point = *point_list_get(&points, rand() % points.size);
      dir_t dir = (dir_t)(rand() % 4);
      if (is_free(p_maze, &point, dir))
      {
        point_t point2 = get_point(&point, dir);
        connect_points(p_maze, point.x, point.y, point2.x, point2.y);
        point_list_add(&points, point2);
        break;
      }
    }
#if WAIT_AND_PRINT
#if (STEP_WAIT > 0)
    sleep(STEP_WAIT);
#endif
    print_maze(p_maze);
#endif
    clear_all_dead_ends(&points, p_maze);
  }

}



static void generate_maze_width_first(maze_t* p_maze)
{
  point_list_t points;
  get_cell(p_maze, 0, 0)->value = 1;

  point_t initial_point = {0, 0, 0};

  point_list_init(&points);
  point_list_add(&points, initial_point);

  while (points.size > 0)
  {
    while (true)
    {
      point_t* point = point_list_get_shallowest(&points);
      dir_t dir = (dir_t)(rand() % 4);
      if (is_free(p_maze, point, dir))
      {
        point_t point2 = get_point(point, dir);
        point2.depth = point->depth;
        connect_points(p_maze, point->x, point->y, point2.x, point2.y);
        point_list_add(&points, point2);
        break;
      }
    }
#if WAIT_AND_PRINT
#if (STEP_WAIT > 0)
    sleep(STEP_WAIT);
#endif
    print_maze(p_maze);
#endif
    clear_all_dead_ends(&points, p_maze);
  }

}

static void generate_maze_depth_first(maze_t* p_maze)
{
  point_list_t points;
  get_cell(p_maze, 0, 0)->value = 1;

  point_t initial_point = {0, 0, 0};

  point_list_init(&points);
  point_list_add(&points, initial_point);

  while (points.size > 0)
  {
    while (true)
    {
      point_t* point = point_list_get_deepest(&points);
      dir_t dir = (dir_t)(rand() % 4);
      if (is_free(p_maze, point, dir))
      {
        point_t point2 = get_point(point, dir);
        point2.depth = point->depth;
        connect_points(p_maze, point->x, point->y, point2.x, point2.y);
        point_list_add(&points, point2);
        break;
      }
    }
#if WAIT_AND_PRINT
#if (STEP_WAIT > 0)
    sleep(STEP_WAIT);
#endif
    print_maze(p_maze);
#endif
    clear_all_dead_ends(&points, p_maze);
  }

}


static void generate_maze_furthest(maze_t* p_maze)
{
  point_list_t points;
  get_cell(p_maze, 0, 0)->value = 1;

  point_t initial_point = {0, 0, 0};

  point_list_init(&points);
  point_list_add(&points, initial_point);
  while (points.size > 0)
  {
    while (true)
    {
      point_t* point = NULL;
      point_list_t temp;
      point_list_init(&temp);
      for (uint8_t i = 0; i < 4; ++i)
        point_list_add(&temp, *point_list_get(&points, rand() % points.size)); // random
      point = get_point_furthest_from(&temp, p_maze->width, p_maze->height);
      dir_t dir = (dir_t)(rand() % 4);
      if (is_free(p_maze, point, dir))
      {
        point_t point2 = get_point(point, dir);
        point2.depth = point->depth;
        connect_points(p_maze, point->x, point->y, point2.x, point2.y);
        point_list_add(&points, point2);
        break;
      }
    }
#if WAIT_AND_PRINT
#if (STEP_WAIT > 0)
    sleep(STEP_WAIT);
#endif
    print_maze(p_maze);
#endif
    clear_all_dead_ends(&points, p_maze);
  }

}

static void generate_maze_branch(maze_t* p_maze)
{
  point_list_t points;
  get_cell(p_maze, 0, 0)->value = 1;

  point_t point = {0, 0, 0};

  point_list_init(&points);
  point_list_add(&points, point);

  uint8_t branch_depth = 0;
  uint8_t branch_end = 10 + rand() % ((p_maze->width + p_maze->height) / 2);
  while (true)
  {
    if (branch_depth++ > branch_end || is_dead_end(p_maze, &point))
    {
      branch_depth = 0;
      branch_end = 10 + rand() % ((p_maze->width + p_maze->height) / 2);
      clear_all_dead_ends(&points, p_maze);
      if (points.size > 0)
        point = *point_list_get(&points, rand() % points.size);
      else
        break;
    }
    else
    {
      dir_t dir = (dir_t) (rand() % 4);
      while (!is_free(p_maze, &point, dir))
      {
        dir = (dir_t) (rand() % 4);
      }
      point_t point2 = get_point(&point, dir);
      connect_points(p_maze, point.x, point.y, point2.x, point2.y);
      point_list_add(&points, point2);
    }
#if WAIT_AND_PRINT
#if (STEP_WAIT > 0)
    sleep(STEP_WAIT);
#endif
    print_maze(p_maze);
#endif
  }

}

int main(int args, char **argv)
{
  srand(time(0));
  if (args < 2)
  {
    printf("Too few arguments. \nUsage: amaze <width> <height>\n\n");
    return 64;
  }

  maze_t maze;
  maze.width = atoi(argv[1]);
  maze.height = atoi(argv[2]);
  maze.cells = (cell_t*) malloc(sizeof(cell_t) * maze.width * maze.height);

  //generate_maze_random(&maze);
  // generate_maze_width_first(&maze);
  //  generate_maze_depth_first(&maze);
  //generate_maze_furthest(&maze);
  generate_maze_branch(&maze);

  printf("FINISHED!");
  fflush(stdout);

  print_maze(&maze);

  printf("FINISHED!");
  fflush(stdout);

  store_maze("maze.bmp", &maze);

  return 0;
}


