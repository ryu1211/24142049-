#pragma once

#include <string>

class Boss
{
private:
    int x;
    int y;
    int hp;

    bool alive;
    bool active;

public:
    Boss();

    void appear(int startX, int startY);

    // ’†‰›50“‚Ì”ÍˆÍ‚Åƒ‰ƒ“ƒ_ƒ€ˆÚ“®
    void moveRandom(int fieldWidth);

    void takeDamage(int damage);

    int getX() const;
    int getY() const;
    int getHP() const;

    bool isAlive() const;
    bool isActive() const;

    std::string getDisplayText() const;
    int getWidth() const;
};