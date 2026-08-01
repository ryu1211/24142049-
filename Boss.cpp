#include "Boss.h"

#include <cstdlib>
#include <string>

Boss::Boss()
{
    x = 0;
    y = 1;

    hp = 100;

    alive = true;
    active = false;
}

void Boss::appear(int startX, int startY)
{
    x = startX;
    y = startY;

    hp = 100;

    alive = true;
    active = true;
}

void Boss::moveRandom(int fieldWidth)
{
    if (!active || !alive)
    {
        return;
    }

    /*
    中央50％の範囲

    fieldWidthが40の場合
    左端は10
    右端は30
    */
    int leftLimit = fieldWidth / 4;
    int rightLimit = fieldWidth * 3 / 4;

    /*
    ボスの右端が範囲を超えないように、
    ボスの表示幅を引く
    */
    int maxBossX = rightLimit - getWidth();

    if (maxBossX < leftLimit)
    {
        maxBossX = leftLimit;
    }

    /*
    -2～+2の範囲でランダムに移動
    */
    int moveAmount = rand() % 5 - 2;

    x += moveAmount;

    // 左端の制限
    if (x < leftLimit)
    {
        x = leftLimit;
    }

    // 右端の制限
    if (x > maxBossX)
    {
        x = maxBossX;
    }
}

void Boss::takeDamage(int damage)
{
    if (!active || !alive)
    {
        return;
    }

    hp -= damage;

    if (hp <= 0)
    {
        hp = 0;
        alive = false;
    }
}

std::string Boss::getDisplayText() const
{
    return "BOSS(" + std::to_string(hp) + ")";
}

int Boss::getWidth() const
{
    return static_cast<int>(getDisplayText().length());
}

int Boss::getX() const
{
    return x;
}

int Boss::getY() const
{
    return y;
}

int Boss::getHP() const
{
    return hp;
}

bool Boss::isAlive() const
{
    return alive;
}

bool Boss::isActive() const
{
    return active;
}