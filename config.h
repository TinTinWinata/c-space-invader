#include <stdlib.h>

const int SIZE_LOBBY_X = 22;
const int SIZE_LOBBY_Y = 35;

const int SIZE_GAME_X = 22;
const int SIZE_GAME_Y = 50;

const char *EMPTY_SPACE = "     ";
const char *EMPTY_50 = "                                                  ";

const int SPACE_1_W = 5;
const int SPACE_1_H = 5;

const int SPACE_2_W = 7;
const int SPACE_2_H = 5;

const int SPACE_3_W = 3;
const int SPACE_3_H = 4;

const int SPACE_4_W = 7;
const int SPACE_4_H = 5;

const int TOTAL_SPACE = 4;

const int ENEMY_WIDTH = 3;

const int AREA[3] = {1, -1, 0};

const char SPACESHIP_NAME[TOTAL_SPACE][255] = {
    {"Default Spaceship"}, {"Great Spaceship"}, {"Little Spaceship"}, {"Rocket Spaceship"}};

void PRINT_EMPTY()
{
  printf("%s", EMPTY_SPACE);
}

class Node
{
public:
  int x;
  int y;
  int w;
  int h;
  Node(int _x, int _y, int _w, int _h)
  {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
  }

  Node(int _x, int _y)
  {
    x = _x;
    y = _y;
  }

  // Debug Function Purpose
  void debug()
  {
    printf("%d | %d | %d | %d\n", x, y, w, h);
    getchar();
  }
};

int randomInt(int min, int max)
{
  return min + rand() % (max + 1 - min);
}

float randomFloat()
{
  float r = (float)rand() / (float)RAND_MAX;
  return r;
}