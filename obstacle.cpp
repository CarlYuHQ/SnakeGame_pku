#include "obstacle.h"

Obstacle::Obstacle(Type type, const QPoint& location)
    : type(type), location(location) {}

Obstacle::Type Obstacle::getType() const {
    return type;
}

QPoint Obstacle::getLocation() const {
    return location;
}
