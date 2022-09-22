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
const int MAX_BULLET = 255;
const int LOBBY_STATUS_COORDINATE_X = SIZE_LOBBY_Y + 4;
const int ENEMY_COOLDOWN = 2000; // milliseconds
const int ENEMY_MOVEMENT_INTERVAL = 3000;
const int ENEMY_SPAWN_PER_WAVE = 3;
const int ENEMY_SHOOT_INTERVAL = 2000;
const int MAX_XP_TO_LEVEL_UP = 100;
const int MAX_PLAYER_LVL = 100;

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

// Function Prototype List
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

int SLEEP_TIME = 20;

int totalEnemy = 0;
int totalNpc = 0;
int enemySpawnInterval = 0;
int maxEnemySpawnInterval = ENEMY_COOLDOWN / SLEEP_TIME;
int enemyBullet = 0;
int enemyMovementInterval = 0;
int maxEnemyMovementInterval = ENEMY_MOVEMENT_INTERVAL / SLEEP_TIME;
bool GAME_IS_RUNNING = false;

Enemy **enemies = new Enemy *[MAX_ENEMY];
UI *ui = NULL;
Score *myScore = NULL;
Shooter *shooter = NULL;
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

  int shootInterval = 0;
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
      enemies[idx] = NULL;
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
  int statusInit = 14;
  char statusText[255] = "";
  char LOBBY_ARENA[SIZE_LOBBY_X][SIZE_LOBBY_Y];
  char GAME_ARENA[SIZE_GAME_X][SIZE_GAME_Y];
  bool forceClsFlag = false;

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

      // If exists Print the text
      int len = strlen(text[j]);
      for (int k = 0; k < len; k++)
      {
        Sleep(20);
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
  const int maxBullets = 10;
  PlayerBullet **bullets = new PlayerBullet *[maxBullets];
  Node *lastNode = NULL;

  int hp = 100;
  int maxHp = 100;
  int h = 5;
  int w = 7;
  int x = 15;
  int y = 21;
  char shooter[255][255];
  char bulletSymbol = '^';
  int bullet = 0;
  int damage = 1;
  int type;

  Shooter(int level)
  {
    char fileName[255];
    sprintf(fileName, "space_%d.txt", level);
    loadPlayer(fileName);
    type = level - 1;
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

  void calculateInitPos()
  {
    x = SIZE_GAME_X - h - 3;
    y = SIZE_GAME_Y / 2 - w;
  }

  void hit(int damage)
  {
    hp -= damage;
    if (hp <= 0)
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

  void shoot()
  {
    bullets[bullet] = new PlayerBullet(x, (y + w / 2), bulletSymbol,
                                       bullet, -1, 0, damage);

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

    // Status Name
    char shipText[255];
    sprintf(shipText, "%s", SPACESHIP_NAME[type - 1]);
    ui->addText(3, shipText);

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
    bullets[_idx]->dead = true;
    // bullets[_idx] = NULL;
  }

  void reload()
  {
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
      game.addText("Welcome to Weapon Dealer");
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
  // Main Attributess
  int xp = 0;
  int level = 1;
  int money = 0;
  char name[255] = "Justine";
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

  int giveXp(int _xp)
  {
    int lvl = _xp / MAX_XP_TO_LEVEL_UP;
    xp = _xp % lvl;

    level += lvl;
    if (level >= MAX_PLAYER_LVL)
    {
      level = MAX_PLAYER_LVL;
    }
    return level;
  }

  void openBackpack()
  {
    forceCls();
    printf("%s backpack's\n", name);
    printf("=====================\n");
    if (potion > 0)
      printf("- Potion : %d\n", potion);
    if (maxPotion > 0)
      printf("- Max Potion : %d\n", maxPotion);
    if (energyDrink > 0)
      printf("- Energy Drink : %d\n", energyDrink);
    if (maxEnergyDrink > 0)
      printf("- Max Energy Drink : %d\n", maxEnergyDrink);
    if (bomb > 0)
      printf("- Bomb : %d\n", bomb);

    printf("\n back to game [press enter]");
    return;
  }

  int printStatus(int _x, int _y)
  {
    moveCursor(_x, _y);
    printf("[%s]", name);
    moveCursor(_x, _y + 1);
    printf("Level : %d", level);
    moveCursor(_x, _y + 2);
    printf("Money : %d", money);
    // moveCursor(_x, _y + 3);
    // printf("Max HP : %d", hp);
    // moveCursor(_x, _y + 4);
    // printf("Max Energy : %d", energy);
    // moveCursor(_x, _y + 5);
    // printf("Max Armor : %d", armor);
    // moveCursor(_x, _y + 6);
    // printf("Damage : %d", damage);
    resetCursor();
    return 3;
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
    case 'o':
      openBackpack();
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
      else
      {
        npcAround = NULL;
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

  npcs[0] = new NPC(4, 26, 'W', "npc_weapon");
  npcs[1] = new NPC(16, 27, 'I', "npc_item");
  npcs[2] = new NPC(16, 6, 'U', "npc_upgrade");

  totalNpc = 3;

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
        shooter->move(buffer);
        shooter->logic();
      }

      if (GAME_IS_RUNNING)
      {
        spawnEnemyLogic();
        renderEnemy();
        renderEnemyBullets(new Node(shooter->x, shooter->y, shooter->w, shooter->h));
        shooter->renderBullets();
        checkMovementEnemy();
        Sleep(SLEEP_TIME);
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

  // Delete all enemies
  totalEnemy = 0;

  // delete[] enemies;

  // Calculate Level
  int score = myScore->score;
  player.money += score;
  // int lvl = player.giveXp(score);
  // delete myScore;
  forceCls();
  printFinishGame(score, 30);
  getchar();

  game.changeState("lobby");
  render();
}

void printFinishGame(int _score, int _level)
{
  printf("C Space Invader\n");
  printf("------------------\n");
  printf("Your score : %d\n", _score);
  printf("Your current level : %d\n", _level);
  printf("\n[press enter]\n");
}

void startGame()
{
  forceCls();

  if (myScore)
    delete myScore;

  myScore = new Score();

  if (shooter)
    delete shooter;
  // Reclear Shooter

  // Create new instance shooter
  shooter = new Shooter(player.ship);

  game.changeState("game");

  if (ui)
    delete ui;

  ui = new UI(8, 52);
  myScore->renderUi();
  GAME_IS_RUNNING = true;
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

    if (i == game.statusInit)
    {
      PRINT_EMPTY();
      printf("%s", game.statusText);
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
    // Spawn Enemy already 3 seconds
    enemySpawnInterval = 0;

    int x = 0;
    int y = randomInt(3, SIZE_GAME_Y - (ENEMY_WIDTH * ENEMY_SPAWN_PER_WAVE) - 2);

    for (int i = 0; i < ENEMY_SPAWN_PER_WAVE; i++)
    {
      int randomLvl = randomInt(1, 3);
      enemies[totalEnemy] = new Enemy(x, y + (i * ENEMY_SPAWN_PER_WAVE), randomLvl, totalEnemy);
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
    if (enemyBullets[i] != NULL)
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
  printf("Do you need anything to upgrade ?\n");
  printf("1. HP %d/%d - $%d\n", player.hp, MAX_HP, UPGRADE_HP_PRICE);
  printf("2. Energy %d/%d - $%d\n", player.energy, MAX_ENERGY, UPGRADE_ENERY_PRICE);
  printf("3. Armor %d/%d - $%d\n", player.armor, MAX_ARMOR, UPGRADE_ARMOR_PRICE);
  printf("4. Damage %d/%d - $%d\n", player.damage, MAX_DAMAGE, UPGRADE_DAMAGE_PRICE);
  printf("5. Back");
  return 5;
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
  case 3:
    if (
        chooseUpgradeMenu(UPGRADE_DAMAGE_PRICE, MAX_DAMAGE, player.damage))
    {
      player.damage += 1;
    }
    return false;
    // Damage
    break;
  case 4:
    printf("Good bye! Dont forget to came again [press enter]");
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
  int rightCoordinate = 30;

  // Initiate '<' at first loop
  makeCoordinate(rightCoordinate, 1 + idx, "<");

  while (true)
  {
    char buffer = getch();
    removeCoordinate(rightCoordinate, 1 + lastIdx);
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
    makeCoordinate(rightCoordinate, 1 + idx, "<");
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
  int rightCoordinate = 30;

  // Initiate '<' at first loop
  makeCoordinate(rightCoordinate, 1 + idx, "<");

  while (true)
  {
    char buffer = getch();
    removeCoordinate(rightCoordinate, 1 + lastIdx);
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
    makeCoordinate(rightCoordinate, 1 + idx, "<");
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
      player.potion += 1;
    }
    break;
  case 1:
    howMany = choosedBuyMenu(MAX_POTION_PRICE, "Max Potion");
    if (howMany != -1)
    {
      player.maxPotion += 1;
    }
    // Max Potion
    break;
  case 2:
    howMany = choosedBuyMenu(ENERGY_DRINK_PRICE, "Energy Drink");
    if (howMany != -1)
    {
      player.potion += 1;
    }
    // Energy Drink
    break;
  case 3:
    howMany = choosedBuyMenu(MAX_ENERGY_DRINK_PRICE, "Max Energy Drink");
    if (howMany != -1)
    {
      player.potion += 1;
    }
    // Max Energy Drink
    break;
  case 4:
    howMany = choosedBuyMenu(BOMB_PRICE, "Bomb");
    if (howMany != -1)
    {
      player.potion += 1;
    }
    // Bomb
    break;
  case 5:
    printf("\nSee you again! [press enter]");
    break;
  }
  return;
}

int menuItemShop()
{
  printf("Welcome to the item shop\n");
  printf("1. Potion [$%d]\n", POTION_PRICE);
  printf("2. Max Potion [$%d]\n", MAX_POTION_PRICE);
  printf("3. Energy Drink [$%d]\n", ENERGY_DRINK_PRICE);
  printf("4. Max Energy Drink [$%d]\n", MAX_ENERGY_DRINK_PRICE);
  printf("5. Bomb [$%d]\n", BOMB_PRICE);
  printf("6. Back");
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
  printf("\nSpaceship Choosed [press enter]");

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
    printf("   %s\n\n", SPACESHIP_NAME[_idx]);
    break;
  case 1:
    w = SPACE_2_W;
    h = SPACE_2_H;
    arraySetSpace(item, w, h);
    printf("    %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_2.txt", item);
    break;
  case 2:
    w = SPACE_3_W;
    h = SPACE_3_H;
    arraySetSpace(item, w, h);
    printf("  %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_3.txt", item);
    break;
  case 3:
    w = SPACE_4_W;
    h = SPACE_4_H;
    arraySetSpace(item, w, h);
    printf("    %s\n\n", SPACESHIP_NAME[_idx]);
    loadSprite("space_4.txt", item);
    break;
  }
  for (int i = 0; i < h; i++)
  {
    printf("\t");
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
  printf(" Welcome to the shop! \nSelect your battleship!\n\n");
  printSpace(_idx);
  printf("[press enter to choose] \n");
  printf("< ------------------- > \n");
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

void successBuyStatus(int _howMany, char _name[255])
{
  printf("Succesfully Purchased %d %s! [press enter]", _howMany, _name);
  return;
}

void dontHaveMoneyStatus()
{
  printf("You dont have enough money! [press enter]");
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
  snprintf(text, sizeof(text), "How many %s do you want to buy ?\n- ", _name);
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
    printf("You already at the max levels [press enter]\n");
    getchar();
  }
  if (_price > player.money)
  {
    printf("You don't have enough money! [press enter]\n");
    getchar();
  }
  else
  {
    printf("Succesfully upgraded skills [press enter]\n");
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