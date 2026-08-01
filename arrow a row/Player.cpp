#include "Player.h"

Player::Player(int startX, int startY)
{
    x = startX;
    y = startY;

    hp = 100;

    arrowDamage = 1;
    shotInterval = 20;
    fireLevel = 1;
}

void Player::moveLeft()
{
    if (x > 0)
    {
        x--;
    }
}

void Player::moveRight(int width)
{
    if (x < width - 1)
    {
        x++;
    }
}

void Player::takeDamage(int damage)
{
    hp -= damage;

    if (hp < 0)
    {
        hp = 0;
    }
}

void Player::powerUpDamage()
{
    arrowDamage++;
}

void Player::powerUpFireRate()
{
    if (shotInterval > 5)
    {
        shotInterval--;
        fireLevel++;
    }
}

int Player::getFireLevel() const
{
    return fireLevel;
}

int Player::getAttackLevel() const
{
    return arrowDamage;
}

int Player::getX() const
{
    return x;
}

int Player::getY() const
{
    return y;
}

int Player::getHP() const
{
    return hp;
}

int Player::getArrowDamage() const
{
    return arrowDamage;
}

int Player::getShotInterval() const
{
    return shotInterval;
}