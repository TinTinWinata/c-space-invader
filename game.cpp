#include <stdio.h>
#include <string.h>

const int SIZE_LOBBY_X = 22;
const int SIZE_LOBBY_Y = 35;

const int SIZE_GAME_X = 60;
const int SIZE_GAME_Y = 60;

class Game
{
private:
  static Game *game;

public:
  char state[255] = "lobby";

  static Game *getInstance()
  {
    if (game == NULL)
    {
      game = new Game();
    }
    return game;
  }

  char text[255];
  char LOBBY_ARENA[SIZE_LOBBY_X][SIZE_LOBBY_Y];

  void loadLobby()
  {
    FILE *ptr;
    char ch;
    ptr = fopen("lobby.txt", "r");
    int jIdx = 0;
    int iIdx = 0;
    do
    {
      ch = fgetc(ptr);
      bool flag = false;
      if (ch == '\n')
      {
        flag = true;
        iIdx += 1;
      }
      else
      {
        LOBBY_ARENA[iIdx][jIdx] = ch;
      }

      jIdx += 1;
      if (flag)
        jIdx = 0;

    } while (ch != EOF);
    fclose(ptr);
  }

  bool isIntersect(int x, int y)
  {
    if (LOBBY_ARENA[x][y] == ' ')
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void render()
  {
    if (strcmp(state, "lobby") == 0)
    {
      for (int i = 0; i < SIZE_LOBBY_X; i++)
      {
        printf("%s\n", LOBBY_ARENA[i]);
      }
    }
  }
};