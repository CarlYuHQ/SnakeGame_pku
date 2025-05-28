#pragma once
#include <QPoint>

class Bullet {
    QPoint position;
    char direction;
public:
    Bullet(const QPoint& pos, char dir);
    void move();
    QPoint getPosition() const;
};
