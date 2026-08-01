#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"
#include "Boss.h"
#include "Meteor.h"

class Game
{
private:
    static const int WIDTH = 40;
    static const int HEIGHT = 18;

    static const int NORMAL_ENEMY_COUNT = 10;

    Player player;
    Boss boss;

    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Item> items;
    std::vector<Meteor> meteors;

    std::string getItemDisplayText(const Item& item) const;
    int getItemDisplayWidth(const Item& item) const;

    std::chrono::steady_clock::time_point bossStartTime;
    double bossBattleTime;

    // í èÌìGÇÃèoåªêî
    int spawnedEnemyCount;

    // í èÌìGÇÃåÇîjêî
    int defeatedEnemyCount;

    // äeéÌÉJÉEÉìÉ^
    int shotCounter;
    int moveCounter;
    int bulletCounter;

    int enemyMoveCounter;
    int itemMoveCounter;
    int spawnCounter;

    int bossMoveCounter;
    int meteorSpawnCounter;
    int meteorMoveCounter;

    bool bossBattle;
    bool gameClear;

    void input();
    void update();

    void updatePlayer();
    void updateShooting();
    void updateBullets();

    void spawnNormalEnemy();
    void updateEnemies();

    void startBossBattle();
    void updateBoss();

    void createMeteor();
    void updateMeteors();

    void updateItems();

    void checkBulletEnemyCollision();
    void checkBulletBossCollision();

    void checkEnemyPlayerCollision();
    void checkMeteorPlayerCollision();
    void checkItemPlayerCollision();

    void createItem(int x, int y);

    void removeOutOfScreenBullets();
    void removeInactiveObjects();

    void draw();
    void drawGameOver();
    void drawGameClear();

    void setCursorPosition(int x, int y);
    void hideCursor();

public:
    Game();

    void run();
};