#include <stdlib.h>

const int SIZE_LOBBY_X = 22;
const int SIZE_LOBBY_Y = 35;

const int SIZE_GAME_X = 22;
const int SIZE_GAME_Y = 50;

const char *EMPTY_SPACE = "     ";
const char *EMPTY_50 = "                                                  ";
const char *EMPTY_100 = "                                                                                                    ";

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
