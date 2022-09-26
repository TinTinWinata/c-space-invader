#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Variable
const int MAIN_MENU_X = 3;
const int MAIN_MENU_Y = 2;
const int TOTAL_TEXT = 3;
const int MAX_UI_TEXT = 10;
const int MAX_ENEMY = 255;
const int MAX_BULLET = 255;
const int LOBBY_STATUS_COORDINATE_X = SIZE_LOBBY_Y + 4;
const int ENEMY_COOLDOWN = 2000; // milliseconds
const int ENEMY_MOVEMENT_INTERVAL = 3000;
const int ENEMY_SPAWN_PER_WAVE = 3;
const int ENEMY_SHOOT_INTERVAL = 1000;
const int MAX_XP_TO_LEVEL_UP = 100;
const int MAX_PLAYER_LVL = 100;
const int LOADING_TIME = 1000;
const int NEED_LEVEL_TO_OPEN_SPACESHIP = 10;

// Price Item Variable
const int POTION_PRICE = 5;
const int MAX_POTION_PRICE = 20;
const int ENERGY_DRINK_PRICE = 3;
const int MAX_ENERGY_DRINK_PRICE = 15;
const int BOMB_PRICE = 10;

const int UPGRADE_HP_PRICE = 50;
const int UPGRADE_ENERY_PRICE = 50;
const int UPGRADE_ARMOR_PRICE = 30;
const int UPGRADE_DAMAGE_PRICE = 10;

const int MAX_HP = 300;
const int MAX_ENERGY = 500;
const int MAX_ARMOR = 30;
const int MAX_DAMAGE = 10;

// Item Variable
const int POTION_HP_INCREMENT = 50;
const int ENERGY_DRINK_INCREMENT = 50;

// Function Prototype List
void resetColor();
void printSetting(int _x, int _y);
void printHowToPlay(int _x, int _y);
void playSound(char str[255]);
void chooseMenu(int _x, int _y);
void endScreen();
void makeCharCoordinate(int _x, int _y, char _symbol);
void printFinishGame(int _score, int _level);
void arraySetSpace(char _arr[255][255], int _w, int _h);
void printSpace(int _idx);
void loadSprite(char *_filename, char sprite[255][255]);
void makeCoordinate(int _x, int _y, char *_symbol);
void removeCoordinate(int x, int y);
void moveCursor(int x, int y);
void resetCursor();
void cls();
void clsCoordinate(int _x, int _y, int _w, int _h);
void forceCls();
void finishGame();
void menuWeaponShop(int _idx);
void chooseMenuWeapon(int _idx, int _max);
void renderWeaponShop();
void render();
void renderEnemyBullets(Node *node);
void removeEnemyBullet(int _idx);
void renderGame();
void renderEnemy();
void spawnEnemyLogic();
bool isIntersect(int x, int y, int x2, int y2, int w2, int h2);
bool isBlockIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void exitGame();
void renderLobby();
void startGame();
void debug(char *str);
void theGame();
void init();
void renderItemShop();
void chooseMenuItem(int _idx, int _max);
int menuItemShop();
void successBuyStatus(int _howMany, char _name[255]);
void dontHaveMoneyStatus();
int scanIndex(char str[255]);
int choosedBuyMenu(int _price, char _name[255]);
void renderUpgradeShop();
bool chooseMenuUpgrade(int _idx, int _max);
bool chooseUpgradeMenu(int _price, int _max, int _base);
void checkMovementEnemy();
void useItem(char str[255]);
bool isSpaceshipOpen();

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
class Bomb;
class Data;

// Variable List

int SLEEP_TIME = 20;

int totalEnemy = 0;
int totalNpc = 0;
int enemySpawnInterval = 0;
int maxEnemySpawnInterval = ENEMY_COOLDOWN / SLEEP_TIME;
int enemyBullet = 0;
int enemyMovementInterval = 0;
int maxEnemyMovementInterval = ENEMY_MOVEMENT_INTERVAL / SLEEP_TIME;
bool GAME_IS_RUNNING = false;
bool GAME_STARTING_FLAG = false;

Enemy **enemies = new Enemy *[MAX_ENEMY];
UI *ui = NULL;
Score *myScore = NULL;
Shooter *shooter = NULL;
EnemyBullet **enemyBullets = new EnemyBullet *[MAX_BULLET];
Bomb *bomb = NULL;
Data *data = NULL;

class UI
{
public:
  int x;
  int totalText = 0;
  char text[MAX_UI_TEXT][255];
  int y;
  char valueColor[255][255];
  int indexColor[255];
  int totalColor = 0;

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

  void color(int i, char str[255])
  {
    indexColor[totalColor] = i;
    strcpy(valueColor[totalColor], str);
    totalColor += 1;
  }

  void cleanText(int idx)
  {
    // Variable Clean
    strcpy(text[idx], EMPTY_50);
    // totalColor = 0;

    // Real Clean
    moveCursor(y, x + idx);
    printf("%s", EMPTY_50);
    resetCursor();
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
      // Checking Color

      // First Always Get Colored Red
      if (i == 0)
      {
        printf(COLOR_RESET);
        printf("\u001b[41m");
      }

      for (int j = 0; j < totalColor; j++)
      {
        if (i == indexColor[j])
        {
          printf(valueColor[j]);
        }
      }
      printf("%s", text[i]);

      resetColor();
      resetCursor();
    }
  }
};

class Score
{
public:
  int score = 0;
  int uiIdx = 3;
  char allName[255][255];
  char allScore[255];
  int allIdx = 0;
  Score()
  {
    score = 0;
    load();
  }

  void increment(int _score)
  {
    score += _score;
  }

  void reset()
  {
    score = 0;
  }

  // Load Score
  void load()
  {
    char line[255];
    FILE *fp;
    fp = fopen("score.dat", "r");
    if (fp == NULL)
    {
      printf("Missing score.dat files ! [press enter]");
      getchar();
      exit(1);
      return;
    }

    while (fgets(line, 255, fp))
    {
      char *data = strtok(line, "#");
      int idx = 0;
      while (data != NULL)
      {
        if (idx == 0)
        {
          // Get name
          strcpy(allName[allIdx], data);
        }
        else if (idx == 1)
        {
          int currScore = strtol(data, NULL, 10);
          allScore[allIdx] = currScore;
          // Get Score
        }
        idx += 1;
        data = strtok(NULL, "#");
      };
      allIdx += 1;
    }
    fclose(fp);
  }

  void sort()
  {
    for (int i = 0; i < allIdx - 1; i++)
    {
      for (int j = 0; j < allIdx - i - 1; j++)
      {
        if (allScore[j] < allScore[j + 1])
        {

          // Swap Score
          int tempScore = allScore[j];
          allScore[j] = allScore[j + 1];
          allScore[j + 1] = tempScore;

          // Swap Name
          char tempName[255];
          strcpy(tempName, allName[j]);
          strcpy(allName[i], allName[j + 1]);
          strcpy(allName[j + 1], tempName);
        }
      }
    }
  }

  // Highscore Print
  void print()
  {
    printf("\n\tC Space Invader Scoreboard\n\n");
    if (allIdx == 0)
    {
      printf("\n\tThere's no other competitor!\n");
      return;
    }

    sort();

    for (int i = 0; i < allIdx; i++)
    {
      printf("\t%d. %s | %d\n", i + 1, allName[i], allScore[i]);
    }
  }

  void save()
  {
    FILE *fp;
    fp = fopen("score.dat", "w");
    if (fp == NULL)
    {
      printf("Missing score.dat files ! [press enter]");
      getchar();
      exit(1);
      return;
    }
    for (int i = 0; i < allIdx; i++)
    {
      char text[255];
      sprintf(text, "%s#%d\n", allName[i], allScore[i]);
      fprintf(fp, text);
    }
    fclose(fp);
  }

  void insert(int _score, char _name[255])
  {

    bool isFound = false;
    for (int i = 0; i < allIdx; i++)
    {
      if (strcmp(allName[i], _name) == 0)
      {
        // printf("Found! %s changing to %d", name, _score);
        // getchar();
        // Is found just update the score
        if (allScore[i] < _score)
        {
          allScore[i] = _score;
        }
        isFound = true;
        return;
      }
    }
    if (!isFound)
    {
      // If not found make another var
      strcpy(allName[allIdx], _name);
      allScore[allIdx] = _score;
      allIdx += 1;
    }
    sort();
  }

  void renderUi()
  {
    char scoreText[255];
    sprintf(scoreText, "Score : %.2d", score);
    ui->cleanText(uiIdx);
    ui->color(uiIdx, COLOR_YELLOW);
    ui->addText(uiIdx, scoreText);
    ui->renderAll();
  }

  int getScore()
  {
    return score;
  }
};

class Bullet
{
public:
  bool dead = false;
  int idx;
  int x;
  int y;
  char symbol;
  Node *lastNode;
  int damage = 1;
  int vx = 0;
  int vy = 0;

  Bullet(int _x, int _y, char _symbol, int _idx, int _vx, int _vy, int _damage)
  {
    damage = _damage;
    idx = _idx;
    x = _x;
    y = _y;
    symbol = _symbol;
    vx = _vx;
    vy = _vy;
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

  void remove()
  {
    removeCoordinate(y, x);
  }

  void logic()
  {
    if (lastNode)
    {
      removeCoordinate(y, x);
      delete lastNode;
    }
    lastNode = new Node(x, y);
    x += vx;
    y += vy;
  }

  void render()
  {
    if (dead)
      return;
    logic();
    moveCursor(y, x);
    printf("%c", symbol);
    resetCursor();
  }
};

class EnemyBullet : public Bullet
{
public:
  EnemyBullet(int _x, int _y, char _symbol, int _idx, int _vx, int _vy, int _damage) : Bullet(_x, _y, _symbol, _idx, _vx, _vy, _damage){};
  bool isPlayerIntersect(Node *node)
  {
    if (dead)
      return false;
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
  int damage = 10;
  char sprite[255][255];
  int x;
  int y;
  int w = 3;
  int h = 1;

  int hp = 1;
  int score = 10;
  int idx = -1;

  float shootInterval = 0;
  int maxShootInterval = ENEMY_SHOOT_INTERVAL / SLEEP_TIME;

  // Movement Speed =  (Sleep Time * Max Interval)
  // If 10, enemy will move 200 milliseconds

  int interval = 0;
  int maxInterval = 100;
  bool left = false;

  Node *lastNode;

  Enemy(int _x, int _y, int _level, int _idx)
  {
    idx = _idx;
    x = _x;
    y = _y;
    char text[255];
    sprintf(text, "enemy_%d.txt", _level);
    loadSprite(text, sprite);

    if (_level == 1)
    {
      damage = 10;
    }
    else if (_level == 2)
    {
      damage = 20;
    }
    else if (_level == 3)
    {
      damage = 25;
    }
  }

  void move()
  {
    if (left)
    {
      left = false;

      // go right
      y += 1;
    }
    else
    {
      left = true;

      // go left
      y -= 1;
    }
  }

  void shoot()
  {
    enemyBullets[enemyBullet] = new EnemyBullet(x, (y + w / 2), bulletSymbol, enemyBullet, 1, 0, damage);
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
    if (x < 0 || x >= SIZE_GAME_X - 1)
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
    char text[255];
    // sprintf(text, "hitted : %d hp %d damage", hp, dmg);
    // debug(text);
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
      if (enemies[idx])
        enemies[idx] = NULL;
    }
  }

  // Enemy Logic
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
      if (x >= SIZE_GAME_X - 2)
      {
        finishGame();
      }
      interval = 0;
    }

    // Shoot Interval
    shootInterval += randomFloat();
    if (shootInterval >= maxShootInterval)
    {
      shootInterval = 0;
      shoot();
    }
  }

  // Enemy Render
  void render()
  {
    logic();
    if (x < 0 || x >= SIZE_GAME_X - 1)
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

class Bomb
{
public:
  char symbol = '.';
  int damage = 99;

  Bomb(){};

  void initMap(bool str[SIZE_GAME_X][SIZE_GAME_Y])
  {
    for (int i = 0; i < SIZE_GAME_X; i++)
    {
      for (int j = 0; j < SIZE_GAME_Y; j++)
      {
        str[i][j] = false;
      }
    }
  }
  void bomb(int _x, int _y)
  {
    // Queue Floodfill in C (Using simple array)

    bool ARENA_GRID[SIZE_GAME_X][SIZE_GAME_Y];
    initMap(ARENA_GRID);

    int queue_x[SIZE_GAME_X * SIZE_GAME_Y];
    int queue_y[SIZE_GAME_X * SIZE_GAME_Y];

    // Fill Empty Array With - 1
    for (int i = 0; i < SIZE_GAME_X * SIZE_GAME_Y; i++)
    {
      queue_x[i] = -1;
      queue_y[i] = -1;
    }

    int idx = 0;
    int nextIdx = 0;

    // Fill first
    queue_x[0] = _x;
    queue_y[0] = _y;
    ARENA_GRID[_x][_y] = true;

    while (queue_x[nextIdx] != -1 && queue_y[nextIdx] != -1)
    {
      int currX = queue_x[nextIdx];
      int currY = queue_y[nextIdx];

      for (int i = 0; i < totalEnemy; i++)
      {
        if (enemies[i] && isIntersect(currX, currY, enemies[i]->x, enemies[i]->y, enemies[i]->w, enemies[i]->h))
        {
          // Enemy Intersect
          enemies[i]->hit(damage);
          // break;
        }
      }

      makeCharCoordinate(currY, currX, symbol);
      // Sleep(2);
      // removeCoordinate(currY, currX);

      for (int i = 0; i < 2; i++)
      {
        for (int j = 0; j < 2; j++)
        {
          // Add around to queue
          if (AREA[i] == 0 && AREA[j] == 0)
            continue;

          int nextX = currX + AREA[i];
          int nextY = currY + AREA[j];

          if (nextX < 0 || nextY < 1 || nextX >= SIZE_GAME_X - 1 || nextY >= SIZE_GAME_Y - 2)
            continue;

          if (ARENA_GRID[nextX][nextY] == true)
            continue;

          idx += 1;
          ARENA_GRID[nextX][nextY] = true;
          queue_x[idx] = nextX;
          queue_y[idx] = nextY;
        }
      }
      nextIdx += 1;
    }
    // Backward clear bomb Radius
    for (int i = 0; i <= idx; i++)
    {
      removeCoordinate(queue_y[i], queue_x[i]);
    }
  }
};

class Game
{
public:
  char state[255] = "lobby";
  char text[TOTAL_TEXT][255];
  int statusInit = 14;
  char statusText[255];
  char LOBBY_ARENA[SIZE_LOBBY_X][SIZE_LOBBY_Y];
  char GAME_ARENA[SIZE_GAME_X][SIZE_GAME_Y];
  bool forceClsFlag = false;
  bool skipBufferFlag = false;

  void skipBuffer()
  {
    // Skip buffer input, used when the return called function
    // no need to get buffer first
    skipBufferFlag = true;
  }

  void checkShopOpen()
  {
    bool flag = isSpaceshipOpen();
    if (flag)
    {
      LOBBY_ARENA[6][6] = ' ';
      LOBBY_ARENA[6][7] = ' ';
      LOBBY_ARENA[6][8] = ' ';
      LOBBY_ARENA[6][9] = ' ';
      LOBBY_ARENA[7][5] = '/';
      LOBBY_ARENA[7][10] = '\\';
    }
    else
    {
      LOBBY_ARENA[7][5] = ' ';
      LOBBY_ARENA[7][10] = ' ';
      LOBBY_ARENA[6][6] = '-';
      LOBBY_ARENA[6][7] = '-';
      LOBBY_ARENA[6][8] = '-';
      LOBBY_ARENA[6][9] = '-';
    }
  }

  void printText(int _x, int _y)
  {
    for (int j = 0; j < TOTAL_TEXT; j++)
    {
      // Move Cursor
      moveCursor(_x, _y + j);

      // Validate is text is empty_50 (Default Value)
      if (strcmp(text[j], EMPTY_50) == 0)
      {
        printf("%s", EMPTY_50);
        continue;
      }

      // If exists Print the status text

      int len = strlen(text[j]);
      for (int k = 0; k < len; k++)
      {
        playSound("typing");
        // Sleep(20);
        printf("%c", text[j][k]);
      }
    }
    resetCursor();
  }

  void changeState(char _state[255])
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
} game;

class PlayerBullet : public Bullet
{
public:
  PlayerBullet(int _x, int _y, char _symbol, int _idx, int _vx, int _vy, int _damage) : Bullet(_x, _y, _symbol, _idx, _vx, _vy, _damage){};
  bool isEnemyIntersect()
  {
    if (dead)
      return false;
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
  const int incrementEnergy = 1;
  const int maxBullets = 999;
  PlayerBullet **bullets = new PlayerBullet *[maxBullets];
  Node *lastNode = NULL;

  char statusText[255];
  int reloadTime = 1000; // milliseconds
  int reloadInterval = 0;
  int reloadMaxInterval = reloadTime / SLEEP_TIME;
  bool reloading = true;
  int skillCost = 30;
  int hp = 100;
  int maxHp = 100;
  int h = 5;
  int w = 7;
  int x = 15;
  int y = 21;
  char shooter[255][255];
  char bulletSymbol = '^';
  char skillSymbol = 'o';
  int bullet = 0;
  int currBullet = 0;
  int damage = 1;
  int type;
  int maxEnergy = 0;
  int energy = 0;
  int armor = 0;
  int clip = 0;

  Shooter(int level, int _damage, int _hp, int _energy, int _armor, int _maxBullet)
  {
    // Initiate Status Text
    strcpy(statusText, EMPTY_50);

    // Initial Variable
    maxEnergy = _energy;
    energy = _energy;
    clip = _maxBullet;
    damage = _damage;
    maxHp = _hp;
    hp = _hp;
    energy = _energy;
    armor = _armor;

    // Load Character
    char fileName[255];
    sprintf(fileName, "space_%d.txt", level);
    loadPlayer(fileName);
    type = 1;
    switch (level)
    {
    case 1:
      h = SPACE_1_H;
      w = SPACE_1_W;
      break;
    case 2:
      w = SPACE_2_W;
      h = SPACE_2_H;
      break;
    case 3:
      w = SPACE_3_W;
      h = SPACE_3_H;
      break;
    case 4:
      w = SPACE_4_W;
      h = SPACE_4_H;
      break;
    }

    calculateInitPos();
  }

  void addHp(int _hp)
  {
    hp += _hp;
    if (hp > maxHp)
    {
      hp = maxHp;
    }
  }
  void addEnergy(int _energy)
  {
    energy += _energy;
    if (energy > maxEnergy)
    {
      energy = maxEnergy;
    }
  }

  void dontHave(char str[255])
  {
    char text[255];
    sprintf(text, "You don't have any %s left!", str);
    strcpy(statusText, text);
  }

  void calculateInitPos()
  {
    x = SIZE_GAME_X - h - 3;
    y = SIZE_GAME_Y / 2 - w;
  }

  // Shooter Hit
  void hit(int damage)
  {
    // Calculate Damage with Armor Protection
    int armorProtection = randomInt(0, armor);
    damage -= armorProtection;

    hp -= damage;
    if (hp <= 0 && GAME_IS_RUNNING)
    {
      hp = 0;
      finishGame();
    }
    else
    {
      renderStatus();
    }
  }

  void saveLastNode()
  {
    lastNode = new Node(x, y, w, h);
  }

  void skill()
  {
    // Validate energy
    if (skillCost > energy)
      return;

    int totalSkillBullet = 3;
    if (bullet + totalSkillBullet < clip)
    {
      // Validate bullet less than 0 if shooted
      bullets[bullet] = new PlayerBullet(x - 1, (y + w / 2), bulletSymbol,
                                         bullet, -1, 1, damage);
      bullet += 1;
      bullets[bullet] = new PlayerBullet(x - 1, (y + w / 2), bulletSymbol,
                                         bullet, -1, 0, damage);
      bullet += 1;
      bullets[bullet] = new PlayerBullet(x - 1, (y + w / 2), bulletSymbol,
                                         bullet, -1, -1, damage);
      bullet += 1;
      energy -= skillCost;
    }
  }

  // Shooter Shoot
  void shoot()
  {
    // Beep(500, 10);
    bullets[bullet] = new PlayerBullet(x - 1, (y + w / 2), bulletSymbol,
                                       bullet, -1, 0, damage);

    bullet += 1;
  }

  void renderStatus()
  {
    // Score

    // Title
    ui->cleanText(0);
    // ui->color(0, COLOR_MAGENTA);
    ui->addText(0, "C Space Invader");

    // Status Name
    char shipText[255];
    sprintf(shipText, "%s", SPACESHIP_NAME[type - 1]);
    ui->addText(1, shipText);

    // Bullet
    ui->cleanText(4);
    char bulletText[255];
    sprintf(bulletText, "Bullets %.2d/%.2d", clip - bullet, clip);
    ui->addText(4, bulletText);

    // energy
    char energyText[255];
    sprintf(energyText, "Energy %.2d/%.2d", energy, maxEnergy);
    ui->addText(7, energyText);

    // Health
    ui->cleanText(6);
    char hpText[255];

    sprintf(hpText, "HP [%d/%d]", hp, maxHp);
    ui->addText(6, hpText);

    // Status Text
    ui->cleanText(9);
    ui->addText(9, statusText);

    // Render All
    ui->renderAll();
  }

  void removeBullet(int _idx)
  {
    removeCoordinate(bullets[_idx]->y, bullets[_idx]->x);
    bullets[_idx]->dead = true;
    // bullets[_idx] = NULL;
  }

  void reload()
  {
    strcpy(statusText, EMPTY_50);
    for (int i = 0; i < bullet; i++)
    {
      char text[255];
      bullets[i]->remove();
      delete bullets[i];
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

  // Shooter Render
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
    if (reloading)
    {
      reloadInterval += 1;
    }
    if (reloadInterval >= reloadMaxInterval)
    {
      reloading = false;
      reloadInterval = 0;
      reload();
    }

    // Add energy
    energy += incrementEnergy;
    if (energy >= maxEnergy)
    {
      energy = maxEnergy;
    }
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
    case 'q':
      finishGame();
      break;
    case 'r':
      reloading = true;
      strcpy(statusText, "Reloading ...");
      renderStatus();
      // reload();
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
      if (bullet < clip && !reloading)
      {
        shoot();
      }
      break;
    case 'f':
      if (!reloading)
      {
        skill();
      }
      break;
    case '1':
      useItem("potion");
      break;
    case '2':
      useItem("max_potion");
      break;
    case '3':
      useItem("energy");
      break;
    case '4':
      useItem("max_energy");
      break;
    case 'g':
      useItem("bomb");
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
  char type[255];
  int x;
  int y;
  char symbol;
  bool ready = false;

  NPC(int _x, int _y, char _symbol, char _type[255])
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
      game.addText("Welcome to Spaceship Station");
      game.addText("Do you want to buy any weapon ? [y\\n]");
    }
    else if (checkType("npc_item"))
    {
      game.addText("Welcome to item dealer!");
      game.addText("Do you want to buy any item ? [y\\n]");
    }
    else if (checkType("npc_upgrade"))
    {
      game.addText("Hohoo! This is an upgrade shop");
      game.addText("Any thing i can do for you ? [y\\n]");
    }
  }
};

NPC **npcs = new NPC *[10];

class Player
{
public:
  // Main Attributes
  int xp = 0;
  int level = 1;
  int money = 0;
  char name[255];
  NPC *npcAround;
  char symbol = 'P';
  int x = 19;
  int y = 17;

  // Upgrade
  int hp = 100;
  int energy = 50;
  int armor = 1;
  int damage = 1;

  // Item
  int potion = 0;
  int maxPotion = 0;
  int energyDrink = 0;
  int maxEnergyDrink = 0;
  int bomb = 0;
  // Ship
  int ship = 1;

  void reset()
  {
    xp = 0;
    level = 1;
    money = 0;
    strcpy(name, "");
    npcAround = NULL;
    x = 19;
    y = 17;
    hp = 100;
    energy = 50;
    armor = 1;
    damage = 1;
    potion = 0;
    maxPotion = 0;
    energyDrink = 0;
    maxEnergyDrink = 0;
    bomb = 0;
    ship = 1;
  }

  Player(char stringify[255])
  {
    char *data = strtok(stringify, "#");
    int idx = 0;
    while (data != NULL)
    {
      if (idx == 0)
        strcpy(name, data);
      else if (idx == 1)
        money = strtol(data, NULL, 10);
      else if (idx == 2)
        xp = strtol(data, NULL, 10);
      else if (idx == 3)
        level = strtol(data, NULL, 10);
      else if (idx == 4)
        hp = strtol(data, NULL, 10);
      else if (idx == 5)
        energy = strtol(data, NULL, 10);
      else if (idx == 6)
        armor = strtol(data, NULL, 10);
      else if (idx == 7)
        damage = strtol(data, NULL, 10);

      data = strtok(NULL, "#");
      idx += 1;
    }
  }
  void debug()
  {
    // Function for debug purposes only

    puts("DEBUG PLAYER");
    printf("%s %d %d %d %d %d %d %d %d", name, money, xp, level, hp, energy, armor, damage);
    getchar();
  }

  Player()
  {
  }

  Player(char _name[255], int _money, int _xp, int _level, int _hp, int _energy, int _armor, int _damage)
  {
    strcpy(name, _name);
    money = _money;
    xp = _xp;
    level = _level;
    hp = _hp;
    energy = _energy;
    armor = _armor;
    damage = _damage;
  }

  int gainXp(int _xp)
  {
    if (_xp > 0)
    {
      // ! Must validate score > 0
      // ! (because in the levelup calculation can be infinity)
      money += _xp;
      int levelup = (_xp + xp) / MAX_XP_TO_LEVEL_UP;
      int restXp = (levelup * MAX_XP_TO_LEVEL_UP) % _xp;
      xp = restXp;
      levelUp(levelup);
      return levelup;
    }
    return 0;
  }
  void levelUp(int _level)
  {
    level += _level;
    if (level >= MAX_PLAYER_LVL)
    {
      level = MAX_PLAYER_LVL;
    }
  }

  void openLeaderboard()
  {
    forceCls();
    myScore->print();
    printf("\n\tBack to menu ? [press enter]");
    getchar();
    forceCls();
    return;
  }

  void openBackpack()
  {
    forceCls();
    game.skipBuffer();
    printf("\n\t%s backpack's\n", name);
    printf("\t=====================\n");
    if (potion > 0)
      printf("\t- Potion : %d\n", potion);
    if (maxPotion > 0)
      printf("\t- Max Potion : %d\n", maxPotion);
    if (energyDrink > 0)
      printf("\t- Energy Drink : %d\n", energyDrink);
    if (maxEnergyDrink > 0)
      printf("\t- Max Energy Drink : %d\n", maxEnergyDrink);
    if (bomb > 0)
      printf("\t- Bomb : %d\n", bomb);

    printf("\n\n\tback to game [press enter]");
    getchar();
    return;
  }

  int printStatus(int _x, int _y)
  {
    moveCursor(_x, _y);
    printf("%s", COLOR_YELLOW);
    printf("[%s]", name);
    printf("%s", COLOR_RESET);
    moveCursor(_x, _y + 1);
    printf("Level : %d", level);
    moveCursor(_x, _y + 2);
    printf("Money : %d", money);
    resetCursor();
    return 3;
  }

  // Player Move
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
      if (isInExitPortal())
      {
        exitGame();
      }
      if (isOnLeaderboard())
      {
        openLeaderboard();
      }
      break;
    case 'y':
      if (npcAround && npcAround->ready)
        npcAround->yes();
      break;
    case 'o':
      openBackpack();
      break;
    }
  }

  bool isOnLeaderboard()
  {
    for (int i = 0; i < 3; i++)
    {
      if (x == 9 + i && y == 5)
      {
        return true;
      }
    }
    return false;
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
      else
      {
        npcAround = NULL;
      }
    }
    return false;
  }

  bool isInExitPortal()
  {
    if (x == 20 && y == 17)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool isFrontClosedDoor()
  {
    if (!isSpaceshipOpen())
    {
      // If not open validate closed door
      for (int i = 0; i < 4; i++)
      {
        if (x == 7 && y == 6 + i)
        {
          return true;
        }
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
    if (isInExitPortal())
    {
      strcpy(game.statusText, "Press SPACE to Exit!");
    }
    if (isAroundNPC())
    {
      strcpy(game.statusText, "Press SPACE To Interact");
    }
    if (isInPortal())
    {
      strcpy(game.statusText, "Press SPACE to interact");
    }
    if (isOnLeaderboard())
    {
      strcpy(game.statusText, "Press SPACE to see Leaderboard");
    }
    if (isFrontClosedDoor())
    {
      char text[255];
      sprintf(text, "You need level %d to unlock spaceship station", NEED_LEVEL_TO_OPEN_SPACESHIP);
      strcpy(game.statusText, "You need level 10 to unlock spaceship");
    }
  }
};

Player player;

class Data
{
public:
  Player **players = new Player *[255];
  int totalPlayer = 0;
  int totalAttrbNeeded = 8;
  void load()
  {
    char line[255];
    FILE *fp;
    fp = fopen("player.dat", "r");

    if (fp == NULL)
    {
      printf("Missing player.dat files ! [press enter]");
      getchar();
      exit(1);
      return;
    }

    while (fgets(line, 255, fp))
    {
      players[totalPlayer] = new Player(line);
      totalPlayer += 1;
    }
    fclose(fp);
  }

  int print(int _x, int _y)
  {
    for (int i = 0; i < totalPlayer; i++)
    {
      moveCursor(_x, _y + i);
      printf("%d. %s [Level : %d]\n", i + 1, players[i]->name, players[i]->level);
    }
    return totalPlayer;
  }

  void load(int i)
  {
    strcpy(player.name, players[i]->name);
    player.money = players[i]->money;
    player.xp = players[i]->xp;
    player.level = players[i]->level;
    player.hp = players[i]->hp;
    player.energy = players[i]->energy;
    player.armor = players[i]->armor;
    player.damage = players[i]->damage;
  }

  void save()
  {
    FILE *fp;
    fp = fopen("player.dat", "w");
    if (fp == NULL)
    {
      printf("Missing player.dat files ! [press enter]");
      getchar();
      exit(1);
      return;
    }
    for (int i = 0; i < totalPlayer; i++)
    {
      char text[255];
      sprintf(text, "%s#%d#%d#%d#%d#%d#%d#%d\n", players[i]->name, players[i]->money, players[i]->xp, players[i]->level, players[i]->hp, players[i]->energy, players[i]->armor, players[i]->damage);
      fprintf(fp, text);
    }
    fclose(fp);
  }

  bool isExistName(char name[255])
  {
    for (int i = 0; i < totalPlayer; i++)
    {
      if (strcmp(players[i]->name, name) == 0)
      {
        return true;
      }
    }
    return false;
  }

  void savePlayer()
  {
    bool isFound = false;
    for (int i = 0; i < totalPlayer; i++)
    {
      if (strcmp(players[i]->name, player.name) == 0)
      {
        isFound = true;

        // Is Found Copy the attributes
        strcpy(players[i]->name, player.name);
        players[i]->money = player.money;
        players[i]->xp = player.xp;
        players[i]->level = player.level;
        players[i]->hp = player.hp;
        players[i]->energy = player.energy;
        players[i]->armor = player.armor;
        players[i]->damage = player.damage;
      }
    }
    if (!isFound)
    {
      // Is not found make another players list
      players[totalPlayer] = new Player(player.name, player.money, player.xp, player.level, player.hp, player.energy, player.armor, player.damage);
      totalPlayer += 1;
    }

    // Save the all players to file
    save();
  }
};

void init()
{
  myScore = new Score();

  data = new Data();
  data->load();
  srand(time(0));

  game.loadLobby();
  game.loadGame();

  npcs[0] = new NPC(4, 26, 'W', "npc_weapon");
  npcs[1] = new NPC(16, 27, 'I', "npc_item");
  npcs[2] = new NPC(16, 6, 'U', "npc_upgrade");

  totalNpc = 3;
}

void loading(int _n, int _x, int _y)
{
  char symbol[4] = "/-\|";
  for (int i = 0; i < _n; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      makeCharCoordinate(_x, _y, symbol[j]);
      // Sleep(100);
      playSound("loading");
    }
  }
}

int printStartMenu(int _x, int _y)
{
  forceCls();

  moveCursor(_x, _y);
  printf("NEW GAME");
  moveCursor(_x, _y + 1);
  printf("LOAD GAME");
  moveCursor(_x, _y + 2);
  printf("SETTING");
  moveCursor(_x, _y + 3);
  printf("HOW TO PLAY");
  moveCursor(_x, _y + 4);
  printf("EXIT");
  return 5;
}

void insertNewName(int _x, int _y)
{
  forceCls();
  char name[255];
  bool isExists = false;
  do
  {
    clsCoordinate(_x, _y, 50, 3);
    moveCursor(_x, _y);
    printf("Select Your New Name : ");
    scanf("%[^\n]", name);
    getchar();
    isExists = data->isExistName(name);
    if (isExists)
    {
      moveCursor(_x, _y + 2);
      printf("Sorry but name already exists! [press enter]");
      getchar();
    }
  } while (isExists);

  // Copy to main player
  strcpy(player.name, name);
}

bool loadGame(int _x, int _y)
{
  forceCls();

  moveCursor(_x, _y);
  printf("Save Data : ");
  int max = data->print(_x, _y + 1);

  moveCursor(_x, _y + 1 + data->totalPlayer);
  printf("%d. Back", data->totalPlayer + 1);

  int idx = 0;
  int offset = 30;
  int lastIdx = idx;
  while (true)
  {
    if (lastIdx != idx)
    {
      removeCoordinate(_x + offset, _y + 1 + lastIdx);
    }
    makeCoordinate(_x + offset, _y + 1 + idx, "<");
    char buffer = getch();
    lastIdx = idx;
    playSound("choose");
    if (buffer == 'w')
      idx -= 1;
    else if (buffer == 's')
      idx += 1;
    else if (buffer == '\r')
      break;

    if (idx > max)
    {
      idx = max;
    }
    else if (idx < 0)
    {
      idx = 0;
    }
  };
  // do
  // {
  //   clsCoordinate(_x, _y + 3 + data->totalPlayer, 50, 1);
  //   moveCursor(_x, _y + 3 + data->totalPlayer);
  //   idx = scanIndex("Choose : ");
  // } while (idx < 1 || idx > data->totalPlayer + 1);

  if (idx == max)
  {
    // Back
    return false;
  }
  else
  {
    data->load(idx);
    return true;
  }
}

void chooseMenu(int _x, int _y)
{
  int max = printStartMenu(_x, _y);
  int idx = 0;
  int lastIdx = -1;
  int offsetX = 15;
  makeCoordinate(_x + offsetX, _y, "<");
  for (;;)
  {
    makeCoordinate(_x + offsetX, _y + idx, "<");
    if (lastIdx != idx)
      removeCoordinate(_x + offsetX, _y + lastIdx);
    char buffer = getch();
    playSound("choose");
    lastIdx = idx;
    if (buffer == 's')
    {
      idx += 1;
    }
    else if (buffer == 'w')
    {
      idx -= 1;
    }
    else if (buffer == '\r')
    {
      break;
    }
    if (idx <= 0)
      idx = 0;
    if (idx >= max)
      idx = max - 1;
  }
  if (idx == 0)
  {
    // Play Game With Default Character
    player.reset();
    insertNewName(_x, _y);
    theGame();
  }
  else if (idx == 1)
  {
    // Load Game
    if (loadGame(_x, _y))
    {
      theGame();
    }
    else
    {
      chooseMenu(_x, _y);
    }
  }
  else if (idx == 2)
  {
    // Setting
    forceCls();
    printSetting(_x, _y);
    printf("\n\tBack to menu ? [press enter]");
    getchar();
    chooseMenu(_x, _y);
  }
  else if (idx == 3)
  {
    // How To Play
    forceCls();
    printHowToPlay(_x, _y);
    printf("\n\tBack to menu ? [press enter]");
    getchar();
    chooseMenu(_x, _y);
  }
  else if (idx == 4)
  {
    endScreen();
  }
}

void printHowToPlay(int _x, int _y)
{
  puts("\n\n\tSpace Invader\n\n");
  puts("\tThe object of the game is, basically, to shoot the invaders with your shooter \n\twhile avoiding their shots and preventing an invasion  Amassing a high score is a further\n\tobjective and one that must be prioritised against your continued survival.");
  puts("\n\n\tThere is no time limit,\n\texcept for the fact that if you do not shoot them all before they reach\n\tthe bottom of the screen the game ends.");
  puts("\n\n\tCredit : TinTin Winata");
}

void printSetting(int _x, int _y)
{
  puts("\n\n\tWelcome to the game!");

  puts("\n\tBasic Movement");
  puts("\t'w' - Move Up");
  puts("\t'a' - Move Left");
  puts("\t's' - Move Down");
  puts("\t'd' - Move Right");

  puts("\n\tLobby Arena");

  puts("\t'SPACE' - Intersect");
  puts("\t'y' - Talk");
  puts("\t'o' - Open Backpack");
  puts("\n\tGame Arena");
  puts("\t'SPACE' - Shoot");
  puts("\t'r' - Reload");
  puts("\t'f' - Skill");
  puts("\t'1' - Use Potion");
  puts("\t'2' - Use Max Potion");
  puts("\t'3' - Use Energy");
  puts("\t'4' - Use Max Energy");
  puts("\t'g' - Use Bomb");
}

void loadingGame(int _x, int _y)
{
  forceCls();
  int initX = _x;
  int initY = _y;
  moveCursor(initX, initY);
  printf("C Space Invader");
  moveCursor(initX, initY + 1);
  printf("Loading ");
  loading(3, initX + 10, initY + 1);
  return;
}

void mainMenu(int _x, int _y)
{
  loadingGame(_x, _y);
  forceCls();
  chooseMenu(_x, _y);
}

int main()
{
  init();
  forceCls();
  mainMenu(MAIN_MENU_X, MAIN_MENU_Y);
}

void theGame()
{
  forceCls();
  render();
  while (true)
  {
    if (strcmp(game.state, "lobby") == 0)
    {
      if (!game.skipBufferFlag)
      {
        char buffer = getch();
        player.move(buffer);
        player.logic();
      }
      else
      {
        game.skipBufferFlag = false;
      }
      render();
    }
    else if (strcmp(game.state, "game") == 0)
    {
      if (GAME_IS_RUNNING)
      {
        shooter->logic();
        spawnEnemyLogic();
        renderEnemy();
        renderEnemyBullets(new Node(shooter->x, shooter->y, shooter->w, shooter->h));
        shooter->renderBullets();
        checkMovementEnemy();
        Sleep(SLEEP_TIME);
        // playSound("game");
      }

      if (kbhit())
      {
        char buffer = getch();
        shooter->move(buffer);
        shooter->render();
      }
      if (GAME_STARTING_FLAG)
      {
        playSound("start_game");
        GAME_STARTING_FLAG = false;
      }
    }
  }
}

void debug(char *str)
{
  printf("%s", str);
  getchar();
}

void finishGame()
{
  GAME_IS_RUNNING = false;
  game.changeState("lobby");
  game.nextForceCLS();
  game.skipBuffer();

  // Delete all enemies
  // totalEnemy = 0;
  // delete[] enemies;

  // Calculate Level
  int score = myScore->score;

  myScore->insert(score, player.name);
  myScore->save();

  player.gainXp(score);

  // Insert Score

  forceCls();
  printFinishGame(score, player.level);
  printf("\n\n\tContinue Game [press enter]");
  playSound("lose");
  getchar();
  return;
}

void printFinishGame(int _score, int _level)
{
  printf("\n\tC Space Invader\n");
  printf("\t------------------\n");
  printf("\tYour score : %d\n", _score);
  printf("\tYou gain %d exp\n", _score);
  printf("\tYour current level is : %d", _level);
}

void playSound(char str[255])
{
  if (strcmp(str, "start_game") == 0)
  {
    Beep(200, 500);
    Beep(300, 800);
  }
  else if (strcmp(str, "typing") == 0)
  {
    Beep(100, 20);
  }
  else if (strcmp(str, "last_show") == 0)
  {
    int random_freq = randomInt(100, 150);
    Beep(random_freq, 20);
  }
  else if (strcmp(str, "last_typing") == 0)
  {
    int random_freq = randomInt(100, 120);
    Beep(random_freq, 30);
  }
  else if (strcmp(str, "loading") == 0)
  {
    Beep(300, 20);
    Beep(500, 50);
    Beep(350, 30);
  }
  else if (strcmp(str, "choose") == 0)
  {
    Beep(200, 30);
  }
  else if (strcmp(str, "game") == 0)
  {
    int randomFreq = randomInt(10, 300);
    Beep(randomFreq, SLEEP_TIME);
  }
  else if (strcmp(str, "lose") == 0)
  {
    Beep(200, 500);
    Beep(300, 800);
  }
}

void startGame()
{
  forceCls();
  bomb = new Bomb();
  totalEnemy = 0;
  enemyBullet = 0;

  myScore->reset();

  if (shooter)
    delete shooter;
  // Reclear Shooter

  // Create new instance shooter
  shooter = new Shooter(player.ship, player.damage, player.hp, player.energy, player.armor, 10);

  game.changeState("game");

  if (ui)
    delete ui;

  ui = new UI(6, 52);
  myScore->renderUi();
  GAME_IS_RUNNING = true;
  GAME_STARTING_FLAG = true;
}

bool isSpaceshipOpen()
{
  if (player.level > NEED_LEVEL_TO_OPEN_SPACESHIP)
    return true;
  else
    return false;
}

void renderLobby()
{
  game.checkShopOpen();
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

    if (i == game.statusInit)
    {
      PRINT_EMPTY();

      // Red Background Color
      // if (strcmp(game.statusText, EMPTY_50) == 0)
      // {
      //   resetColor();
      // }
      // else
      // {
      //   printf("\u001b[41m");
      // }

      printf("%s", game.statusText);
      // resetColor();
      strcpy(game.statusText, EMPTY_50);
    }

    printf("\n");
  }

  // Print Status
  player.printStatus(LOBBY_STATUS_COORDINATE_X, 3);

  // Print Text Game
  game.printText(LOBBY_STATUS_COORDINATE_X, 11);

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
    // Spawn Enemy if already 3 seconds passed
    enemySpawnInterval = 0;

    int x = 0;
    int y = randomInt(3, SIZE_GAME_Y - ((ENEMY_WIDTH + 1) * ENEMY_SPAWN_PER_WAVE) - 2);

    for (int i = 0; i < ENEMY_SPAWN_PER_WAVE; i++)
    {
      int randomLvl = randomInt(1, 3);
      enemies[totalEnemy] = new Enemy(x, y + (i * (ENEMY_WIDTH + 1)), randomLvl, totalEnemy);
      totalEnemy += 1;
    }
  }
}

void checkMovementEnemy()
{
  enemyMovementInterval += 1;
  if (enemyMovementInterval >= maxEnemyMovementInterval)
  {
    enemyMovementInterval = 0;
    for (int i = 0; i < totalEnemy; i++)
    {
      if (enemies[i])
        enemies[i]->move();
    }
  }
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
    if (enemyBullets[i] != NULL && GAME_IS_RUNNING)
    {
      enemyBullets[i]->render();
      if (enemyBullets[i]->isPlayerIntersect(node))
      {
        shooter->hit(enemyBullets[i]->damage);
        removeEnemyBullet(i);
      }
      else if (enemyBullets[i]->isOutsideMap())
      {
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
    game.forceClsFlag = false;
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
    renderItemShop();
  }
  else if (strcmp(game.state, "npc_upgrade") == 0)
  {
    renderUpgradeShop();
  }
}

int menuUpgradeShop()
{
  printf("\n\n\tDo you need anything to upgrade ?\n");
  printf("\t1. HP %d/%d - $%d\n", player.hp, MAX_HP, UPGRADE_HP_PRICE);
  printf("\t2. Energy %d/%d - $%d\n", player.energy, MAX_ENERGY, UPGRADE_ENERY_PRICE);
  printf("\t3. Armor %d/%d - $%d\n", player.armor, MAX_ARMOR, UPGRADE_ARMOR_PRICE);
  printf("\t4. Back");
  return 4;
}

bool chooseMenuUpgrade(int _idx, int _max)
{
  moveCursor(0, _max + 2);
  switch (_idx)
  {
  case 0:
    if (chooseUpgradeMenu(UPGRADE_HP_PRICE, MAX_HP, player.hp))
    {
      player.hp += 1;
    };
    return false;
    // HP
    break;
  case 1:
    if (
        chooseUpgradeMenu(UPGRADE_ENERY_PRICE, MAX_ENERGY, player.energyDrink))
    {
      player.energy += 1;
    }
    return false;
    // Energy
    break;
  case 2:
    if (
        chooseUpgradeMenu(UPGRADE_ARMOR_PRICE, MAX_ARMOR, player.armor))
    {
      player.armor += 1;
    }
    return false;
    // Armor
    break;

  // ? don't want to add damage attribute to the game

  // case 3:
  //   if (
  //       chooseUpgradeMenu(UPGRADE_DAMAGE_PRICE, MAX_DAMAGE, player.damage))
  //   {
  //     player.damage += 1;
  //   }
  //   return false;
  //   // Damage
  //   break;
  case 3:
    printf("\n\n\tGood bye! Dont forget to came again [press enter]");
    return true;
    // Back
  }
}

void renderUpgradeShop()
{
  forceCls();
  int idx = 0;
  int max = menuUpgradeShop();
  int lastIdx = idx;
  int rightCoordinate = 45;
  int topOffset = 2;

  // Initiate '<' at first loop
  makeCoordinate(rightCoordinate, topOffset + 1 + idx, "<");

  while (true)
  {
    char buffer = getch();
    playSound("choose");
    removeCoordinate(rightCoordinate, topOffset + 1 + lastIdx);
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
      bool back = chooseMenuUpgrade(idx, max);
      if (back)
      {
        break;
      }
      else
      {
        forceCls();
        max = menuUpgradeShop();
      }
    }
    lastIdx = idx;
    makeCoordinate(rightCoordinate, topOffset + 1 + idx, "<");
  }
  game.changeState("lobby");
  game.nextForceCLS();
  return;
}

void renderItemShop()
{
  forceCls();
  int idx = 0;
  int max = menuItemShop();
  int lastIdx = idx;
  int rightCoordinate = 45;
  int topOffset = 2;

  // Initiate '<' at first loop
  makeCoordinate(rightCoordinate, topOffset + 1 + idx, "<");

  while (true)
  {
    char buffer = getch();
    playSound("choose");
    removeCoordinate(rightCoordinate, topOffset + 1 + lastIdx);
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
      chooseMenuItem(idx, max);
      break;
    }
    lastIdx = idx;
    makeCoordinate(rightCoordinate, topOffset + 1 + idx, "<");
  }
  game.changeState("lobby");
  game.nextForceCLS();
  return;
}

void chooseMenuItem(int _idx, int _max)
{
  moveCursor(0, _max + 2);
  int howMany = -1;
  switch (_idx)
  {
  case 0:
    // Potion
    howMany = choosedBuyMenu(POTION_PRICE, "Potion");
    if (howMany != -1)
    {
      player.potion += howMany;
    }
    break;
  case 1:
    howMany = choosedBuyMenu(MAX_POTION_PRICE, "Max Potion");
    if (howMany != -1)
    {
      player.maxPotion += howMany;
    }
    // Max Potion
    break;
  case 2:
    howMany = choosedBuyMenu(ENERGY_DRINK_PRICE, "Energy Drink");
    if (howMany != -1)
    {
      player.energyDrink += howMany;
    }
    // Energy Drink
    break;
  case 3:
    howMany = choosedBuyMenu(MAX_ENERGY_DRINK_PRICE, "Max Energy Drink");
    if (howMany != -1)
    {
      player.maxEnergyDrink += howMany;
    }
    // Max Energy Drink
    break;
  case 4:
    howMany = choosedBuyMenu(BOMB_PRICE, "Bomb");
    if (howMany != -1)
    {
      player.bomb += howMany;
    }
    // Bomb
    break;
  case 5:
    printf("\n\n\tSee you again! [press enter]");
    break;
  }
  return;
}

int menuItemShop()
{
  printf("\n\n\tWelcome to the item shop\n");
  printf("\t1. Potion [$%d]\n", POTION_PRICE);
  printf("\t2. Max Potion [$%d]\n", MAX_POTION_PRICE);
  printf("\t3. Energy Drink [$%d]\n", ENERGY_DRINK_PRICE);
  printf("\t4. Max Energy Drink [$%d]\n", MAX_ENERGY_DRINK_PRICE);
  printf("\t5. Bomb [$%d]\n", BOMB_PRICE);
  printf("\t6. Back");
  return 6;
}

void renderWeaponShop()
{
  int idx = 0;
  while (true)
  {
    forceCls();
    menuWeaponShop(idx);
    char buffer = getch();
    playSound("choose");

    if (buffer == 'a')
      idx -= 1;
    else if (buffer == 'd')
      idx += 1;
    else if (buffer == '\r')
      break;

    if (idx < 0)
      idx = TOTAL_SPACE - 1;
    else if (idx >= TOTAL_SPACE)
      idx = 0;
  }
  player.ship = idx + 1;
  printf("\n\tSpaceship Choosed [press enter]");

  game.changeState("lobby");
  game.nextForceCLS();
  return;
}

void printSpace(int _idx)
{
  char item[255][255];

  int w, h;
  switch (_idx)
  {
  case 0:
    w = SPACE_1_W;
    h = SPACE_1_H;
    arraySetSpace(item, w, h);
    loadSprite("space_1.txt", item);
    printf("\t   %s\n\n", SPACESHIP_NAME[_idx]);
    break;
  case 1:
    w = SPACE_2_W;
    h = SPACE_2_H;
    arraySetSpace(item, w, h);
    printf("\t    %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_2.txt", item);
    break;
  case 2:
    w = SPACE_3_W;
    h = SPACE_3_H;
    arraySetSpace(item, w, h);
    printf("\t  %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_3.txt", item);
    break;
  case 3:
    w = SPACE_4_W;
    h = SPACE_4_H;
    arraySetSpace(item, w, h);
    printf("\t    %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_4.txt", item);
    break;
  }
  for (int i = 0; i < h; i++)
  {
    printf("\t\t");
    for (int j = 0; j < w; j++)
    {
      printf("%c", item[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void menuWeaponShop(int _idx)
{
  printf("\n\n\tWelcome to the station! \n\t  Select your spaceship!\n\n");
  printSpace(_idx);
  printf("\t[press enter to choose] \n");
  printf("\t< ------------------- > \n");
  return;
}

void forceCls()
{
  // cls();
  // clrscr();
  // printf("SYSTEM CLEAR");
  system("cls");

  // printf("TERCLEAR");
  // resetCursor();
  // for (int i = 0; i < 80; i++)
  // printf("%s", EMPTY_50);
  // resetCursor();
}

void clsCoordinate(int _x, int _y, int _w, int _h)
{
  for (int i = 0; i < _h; i++)
  {
    moveCursor(_x, _y + i);
    for (int j = 0; j < _w; j++)
    {
      printf(" ");
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

void makeCharCoordinate(int _x, int _y, char _symbol)
{
  moveCursor(_x, _y);
  printf("%c", _symbol);
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

void successBuyStatus(int _howMany, char _name[255])
{
  printf("\n\n\tSuccesfully Purchased %d %s! [press enter]", _howMany, _name);
  return;
}

void dontHaveMoneyStatus()
{
  printf("\n\n\tYou dont have enough money! [press enter]");
  return;
}

int scanIndex(char str[255])
{
  int temp;
  printf("%s", str);
  scanf("%d", &temp);
  getchar();
  return temp;
}

int choosedBuyMenu(int _price, char _name[255])
{
  char text[255];
  snprintf(text, sizeof(text), "\n\n\tHow many %s do you want to buy ?\n\t- ", _name);
  int howMany = scanIndex(text);
  int totalPrice = howMany * _price;
  if (totalPrice > player.money)
  {
    // Dont have money
    dontHaveMoneyStatus();
    return -1;
  }
  else
  {
    // Buy
    successBuyStatus(howMany, _name);
    player.money -= totalPrice;
    return howMany;
  }
}

bool chooseUpgradeMenu(int _price, int _max, int _base)
{
  if (_base >= _max)
  {
    printf("\n\n\tYou already at the max levels [press enter]\n");
    getchar();
  }
  else if (_price > player.money)
  {
    printf("\n\n\tYou don't have enough money! [press enter]\n");
    getchar();
  }
  else
  {
    printf("\n\n\tSuccesfully upgraded skills [press enter]\n");
    getchar();
    player.money -= _price;
    return true;
  }
  return false;
}

void arraySetSpace(char _arr[255][255], int _w, int _h)
{
  for (int i = 0; i < _h; i++)
  {
    for (int j = 0; j < _w; j++)
    {
      _arr[i][j] = ' ';
    }
  }
}

void endScreen()
{
  forceCls();
  FILE *fptr;
  char c;
  fptr = fopen("logo.txt", "r");
  if (fptr == NULL)
  {
    printf("Cannot open file \n");
    exit(0);
  }
  c = fgetc(fptr);
  while (c != EOF)
  {
    if (c == '\n')
    {
      playSound("last_show");
      // Sleep(20);
    }
    printf("%c", c);
    c = fgetc(fptr);
  }
  fclose(fptr);
  char text[] = {"\n\n\tAlongside courage and perseverance, we shape and define our future.\n\tJT 22-1"};
  int length = strlen(text);
  for (int i = 0; i < length; i++)
  {
    playSound("last_typing");
    printf("%c", text[i]);
  }
  getch();
  exit(1);
}

void exitGame()
{
  data->savePlayer();
  forceCls();
  chooseMenu(MAIN_MENU_X, MAIN_MENU_Y);
  endScreen();
}

void useItem(char str[255])
{
  if (strcmp(str, "potion") == 0)
  {
    if (player.potion > 0)
    {
      shooter->addHp(POTION_HP_INCREMENT);
      player.potion -= 1;
      strcpy(shooter->statusText, "Used Potion!");
      shooter->renderStatus();
    }
    else
    {
      shooter->dontHave("potion");
    }
  }
  else if (strcmp(str, "max_potion") == 0)
  {
    if (player.maxPotion > 0)
    {
      shooter->hp = shooter->maxHp;
      player.maxPotion -= 1;
      strcpy(shooter->statusText, "Used Max Potion!");
      shooter->renderStatus();
    }
    else
    {
      shooter->dontHave("max potion");
    }
  }
  else if (strcmp(str, "energy") == 0)
  {
    if (player.energyDrink > 0)
    {

      shooter->addEnergy(ENERGY_DRINK_INCREMENT);
      player.energyDrink -= 1;
      strcpy(shooter->statusText, "Used Energy Drink!");
      shooter->renderStatus();
    }
    else
    {
      shooter->dontHave("energy drink");
    }
  }
  else if (strcmp(str, "max_energy") == 0)
  {
    if (player.maxEnergyDrink > 0)
    {
      shooter->energy = shooter->maxEnergy;
      player.maxEnergyDrink -= 1;
      strcpy(shooter->statusText, "Max Energy Drink!");
      shooter->renderStatus();
    }
    else
    {
      shooter->dontHave("max energy drink");
    }
  }
  else if (strcmp(str, "bomb") == 0)
  {
    if (player.bomb > 0)
    {

      bomb->bomb(shooter->x, shooter->y);
      bomb -= 1;
      strcpy(shooter->statusText, "BOMB!");
      shooter->renderStatus();
    }
    else
    {
      shooter->dontHave("bomb");
    }
  }
}

void resetColor()
{
  // Reset Color Background
  printf("\033[0m");

  // Reset White Color
  printf(COLOR_RESET);
}