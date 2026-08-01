#include "Enemy.h"

Enemy::Enemy(int startX, int startY, int startHP)
{
    x = startX;
    y = startY;

    hp = startHP;

    alive = true;
    spawned = false;
}

void Enemy::moveDown()
{
    y++;
}

void Enemy::takeDamage(int damage)
{
    hp -= damage;

    if (hp <= 0)
    {
        alive = false;
    }
}

void Enemy::spawn()
{
    spawned = true;
}

void Enemy::destroy()
{
    alive = false;
}

int Enemy::getX() const
{
    return x;
}

int Enemy::getY() const
{
    return y;
}

int Enemy::getHP() const
{
    return hp;
}

bool Enemy::isAlive() const
{
    return alive;
}

bool Enemy::isSpawned() const
{
    return spawned;
}