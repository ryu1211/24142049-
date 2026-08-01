#pragma once

class Enemy
{
private:
    int x;
    int y;

    int hp;

    bool alive;
    bool spawned;

public:
    Enemy(int startX, int startY, int startHP);

    void moveDown();

    void takeDamage(int damage);

    void spawn();
    void destroy();

    int getX() const;
    int getY() const;
    int getHP() const;

    bool isAlive() const;
    bool isSpawned() const;
};
