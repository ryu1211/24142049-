#pragma once

class Player
{
private:
    int x;
    int y;
    int hp;

    int arrowDamage;
    int shotInterval;
    int fireLevel;
    

public:
    Player(int startX, int startY);

    void moveLeft();
    void moveRight(int width);

    void takeDamage(int damage);

    void powerUpDamage();
    void powerUpFireRate();

    int getX() const;
    int getY() const;
    int getHP() const;

    int getArrowDamage() const;
    int getShotInterval() const;

    int getFireLevel() const;
    int getAttackLevel() const;
};
