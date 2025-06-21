#include "bullet.h"
#include <map>
#include <QList>
#include <QPoint>
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

// 添加新方法：获取子弹移动路径上的所有点
QList<QPoint> Bullet::getPath() const {
    QList<QPoint> path;
    QPoint current = position;

    // 添加当前位置
    path.append(current);

    // 添加移动方向上的下一个位置
    path.append(current + chartopoint[direction]);

    return path;
}
