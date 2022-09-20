#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <string.h>
#include <conio.h>
#include <windows.h>

const int TOTAL_TEXT = 3;

class Shooter
{
public:
  int h = 5;
  int w = 7;
  int x = 3;
  int y = 3;
  char shooter[255][255]; 
  Shooter(int level)
  {
    if (level == 1)
    {
      loadPlayer("space");
    }
    else if (level == 2)
    {
    }
  }

  void loadPlayer(char *_filename)
  {
    FILE *ptr;
    char ch;
    strncat(_filename, ".txt", 4);
    ptr = fopen(_filename, "r");
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
        shooter[iIdx][jIdx] = ch;
      }
      jIdx += 1;
      if (flag)
        jIdx = 0;
    } while (ch != EOF);
    fclose(ptr);
  }
};

class Game
{
public:
  char state[255] = "lobby";
  char text[TOTAL_TEXT][255];
  int textInit[TOTAL_TEXT] = {10, 11, 12};
  int statusInit = 14;
  char statusText[255] = "";
  char LOBBY_ARENA[SIZE_LOBBY_X][SIZE_LOBBY_Y];
  char GAME_ARENA[SIZE_GAME_X][SIZE_GAME_Y];

  void changeState(char *_state)
  {
    strcpy(state, _state);
  }

  Game()
  {
    clearText();
  }

  void addText(char _text[255])
  {
    for (int i = 0; i < TOTAL_TEXT; i++)
    {
      if (strcmp(text[i], EMPTY_50) == 0)
      {
        strcpy(text[i], _text);
        return;
      }
    }
  }

  void clearText()
  {
    for (int i = 0; i < TOTAL_TEXT; i++)
    {
      strcpy(text[i], EMPTY_50);
    }
  }

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
  void loadGame()
  {
    FILE *ptr;
    char ch;
    ptr = fopen("game.txt", "r");
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
        GAME_ARENA[iIdx][jIdx] = ch;
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
  void addSymbol(int _x, int _y, char _symbol)
  {
    if (strcmp(state, "lobby") == 0)
    {
      LOBBY_ARENA[_x][_y] = _symbol;
    }
    else
    {
    }
  }
};

Game game;

//  Class NPC
class NPC
{
public:
  char *name;
  int x;
  int y;
  char symbol;

  NPC(int _x, int _y, char _symbol)
  {
    x = _x;
    y = _y;
    symbol = _symbol;
    game.addSymbol(_x, _y, _symbol);
  }
  void talk()
  {
    game.addText("Welcome to Weapon Dealer");
    game.addText("Do you want to buy any weapon ? [y\\n]");
  }
};

class WeaponDealer : public NPC
{
public:
  WeaponDealer(int _x, int _y, char _symbol) : NPC(_x, _y, _symbol)
  {
  }

  void talk()
  {
    game.addText("Welcome to Weapon Dealer");
    game.addText("Do you want to buy any weapon ? [y\\n]");
  }
};

int totalNpc;
NPC **npcs = new NPC *[10];

// Class Player
class Player
{

public:
  NPC *npcAround;
  char symbol = 'P';
  int x = 3;
  int y = 10;

  void move(char str)
  {
    switch (str)
    {
    case 'w':
      if (game.isIntersect(x - 1, y))
      {
        x -= 1;
      }
      break;
    case 'd':
      if (game.isIntersect(x, y + 1))
      {
        y += 1;
      }
      break;
    case 's':
      if (game.isIntersect(x + 1, y))
      {
        x += 1;
      }
      break;
    case 'a':
      if (game.isIntersect(x, y - 1))
      {
        y -= 1;
      }
      break;
    case ' ':
      if (npcAround)
        npcAround->talk();
      if (isInPortal())
      {
        game.changeState("game");
      }
      break;
    }
  }

  bool isAroundNPC()
  {
    // Check for the npcs
    for (int i = 0; i < totalNpc; i++)
    {
      bool bottom = x + 1 == npcs[i]->x && y == npcs[i]->y;
      bool top = x - 1 == npcs[i]->x && y == npcs[i]->y;
      bool right = x == npcs[i]->x && y + 1 == npcs[i]->y;
      bool left = x == npcs[i]->x && y - 1 == npcs[i]->y;

      if (bottom || top || right || left)
      {
        npcAround = npcs[i];
        return true;
      }
    }
    return false;
  }

  bool isInPortal()
  {
    if (x == 10 && y == 17)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  // Player Logic
  void logic()
  {
    if (isAroundNPC())
    {
      strcpy(game.statusText, "Press SPACE To Interact");
    }
    if (isInPortal())
    {
      strcpy(game.statusText, "Press SPACE to interact");
    }
  }
};
Player player;

// Function Prototype List
void theGame();
void render();
void forceCls();
void cls();
void renderLobby();

void init()
{
  game.loadLobby();

  *npcs = new WeaponDealer(16, 6, 'W');
  totalNpc = 1;

  forceCls();
}

int main()
{
  init();
  theGame();
}

void theGame()
{
  render();
  while (true)
  {
    char buffer = getch();
    player.move(buffer);
    player.logic();
    render();
  }
}

void renderLobby()
{
  for (int i = 0; i < SIZE_LOBBY_X; i++)
  {
    for (int j = 0; j < SIZE_LOBBY_Y; j++)
    {
      if (player.x == i && player.y == j)
      {
        // Print Player
        printf("%c", player.symbol);
      }
      else
      {
        printf("%c", game.LOBBY_ARENA[i][j]);
      }
    }
    for (int j = 0; j < TOTAL_TEXT; j++)
    {
      if (i == game.textInit[j])
      {
        // Validate is text is empty_50 (Default Value)
        if (strcmp(game.text[j], EMPTY_50) == 0)
        {
          printf("%s", EMPTY_50);
          continue;
        }

        // If exsts Print the text
        PRINT_EMPTY();
        int len = strlen(game.text[j]);
        for (int k = 0; k < len; k++)
        {
          Sleep(20);
          printf("%c", game.text[j][k]);
        }
      }
    }

    if (i == game.statusInit)
    {
      PRINT_EMPTY();
      printf("%s", game.statusText);
      strcpy(game.statusText, EMPTY_50);
    }
    printf("\n");
  }
  game.clearText();
}

void renderGame()
{
  for (int i = 0; i < SIZE_GAME_X; i++)
  {
    for (int j = 0; j < SIZE_GAME_Y; j++)
    {

      printf("%c", game.GAME_ARENA[i][j]);
    }
    printf("\n");
  }
}

void render()
{
  cls();

  if (strcmp(game.state, "lobby") == 0)
  {
    renderLobby();
  }
  else if (strcmp(game.state, "game") == 0)
  {
    renderGame();
  }
}
void forceCls()
{
  system("cls");
}

void cls()
{
  // Custom Clear Screen -> Not removing all buffer, just a needed pos onlys

  COORD cursorPosition;
  cursorPosition.X = 0;
  cursorPosition.Y = 0;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}
