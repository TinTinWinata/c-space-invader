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
const int SLEEP_TIME = 20;
const int MAX_BULLET = 255;

// Function Prototype List
void chooseMenuWeapon(int _idx, int _max);
void makeCoordinate(int _x, int _y, char *_symbol);
bool isIntersect(int x, int y, int x2, int y2, int w2, int h2);
void spawnEnemy();
void renderEnemy();
void theGame();
void renderWeaponShop();
int menuWeaponShop();
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
void spawnEnemyLogic();
void renderEnemyBullets(Node *node);
void removeEnemyBullet(int _idx);
void clsCoordinate(int _x, int _y, int _w, int _h);

// Class Prototype List
class Enemy;
class UI;
class Shooter;
class Score;
class Bullet;
class NPC;
class Player;
class Game;
class EnemyBullet;
class PlayerBullet;

// Variable List

int totalEnemy = 0;
int totalNpc = 0;
int enemyCooldown = 3000; // milliseconds
int enemySpawnInterval = 0;
int maxEnemySpawnInterval = enemyCooldown / SLEEP_TIME;
int enemyBullet = 0;

Enemy **enemies = new Enemy *[MAX_ENEMY];
UI *ui;
Score *myScore;
Shooter *shooter;
NPC **npcs = new NPC *[10];
EnemyBullet **enemyBullets = new EnemyBullet *[MAX_BULLET];

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

class Score
{

public:
  int score = 0;
  int uiIdx = 2;
  Score()
  {
    printf("Newing score...");
    score = 0;
  }

  void increment(int _score)
  {
    score += _score;
  }

  void reset()
  {
    score = 0;
  }

  void save()
  {
    // TODO: Save Current Score to highscore.txt
  }

  void renderUi()
  {
    char scoreText[255];
    sprintf(scoreText, "Score : %.2d", score);
    ui->cleanText(uiIdx);
    ui->addText(uiIdx, scoreText);
    ui->renderAll();
  }

  int get()
  {
    return score;
  }
};

class Bullet
{
public:
  int idx;
  int x;
  int y;
  char symbol;
  Node *lastNode;
  int damage = 1;
  int vx = 0;

  Bullet(int _x, int _y, char _symbol, int _idx, int _vx)
  {
    idx = _idx;
    x = _x;
    y = _y;
    symbol = _symbol;
    vx = _vx;
  }

  Bullet(char _symbol)
  {
    symbol = _symbol;
  }

  bool isEnemyIntersect()
  {
    printf("needs to be derived");
  }

  bool isOutsideMap()
  {
    if (x <= 0 || x >= SIZE_GAME_X - 2 || y <= 1 || y >= SIZE_GAME_Y - 1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  // Bullet Logic
  void logic()
  {
    if (lastNode)
    {
      removeCoordinate(y, x);
    }
    lastNode = new Node(x, y);
    x += vx;
  }

  void render()
  {
    logic();
    moveCursor(y, x);
    printf("%c", symbol);
    resetCursor();
  }
};

class EnemyBullet : public Bullet
{
public:
  EnemyBullet(int _x, int _y, char _symbol, int _idx, int _vx) : Bullet(_x, _y, _symbol, _idx, _vx){};
  bool isPlayerIntersect(Node *node)
  {
    if (shooter && isIntersect(x, y, node->x, node->y, node->w, node->h))
    {
      // Player Intersect
      return true;
    }
    else
    {
      return false;
    }
  }
};

class Enemy
{
public:
  char bulletSymbol = '*';

  char sprite[255][255];
  int x;
  int y;
  int w = 3;
  int h = 1;

  int hp = 1;
  int score = 10;
  int idx = -1;

  int shootInterval = 0;
  int maxShootInterval = 200;

  // Movement Speed =  (Sleep Time * Max Interval)
  // If 10, enemy will move 200 milliseconds

  int interval = 0;
  int maxInterval = 100;

  Node *lastNode;

  Enemy(int _x, int _y, int _level, int _idx)
  {
    idx = _idx;
    x = _x;
    y = _y;
    if (_level == 1)
    {
      loadSprite("enemy.txt", sprite);
    }
  }

  void shoot()
  {
    enemyBullets[enemyBullet] = new EnemyBullet(x, (y + w / 2), bulletSymbol, enemyBullet, 1);
    enemyBullet += 1;
  }

  void cleanRemoveNode()
  {
    for (int i = 0; i < h; i++)
    {
      moveCursor(y, x + i);
      for (int j = 0; j < w; j++)
      {
        printf(" ");
      }
    }
  }

  void removeLastNode()
  {
    if (x <= 0 || x >= SIZE_GAME_X - 1)
    {
      return;
    }

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

  void hit(int dmg)
  {
    hp -= dmg;
    if (hp <= 0)
    {
      // Remove Last Node
      if (lastNode)
        removeLastNode();

      // Clean Remove
      cleanRemoveNode();

      // Up Score
      myScore->increment(score);
      myScore->renderUi();

      // Remove enemy to null

      enemies[idx] = NULL;

      printf("Removed! %d", idx);
      getchar();
    }
  }

  void logic()
  {
    if (lastNode)
    {
      removeLastNode();
    }
    lastNode = new Node(x, y, w, h);

    // Movement Interval
    interval += 1;
    if (interval >= maxInterval)
    {
      x += 1;
      interval = 0;
    }

    // Shoot Interval
    shootInterval += 1;
    if (shootInterval >= maxShootInterval)
    {
      shootInterval = 0;
      shoot();
    }
  }

  void render()
  {
    // printf("Rendering\n");
    // getchar();

    logic();
    if (x <= 0 || x >= SIZE_GAME_X - 1)
    {
      return;
    }

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
    if (isLobby() && (LOBBY_ARENA[x][y] == ' ' || LOBBY_ARENA[x][y] == '-'))
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
} game;

class PlayerBullet : public Bullet
{
public:
  PlayerBullet(int _x, int _y, char _symbol, int _idx, int _vx) : Bullet(_x, _y, _symbol, _idx, _vx){

                                                                  };
  bool isEnemyIntersect()
  {
    for (int i = 0; i < totalEnemy; i++)
    {
      if (enemies[i] && isIntersect(x, y, enemies[i]->x, enemies[i]->y, enemies[i]->w, enemies[i]->h))
      {
        // Enemy Intersect
        enemies[i]->hit(damage);
        return true;
      }
    }
    return false;
  }
};

class Shooter
{
public:
  const int maxBullets = 10;
  PlayerBullet **bullets = new PlayerBullet *[maxBullets];
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
  bool dead = false;

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

  void hit(int damage)
  {
    hp -= damage;
    if (hp <= 0)
    {
      dead = true;
    }
    renderStatus();
  }

  void saveLastNode()
  {
    lastNode = new Node(x, y, w, h);
  }

  void shoot()
  {
    bullets[bullet] = new PlayerBullet(x, (y + w / 2), bulletSymbol,
                                       bullet, -1);
    bullet += 1;
  }

  void renderStatus()
  {
    // Score

    // Title
    ui->cleanText(0);
    ui->addText(0, "C Space Invader");

    // Bullet
    ui->cleanText(1);
    char bulletText[255];
    sprintf(bulletText, "Bullets %.2d/%.2d", maxBullets - bullet, maxBullets);
    ui->addText(1, bulletText);

    // Health
    ui->cleanText(6);
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

  void removeBullet(int _idx)
  {
    removeCoordinate(bullets[_idx]->y, bullets[_idx]->x);
    // free(bullets[_idx]);
    bullets[_idx] = NULL;
  }

  void reload()
  {
    for (int i = 0; i < maxBullets; i++)
    {
      if (bullets[i])
      {
        bullets[i] = NULL;
      }
    }
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
    renderStatus();
    resetCursor();
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
        // printf("Rendering bullet : %d\n", i);
        bullets[i]->render();
        if (bullets[i]->isEnemyIntersect() || bullets[i]->isOutsideMap())
        {
          removeBullet(i);
        }
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

//  Class NPC
class NPC
{
public:
  char *type;
  int x;
  int y;
  char symbol;
  bool ready = false;

  NPC(int _x, int _y, char _symbol, char *_type)
  {
    strcpy(type, _type);
    x = _x;
    y = _y;
    symbol = _symbol;
    game.addSymbol(_x, _y, _symbol);
  }

  void yes()
  {
    game.changeState(type);
  }

  bool checkType(char *_str)
  {
    return strcmp(type, _str) == 0;
  }

  void talk()
  {
    ready = true;
    if (checkType("npc_weapon"))
    {
      game.addText("Welcome to Weapon Dealer");
      game.addText("Do you want to buy any weapon ? [y\\n]");
    }
  }
};

class Player
{

public:
  int level = 0;
  int money = 0;
  char *name = "Justine";
  NPC *npcAround;
  char symbol = 'P';
  int x = 3;
  int y = 10;

  int printStatus(int _x, int _y)
  {
    moveCursor(_x, _y);
    printf("[%s]\n", name);
    moveCursor(_x, _y + 1);
    printf("Level : %d\n", level);
    moveCursor(_x, _y + 2);
    printf("Money : %d\n", money);
    resetCursor();
    return 3;
  }

  void
  renderStatus(int _x, int _y)
  {
  }

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
    case 'y':
      if (npcAround && npcAround->ready)
        npcAround->yes();
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
        // npcs[i]->ready = false;
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
} player;

void init()
{
  game.loadLobby();
  game.loadGame();

  npcs[0] = new NPC(16, 6, 'W', "npc_weapon");

  totalNpc = 1;
  forceCls();
}

int main()
{
  init();
  // printf("test");
  theGame();
  return 0;
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
        shooter->move(buffer);
        shooter->logic();
      }
      spawnEnemyLogic();
      renderEnemy();
      renderEnemyBullets(new Node(shooter->x, shooter->y, shooter->w, shooter->h));
      shooter->renderBullets();
      Sleep(SLEEP_TIME);
    }
  }
}

void debug(char *str)
{
  printf("%s", str);
  getchar();
}

void startGame()
{
  forceCls();

  debug("1");
  myScore = new Score();
  debug("2");
  shooter = new Shooter(1);
  debug("3");
  game.changeState("game");
  debug("4");
  ui = new UI(8, 52);
  debug("5");
  myScore->renderUi();
  debug("6");
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

  // Print Status
  player.printStatus(SIZE_LOBBY_Y + 4, 3);
  game.clearText();
}

bool isBlockIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  if (y1 + w1 >= y2 && y1 <= y2 + w2 && x1 + h1 >= x2 && x2 <= x2 + h2)
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
  if (x >= x2 && y <= y2 + w2 && y >= y2 && x <= x2 + h2)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void spawnEnemyLogic()
{
  enemySpawnInterval += 1;
  if (enemySpawnInterval >= maxEnemySpawnInterval)
  {
    enemySpawnInterval = 0;
    spawnEnemy();
  }
}

void spawnEnemy()
{
  int x = 0;
  int y = randomInt(10, SIZE_GAME_Y - 10);
  enemies[totalEnemy] = new Enemy(x, y, 1, totalEnemy);
  totalEnemy += 1;
}

void renderEnemy()
{
  for (int i = 0; i < totalEnemy; i++)
  {
    if (enemies[i])
    {
      enemies[i]->render();
    }
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
  shooter->render();
}

void removeEnemyBullet(int _idx)
{
  if (enemyBullets[_idx])
  {
    removeCoordinate(enemyBullets[_idx]->y, enemyBullets[_idx]->x);
    // free(enemyBullets[_idx]);
    enemyBullets[_idx] = NULL;
  }
}

void renderEnemyBullets(Node *node)
{
  for (int i = 0; i < enemyBullet; i++)
  {
    if (enemyBullets[i])
    {
      enemyBullets[i]->render();
      if (enemyBullets[i]->isPlayerIntersect(node) || enemyBullets[i]->isOutsideMap())
      {
        shooter->hit(enemyBullets[i]->damage);
        removeEnemyBullet(i);
      }
    }
  }
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
  else if (strcmp(game.state, "npc_weapon") == 0)
  {
    renderWeaponShop();
  }
  else if (strcmp(game.state, "npc_item") == 0)
  {
  }
  else if (strcmp(game.state, "npc_upgrade") == 0)
  {
  }
}

void renderWeaponShop()
{
  forceCls();
  int idx = 0;
  int max = menuWeaponShop();
  int lastIdx = -1;
  while (true)
  {
    char buffer = getch();

    if (lastIdx != -1)
    {
      removeCoordinate(20, 1 + lastIdx);
    }

    if (buffer == 'w')
    {
      if (idx > 0)
      {
        idx -= 1;
      }
    }
    else if (buffer == 's')
    {
      if (idx + 1 < max)
      {
        idx += 1;
      }
    }
    else if (buffer == '\r')
    {
      chooseMenuWeapon(idx, max);
      break;
    }
    lastIdx = idx;
    makeCoordinate(20, 1 + idx, "<");
  }
  game.changeState("lobby");
  render();
}

void chooseMenuWeapon(int _idx, int _max)
{
  switch (_idx)
  {
  case 0:
    makeCoordinate(0, _max + 2, "You choose pistol!");
    break;
  case 1:
    makeCoordinate(0, _max + 2, "You choose machinegun!");
    break;
  case 2:
    makeCoordinate(0, _max + 2, "Exit!");
    break;
  }
  return;
}

int menuWeaponShop()
{
  printf("Welcome to weapon shop\n");
  printf("1. Pistol\n");
  printf("2. Machinegun\n");
  printf("3. Exit\n");
  return 3;
}

void forceCls()
{
  // cls();
  // clrscr();
  // printf("SYSTEM CLEAR");
  // system("cls");
  // printf("TERCLEAR");
  resetCursor();
  for (int i = 0; i < 80; i++)
    printf("%s", EMPTY_50);
  resetCursor();
}

void clsCoordinate(int _x, int _y, int _w, int _h)
{
  for (int i = 0; i < _h; i++)
  {
    moveCursor(_x, _y + i);
    for (int j = 0; j < _w; j++)
    {
      printf("%c");
    }
  }
  resetCursor();
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

void makeCoordinate(int _x, int _y, char *_symbol)
{
  moveCursor(_x, _y);
  printf("%s", _symbol);
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