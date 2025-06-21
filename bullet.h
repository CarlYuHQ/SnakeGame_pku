#pragma once
#include <QPoint>
#include <map>

// 添加 extern 声明
extern std::map<char, QPoint> chartopoint; // 全局变量声明
class Bullet {
    QPoint position;
    char direction;
public:
    Bullet(const QPoint& pos, char dir);
    void move();
    QPoint getPosition() const;

    // 添加新方法用于检测路径上的障碍物
    QList<QPoint> getPath() const;
};
