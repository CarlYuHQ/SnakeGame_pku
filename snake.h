#pragma once

#include <QList>
#include <QPoint>
class Snake {
    QList<QPoint> snake_body;
    char direction;
public:
    Snake(int len, const QPoint& head, char dir);
    void reset(int len, const QPoint& head, char dir);
    void changeDirection(char target_dir);
    bool move();
    bool move_and_grow();

    QList<QPoint> getSnake() const;
    QPoint getHead() const;
    QPoint getNext() const;
    char getDirection() const;
};
