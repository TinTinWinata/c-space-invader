#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <string.h>
#include <conio.h>
#include <windows.h>

// Variable
const int TOTAL_TEXT = 3;
const int MAX_UI_TEXT = 10;
const int MAX_ENEMY = 255;

// Function Prototype List
void spawnEnemy();
void renderEnemy();
void theGame();
void render();
void renderGame();
void forceCls();
void cls();
void renderLobby();
void moveCursor(int x, int y);
void resetCursor();
void removeCoordinate(int x, int y);
void startGame();
void loadSprite(char *_filename, char sprite[255][255]);

class UI
{
public:
  int x;
  int totalText = 0;
  char text[MAX_UI_TEXT][255];
  int y;

  UI(int _x, int _y)
  {
    x = _x;
    y = _y;
    cleanAllText();
  }

  void addText(int idx, char *val)
  {
    if (idx >= MAX_UI_TEXT)
    {
      printf("Invalid text!");
      getchar();
    }
    strcpy(text[idx], val);
  }

  void cleanAllText()
  {
    for (int i = 0; i < MAX_UI_TEXT; i++)
    {
      strcpy(text[i], EMPTY_50);
    }
  }

  void cleanText(int idx)
  {
    strcpy(text[idx], EMPTY_50);
  }

  void render(int idx)
  {
    moveCursor(y, x + idx);
    printf("%s", text[idx]);
    resetCursor();
  }

  void renderAll()
  {
    for (int i = 0; i < MAX_UI_TEXT; i++)
    {
      moveCursor(y, x + i);
      printf("%s", text[i]);
      resetCursor();
    }
  }
};

UI *ui;

class Enemy
{
public:
  char sprite[255][255];

  int x;
  int y;
  int w = 3;
  int h = 1;
  int interval = 0;
  int maxInterval = 10;
  Node *lastNode;

  Enemy(int _x, int _y)
  {
    x = _x;
    y = _y;
    loadSprite("enemy.txt", sprite);
  }

  void removeLastNode()
  {
    for (int i = 0; i < lastNode->h; i++)
    {
      moveCursor(lastNode->y, lastNode->x + i);
      for (int j = 0; j < lastNode->w; j++)
      {
        printf(" ");
      }
    }
    lastNode = NULL;
  }

  void logic()
  {
    if (lastNode)
    {
      removeLastNode();
    }
    lastNode = new Node(x, y, w, h);

    interval += 1;
    if (interval >= maxInterval)
    {
      x += 1;
      interval = 0;
    }
  }

  void render()
  {
    logic();
    if (x <= 0 || x >= SIZE_GAME_X)
      return;

    for (int i = 0; i < h; i++)
    {
      moveCursor(y, x + i);
      for (int j = 0; j < w; j++)
      {
        printf("%c", sprite[i][j]);
      }
    }
    resetCursor();
  }
};

int totalEnemy = 0;
Enemy **enemies = new Enemy *[MAX_ENEMY];

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
  bool forceClsFlag = false;

  void changeState(char *_state)
  {
    strcpy(state, _state);
  }

  void nextForceCLS()
  {
    forceClsFlag = true;
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

  bool isLobby()
  {
    return strcmp(state, "lobby") == 0;
  }

  bool isGame()
  {
    return strcmp(state, "game") == 0;
  }

  bool isInsideArena(int _x, int _y, int _w, int _h)
  {
    if (_x > 0 && _y + _w < SIZE_GAME_Y - 1 && _y > 0 && _x + _h < SIZE_GAME_X)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool isIntersect(int x, int y)
  {
    if (isLobby() && LOBBY_ARENA[x][y] == ' ')
    {
      return true;
    }
    else if (isGame() && GAME_ARENA[x][y] == ' ')
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

class Bullet
{
public:
  int x;
  int y;
  char symbol;
  Node *lastNode;

  Bullet(int _x, int _y, char _symbol)
  {
    x = _x;
    y = _y;
    symbol = _symbol;
  }

  Bullet(char _symbol)
  {
    symbol = _symbol;
  }

  // Bullet Logic
  void logic()
  {
    if (lastNode)
    {
      removeCoordinate(y, x);
    }
    lastNode = new Node(x, y);

    x -= 1;
  }

  void render()
  {
    logic();

    if (x <= 0 || x >= SIZE_GAME_X)
      return;

    moveCursor(y, x);
    printf("%c", symbol);
    resetCursor();
  }
};

class Shooter
{
public:
  const int maxBullets = 10;
  Bullet **bullets = new Bullet *[maxBullets];

  Node *lastNode = NULL;
  int hp = 100;
  int maxHp = 100;
  int h = 5;
  int w = 7;
  int x = 10;
  int y = 10;
  char shooter[255][255];
  char bulletSymbol = '^';
  int bullet = 0;

  Shooter(int level)
  {
    if (level == 1)
    {
      loadPlayer("space.txt");
    }
    else if (level == 2)
    {
      // !TODO Build level 2
    }
  }

  void saveLastNode()
  {
    lastNode = new Node(x, y, w, h);
  }

  void shoot()
  {
    bullets[bullet] = new Bullet(x, (y + w / 2), bulletSymbol);
    bullet += 1;
  }

  void renderStatus()
  {
    ui->cleanAllText();

    // Title
    ui->addText(0, "C Space Invader");

    // Bullet
    char bulletText[255];
    sprintf(bulletText, "Bullets %.2d/%.2d", maxBullets - bullet, maxBullets);
    ui->addText(1, bulletText);

    // Health
    char hpText[255] = "| ";
    int mod = hp / 10;
    for (int i = 0; i < hp; i += mod)
    {
      strcat(hpText, "-");
    }
    char hpTextTemp[255] = "";
    sprintf(hpTextTemp, " | [%d/%d]", hp, maxHp);
    strcat(hpText, hpTextTemp);
    ui->addText(6, hpText);

    // Render All
    ui->renderAll();
  }

  void reload()
  {
    bullet = 0;
  }

  void removeLastNode()
  {
    if (lastNode)
    {
      for (int i = 0; i < lastNode->h; i++)
      {
        moveCursor(lastNode->y, lastNode->x + i);
        for (int j = 0; j < lastNode->w; j++)
        {
          printf(" ");
        }
      }
      lastNode = NULL;
    }
  }

  void render()
  {
    removeLastNode();
    for (int i = 0; i < h; i++)
    {
      moveCursor(y, x + i);
      for (int j = 0; j < w; j++)
      {
        printf("%c", shooter[i][j]);
      }
    }
    resetCursor();
    renderStatus();
  }

  // Shooter Logic
  void logic()
  {
    render();
  }

  void renderBullets()
  {
    for (int i = 0; i < bullet; i++)
    {
      if (bullets[i])
      {
        bullets[i]->render();
      }
    }
  }

  // Shooter Move
  void move(char str)
  {
    saveLastNode();
    switch (str)
    {
    case 'r':
      reload();
      break;
    case 'w':
      if (game.isInsideArena(x - 1, y, w, h))
      {
        x -= 1;
      }
      break;
    case 'd':
      if (game.isInsideArena(x, y + 1, w, h))
      {
        y += 1;
      }
      break;
    case 's':
      if (game.isInsideArena(x + 1, y, w, h))
      {
        x += 1;
      }
      break;
    case 'a':
      if (game.isInsideArena(x, y - 1, w, h))
      {
        y -= 1;
      }
      break;
    case ' ':
      if (bullet < maxBullets)
      {
        shoot();
      }
      break;
    }
  }

  void loadPlayer(char *_filename)
  {
    FILE *ptr;
    char ch;
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

Shooter shooter(1);

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
        startGame();
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

void init()
{
  game.loadLobby();
  game.loadGame();

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

    if (strcmp(game.state, "lobby") == 0)
    {
      char buffer = getch();
      player.move(buffer);
      player.logic();
      render();
    }
    else if (strcmp(game.state, "game") == 0)
    {
      if (kbhit())
      {
        char buffer = getch();
        if (buffer == 'o')
        {
          spawnEnemy();
        }
        shooter.move(buffer);
        shooter.logic();
      }
      Sleep(20);
      renderEnemy();
      shooter.renderBullets();
    }
  }
}

void startGame()
{
  forceCls();
  game.changeState("game");
  ui = new UI(8, 52);
  // shooter.renderStatus();
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

        // If exists Print the text
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

bool isBlockIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y2 <= y2 + h2)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool isIntersect(int x, int y, int x2, int y2, int w2, int h2)
{
  if (x >= x2 && x <= x2 + w2 && y > y2 && y <= y2 + h2)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void spawnEnemy()
{
  int x = 0;
  int y = randomInt(10, SIZE_GAME_Y - 10);
  enemies[totalEnemy] = new Enemy(x, y);
  totalEnemy += 1;
}

void renderEnemy()
{
  for (int i = 0; i < totalEnemy; i++)
  {
    enemies[i]->render();
  }
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
  shooter.render();
}

void render()
{
  if (game.forceClsFlag)
  {
    forceCls();
  }
  else
  {
    cls();
  }

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

void resetCursor()
{
  COORD coord;
  coord.X = 0;
  coord.Y = 0;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void moveCursor(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void removeCoordinate(int x, int y)
{
  moveCursor(x, y);
  printf(" ");
  resetCursor();
}

void loadSprite(char *_filename, char sprite[255][255])
{
  FILE *ptr;
  char ch;
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
      sprite[iIdx][jIdx] = ch;
    }
    jIdx += 1;
    if (flag)
      jIdx = 0;
  } while (ch != EOF);
  fclose(ptr);
}
