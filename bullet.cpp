#include "bullet.h"
#include <map>
using namespace std;
map<char, QPoint> chartopoint = {	//把方向对应的字符转成QPoint类型
    {'u', QPoint(0, -1)},
    {'d', QPoint(0, 1)},
    {'r', QPoint(1, 0)},
    {'l', QPoint(-1, 0)},
    };
Bullet::Bullet(const QPoint& pos, char dir)
    : position(pos), direction(dir) {}

void Bullet::move() {
    position += chartopoint[direction];
}

QPoint Bullet::getPosition() const {
    return position;
}
