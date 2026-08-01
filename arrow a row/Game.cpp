#include "Game.h"
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

Game::Game()
    : player(WIDTH / 2, HEIGHT - 2)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    spawnedEnemyCount = 0;
    defeatedEnemyCount = 0;

    shotCounter = 0;
    moveCounter = 0;
    bulletCounter = 0;

    enemyMoveCounter = 0;
    itemMoveCounter = 0;
    spawnCounter = 0;

    bossMoveCounter = 0;
    meteorSpawnCounter = 0;
    meteorMoveCounter = 0;

    bossBattle = false;
    gameClear = false;
}

void Game::run()
{
    hideCursor();
    system("cls");

    while (player.getHP() > 0 && !gameClear)
    {
        input();
        update();
        draw();

        Sleep(20);
    }

    if (gameClear)
    {
        drawGameClear();
    }
    else
    {
        drawGameOver();
    }

    cout << endl;
    cout << "Enterキーを押すと終了" << endl;

    // Enterキーが押されるまで待つ
    while (true)
    {
        int key = _getch();

        if (key == 13)  // Enterキー
        {
            break;
        }
    }
}

void Game::input()
{
    updatePlayer();
}

std::string Game::getItemDisplayText(const Item& item) const
{
    if (item.type == 'D')
    {
        return "攻撃力+1";
    }

    if (item.type == 'F')
    {
        return "発射速度+1";
    }

    return "";
}

int Game::getItemDisplayWidth(const Item& item) const
{
    if (item.type == 'D')
    {
        /*
        攻撃力は全角3文字
        3文字 × 2マス = 6マス

        +1は半角2文字なので2マス

        合計8マス
        */
        return 8;
    }

    if (item.type == 'F')
    {
        /*
        発射速度は全角2文字
        4文字 × 2マス = 8マス

        +1は半角2文字なので2マス

        合計10マス
        */
        return 10;
    }

    return 1;
}

void Game::updatePlayer()
{
    moveCounter++;

    if (moveCounter < 3)
    {
        return;
    }

    bool leftPressed =
        (GetAsyncKeyState('A') & 0x8000) != 0;

    bool rightPressed =
        (GetAsyncKeyState('D') & 0x8000) != 0;

    if (leftPressed)
    {
        player.moveLeft();
    }

    if (rightPressed)
    {
        player.moveRight(WIDTH);
    }

    moveCounter = 0;
}

void Game::update()
{
    // 通常敵戦
    if (!bossBattle)
    {
        spawnNormalEnemy();
        updateEnemies();

        checkBulletEnemyCollision();
        checkEnemyPlayerCollision();

        /*
        10体すべて出現済みで、
        10体すべて撃破したらボス戦へ移行
        */
        if (spawnedEnemyCount >= NORMAL_ENEMY_COUNT &&
            defeatedEnemyCount >= NORMAL_ENEMY_COUNT)
        {
            startBossBattle();
        }
    }
    // ボス戦
    else
    {
        updateBoss();
        updateMeteors();

        checkBulletBossCollision();
        checkMeteorPlayerCollision();

        if (boss.isActive() && !boss.isAlive())
        {
            gameClear = true;

            auto endTime = std::chrono::steady_clock::now();

            bossBattleTime =
                std::chrono::duration<double>(
                    endTime - bossStartTime
                ).count();
        }
    }

    updateShooting();
    updateBullets();

    updateItems();
    checkItemPlayerCollision();

    removeOutOfScreenBullets();
    removeInactiveObjects();
}

void Game::updateShooting()
{
    shotCounter++;

    if (shotCounter >= player.getShotInterval())
    {
        Bullet bullet;

        bullet.x = player.getX();
        bullet.y = player.getY() - 1;

        bullets.push_back(bullet);

        shotCounter = 0;
    }
}

void Game::updateBullets()
{
    bulletCounter++;

    // 約80ミリ秒ごとに上へ1マス移動
    if (bulletCounter >= 4)
    {
        for (Bullet& bullet : bullets)
        {
            bullet.y--;
        }

        bulletCounter = 0;
    }
}

// ========================================
// 通常敵
// ========================================

void Game::spawnNormalEnemy()
{
    if (spawnedEnemyCount >= NORMAL_ENEMY_COUNT)
    {
        return;
    }

    spawnCounter++;

    // 約1秒ごとに1体出現
    if (spawnCounter < 50)
    {
        return;
    }

    // 左右の端を少し空けたランダム位置
    int randomX = rand() % (WIDTH - 4) + 2;

    // HPは3～10
    int randomHP = rand() % 8 + 3;

    Enemy enemy(randomX, 1, randomHP);

    enemy.spawn();

    enemies.push_back(enemy);

    spawnedEnemyCount++;

    spawnCounter = 0;
}

void Game::updateEnemies()
{
    enemyMoveCounter++;

    // 約1秒ごとに下へ1マス移動
    if (enemyMoveCounter < 50)
    {
        return;
    }

    for (Enemy& enemy : enemies)
    {
        if (enemy.isAlive() &&
            enemy.isSpawned())
        {
            enemy.moveDown();
        }
    }

    enemyMoveCounter = 0;
}

void Game::checkBulletEnemyCollision()
{
    for (Enemy& enemy : enemies)
    {
        if (!enemy.isAlive() ||
            !enemy.isSpawned())
        {
            continue;
        }

        for (int b = 0;
            b < static_cast<int>(bullets.size());)
        {
            if (bullets[b].x == enemy.getX() &&
                bullets[b].y == enemy.getY())
            {
                enemy.takeDamage(
                    player.getArrowDamage()
                );

                bullets.erase(
                    bullets.begin() + b
                );

                if (!enemy.isAlive())
                {
                    defeatedEnemyCount++;

                    createItem(
                        enemy.getX(),
                        enemy.getY()
                    );

                    break;
                }
            }
            else
            {
                b++;
            }
        }
    }
}

void Game::checkEnemyPlayerCollision()
{
    for (Enemy& enemy : enemies)
    {
        if (!enemy.isAlive() ||
            !enemy.isSpawned())
        {
            continue;
        }

        // 敵とプレイヤーが接触
        if (enemy.getX() == player.getX() &&
            enemy.getY() >= player.getY())
        {
            player.takeDamage(20);

            /*
            接触した敵は倒した扱いにする。

            この加算がないと、接触で消えた敵がいる場合に
            ボスが出現しなくなるため。
            */
            enemy.destroy();
            defeatedEnemyCount++;
        }
        // プレイヤーを通過した敵
        else if (enemy.getY() >= HEIGHT)
        {
            /*
            通過した敵も消滅し、処理上は終了した敵として数える。

            「10体倒したらボス」という条件を厳密に
            撃破だけにしたい場合は、この加算を削除してください。
            */
            enemy.destroy();
            defeatedEnemyCount++;
        }
    }
}

// ========================================
// ボス
// ========================================

void Game::startBossBattle()
{
    bossBattle = true;

    bullets.clear();
    items.clear();

    boss.appear(
        WIDTH / 2,
        2
    );

    bossMoveCounter = 0;
    meteorSpawnCounter = 0;
    meteorMoveCounter = 0;

    bossStartTime = std::chrono::steady_clock::now();
}

void Game::updateBoss()
{
    if (!boss.isActive() ||
        !boss.isAlive())
    {
        return;
    }

    bossMoveCounter++;

    // 約300ミリ秒ごとにランダム移動
    if (bossMoveCounter >= 15)
    {
        boss.moveRandom(WIDTH);

        bossMoveCounter = 0;
    }

    meteorSpawnCounter++;

    // 約1秒ごとに隕石を生成
    if (meteorSpawnCounter >= 20)
    {
        createMeteor();

        meteorSpawnCounter = 0;
    }
}

void Game::checkBulletBossCollision()
{
    if (!boss.isActive() ||
        !boss.isAlive())
    {
        return;
    }

    for (int b = 0;
        b < static_cast<int>(bullets.size());)
    {
        if (bullets[b].y == boss.getY() &&
            bullets[b].x >= boss.getX() &&
            bullets[b].x < boss.getX() + boss.getWidth())
        {
            boss.takeDamage(
                player.getArrowDamage()
            );

            bullets.erase(
                bullets.begin() + b
            );
        }
        else
        {
            b++;
        }
    }
}

// ========================================
// 隕石
// ========================================

void Game::createMeteor()
{
    Meteor meteor;

    /*
    ボスの位置を中心として、
    左右4マス程度の場所へランダムに落とす
    */
    int offset = rand() % 9 - 4;

    meteor.x =
        boss.getX()
        + rand() % boss.getWidth();

    if (meteor.x < 0)
    {
        meteor.x = 0;
    }

    if (meteor.x >= WIDTH)
    {
        meteor.x = WIDTH - 1;
    }

    meteor.y = boss.getY() + 1;
    meteor.active = true;

    meteors.push_back(meteor);
}

void Game::updateMeteors()
{
    meteorMoveCounter++;

    // 約100ミリ秒ごとに下へ1マス
    if (meteorMoveCounter < 5)
    {
        return;
    }

    for (Meteor& meteor : meteors)
    {
        if (meteor.active)
        {
            meteor.y++;
        }
    }

    meteorMoveCounter = 0;
}

void Game::checkMeteorPlayerCollision()
{
    for (Meteor& meteor : meteors)
    {
        if (!meteor.active)
        {
            continue;
        }

        // プレイヤーと接触
        if (meteor.x == player.getX() &&
            meteor.y >= player.getY())
        {
            player.takeDamage(20);
            meteor.active = false;
        }
        // 画面外へ出た
        else if (meteor.y >= HEIGHT)
        {
            meteor.active = false;
        }
    }
}

// ========================================
// アイテム
// ========================================

void Game::createItem(int enemyX, int enemyY)
{
    Item item;

    item.y = enemyY;
    item.active = true;

    if (rand() % 2 == 0)
    {
        item.type = 'D';
    }
    else
    {
        item.type = 'F';
    }

    int itemWidth = getItemDisplayWidth(item);

    /*
    敵の位置がアイテムの中央になるように、
    アイテムの左端を決める
    */
    item.x = enemyX - itemWidth / 2;

    // 左端からはみ出さないようにする
    if (item.x < 0)
    {
        item.x = 0;
    }

    // 右端からはみ出さないようにする
    if (item.x + itemWidth > WIDTH)
    {
        item.x = WIDTH - itemWidth;
    }

    items.push_back(item);
}

void Game::updateItems()
{
    itemMoveCounter++;

    // 約200ミリ秒ごとに下へ1マス
    if (itemMoveCounter < 10)
    {
        return;
    }

    for (Item& item : items)
    {
        if (item.active)
        {
            item.y++;
        }
    }

    itemMoveCounter = 0;
}

void Game::checkItemPlayerCollision()
{
    for (Item& item : items)
    {
        if (!item.active)
        {
            continue;
        }

        int itemLeft = item.x;
        int itemRight =
            item.x + getItemDisplayWidth(item) - 1;

        /*
        プレイヤーがアイテム文字列の横幅内にいて、
        アイテムがプレイヤーの高さまで来たら取得
        */
        bool hitX =
            player.getX() >= itemLeft &&
            player.getX() <= itemRight;

        bool hitY =
            item.y >= player.getY();

        if (hitX && hitY)
        {
            if (item.type == 'D')
            {
                player.powerUpDamage();
            }
            else if (item.type == 'F')
            {
                player.powerUpFireRate();
            }

            item.active = false;
        }
        else if (item.y >= HEIGHT)
        {
            item.active = false;
        }
    }
}

// ========================================
// 不要なデータの削除
// ========================================

void Game::removeOutOfScreenBullets()
{
    bullets.erase(
        remove_if(
            bullets.begin(),
            bullets.end(),
            [](const Bullet& bullet)
            {
                return bullet.y < 0;
            }
        ),
        bullets.end()
    );
}

void Game::removeInactiveObjects()
{
    items.erase(
        remove_if(
            items.begin(),
            items.end(),
            [](const Item& item)
            {
                return !item.active;
            }
        ),
        items.end()
    );

    meteors.erase(
        remove_if(
            meteors.begin(),
            meteors.end(),
            [](const Meteor& meteor)
            {
                return !meteor.active;
            }
        ),
        meteors.end()
    );
}

// ========================================
// 描画
// ========================================

void Game::draw()
{
    setCursorPosition(0, 0);

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            bool drawn = false;

            // ボス
            if (!drawn &&
                boss.isActive() &&
                boss.isAlive() &&
                y == boss.getY() &&
                x == boss.getX())
            {
                cout << boss.getDisplayText();

                drawn = true;

                x += boss.getWidth() - 1;
            }

            // 通常敵
            if (!drawn && !bossBattle)
            {
                for (const Enemy& enemy : enemies)
                {
                    if (enemy.isAlive() &&
                        enemy.isSpawned() &&
                        enemy.getX() == x &&
                        enemy.getY() == y)
                    {
                        cout << enemy.getHP();

                        drawn = true;
                        break;
                    }
                }
            }

            // 隕石
            if (!drawn)
            {
                for (const Meteor& meteor : meteors)
                {
                    if (meteor.active &&
                        meteor.x == x &&
                        meteor.y == y)
                    {
                        cout << "〇";

                        drawn = true;
                        break;
                    }
                }
            }

            // アイテム
            // アイテム
            if (!drawn)
            {
                for (const Item& item : items)
                {
                    if (item.active &&
                        item.x == x &&
                        item.y == y)
                    {
                        cout << getItemDisplayText(item);

                        /*
                        表示した文字列の横幅分だけ、
                        描画ループのxを先に進める
                        */
                        x += getItemDisplayWidth(item) - 1;

                        drawn = true;
                        break;
                    }
                }
            }

            // 弾
            if (!drawn)
            {
                for (const Bullet& bullet : bullets)
                {
                    if (bullet.x == x &&
                        bullet.y == y)
                    {
                        cout << "|";

                        drawn = true;
                        break;
                    }
                }
            }

            // プレイヤー
            if (!drawn &&
                player.getX() == x &&
                player.getY() == y)
            {
                cout << "△";
                drawn = true;
            }

            // 空白
            if (!drawn)
            {
                cout << " ";
            }
        }

        cout << endl;
    }

    cout << endl;

    cout << "HP : "
        << player.getHP()
        << " / 100"
        << "                    "
        << endl;

    cout << "攻撃力 : レベル"
        << player.getAttackLevel()
        << "                    "
        << endl;

    cout << "発射速度 : レベル"
        << player.getFireLevel()
        << "                    "
        << endl;

    /*
    if (!bossBattle)
    {
        cout << "通常敵 : "
            << defeatedEnemyCount
            << " / "
            << NORMAL_ENEMY_COUNT
            << "                    "
            << endl;

        cout << "次の目標 : 通常敵を10体倒す"
            << "                    "
            << endl;
    }
    else
    {
        cout << "BOSS HP : "
            << boss.getHP()
            << " / 50"
            << "                    "
            << endl;

        cout << "隕石〇を左右移動で避ける"
            << "                    "
            << endl;
    }
    */

    cout << "A : 左移動   D : 右移動"
        << endl;

    // 前回の長い表示が残らないように空白を出す
    cout << "                                        "
        << endl;

    cout.flush();
}

void Game::drawGameOver()
{
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED | FOREGROUND_INTENSITY
    );

    cout << "==============================" << endl;
    cout << "          GAME OVER" << endl;
    cout << "==============================" << endl;
    cout << endl;
    cout << "プレイヤーのHPが0になりました" << endl;

    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_BLUE
    );
}

void Game::drawGameClear()
{
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY
    );

    cout << "==============================" << endl;
    cout << "          GAME CLEAR" << endl;
    cout << "==============================" << endl;

    // 白色に戻す
    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_BLUE
    );

    cout << endl;
    cout << "ボスを倒しました！" << endl;

    // 赤文字
    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED | FOREGROUND_INTENSITY
    );

    cout << "ボス撃破タイム : "
        << fixed
        << setprecision(2)
        << bossBattleTime
        << " 秒"
        << endl;

    // 元の白色
    SetConsoleTextAttribute(
        hConsole,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_BLUE
    );

    
}

// ========================================
// コンソール操作
// ========================================

void Game::setCursorPosition(int x, int y)
{
    HANDLE hConsole =
        GetStdHandle(STD_OUTPUT_HANDLE);

    COORD position;

    position.X = static_cast<SHORT>(x);
    position.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(
        hConsole,
        position
    );
}

void Game::hideCursor()
{
    HANDLE hConsole =
        GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(
        hConsole,
        &cursorInfo
    );

    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(
        hConsole,
        &cursorInfo
    );
}