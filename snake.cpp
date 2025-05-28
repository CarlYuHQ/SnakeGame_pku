#include "snake.h"
#include <cstdlib>
#include <map>
#include <QTimer>
using namespace std;

map<char, QPoint> char2point = {	//把方向对应的字符转成QPoint类型
    {'u', QPoint(0, -1)},
    {'d', QPoint(0, 1)},
    {'r', QPoint(1, 0)},
    {'l', QPoint(-1, 0)},
    };

void Snake::activateInvincible(int sec) {
    isInvincible = true;
    QTimer::singleShot(sec * 1000, [this](){
        isInvincible = false;
    });
}

Snake::Snake(int len, const QPoint& head, char dir) : direction(dir) {
    QPoint dir_point = char2point[dir];
    for (int i = 0; i < len; i++) {
        snake_body.push_back(head - i * dir_point);
    }
}

void Snake::reset(int len, const QPoint& head, char dir) {
    direction = dir;
    snake_body.clear();
    QPoint dir_point = char2point[dir];
    for (int i = 0; i < len; i++) {
        snake_body.push_back(head - i * dir_point);
    }
}

void Snake::changeDirection(char target_dir) {
    direction = target_dir;
}


bool Snake::move() {
    QPoint dir_point = char2point[direction];
    QPoint next_head = snake_body.front() + dir_point;
    snake_body.pop_back();

    // 无敌状态直接移动，不检测任何碰撞
    if (func_isInvincible()) {
        snake_body.push_front(next_head);
        return true;
    }

    // 处理护盾碰撞
    if (snake_body.contains(next_head)) {
        if (func_hasShield()) { // 有护盾则消耗
            hasShield = false;
        } else {          // 无护盾则死亡
            return false;
        }
    }

    snake_body.push_front(next_head);
    return true;
}

bool Snake::move_and_grow() {	//吃到食物的情况下走一步，尾部不删点


    QPoint dir_point = char2point[direction];
    QPoint next_head = snake_body.front() + dir_point;
    // 无敌状态直接移动，不检测任何碰撞
    if (isInvincible) {
        snake_body.push_front(next_head);
        return true;
    }
    // 在碰撞检测处处理护盾
    // 仅在发生自身碰撞时处理护盾
    if (snake_body.contains(next_head)) {
        if (!hasShield) {
            return false;  // 无护盾则死亡
        } else {
            hasShield = false;  // 有护盾则消耗
        }
    }

    snake_body.push_front(next_head);
    return 1;
}

QList<QPoint> Snake::getSnake() const {
    return snake_body;
}

QPoint Snake::getHead() const {
    return snake_body.front();
}

QPoint Snake::getNext() const {
    return getHead() + char2point[direction];
}

char Snake::getDirection() const{
    return direction;
}
