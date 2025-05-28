#pragma once
#include <QPoint>

class Obstacle {
public:
    enum Type { HARD, SOFT };
    Obstacle(Type type, const QPoint& location);
    Type getType() const;
    QPoint getLocation() const;
private:
    Type type;
    QPoint location;
};

