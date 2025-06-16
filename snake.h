#pragma once
#include <QTimer>
#include <QList>
#include <QPoint>
class Snake {
    QList<QPoint> snake_body;
    char direction;

public:
    bool isInvincible = false;
    bool hasShield = false;
    Snake(int len, const QPoint& head, char dir);
    void reset(int len, const QPoint& head, char dir);
    void changeDirection(char target_dir);
    bool move();
    bool move_and_grow();

    QList<QPoint> getSnake() const;
    QPoint getHead() const;
    QPoint getNext() const;
    char getDirection() const;

    void activateInvincible(int sec);
    // 添加公共访问方法
    bool func_isInvincible() const { return isInvincible; }
    bool func_hasShield() const { return hasShield; }
    void activateShield() { hasShield = true; }

    void setBody(const QList<QPoint>& newBody) { snake_body = newBody; }
    int getScore() const;

    QTimer *invincibleTimer;    // QTimer不可拷贝，使用指针避免拷贝
};
