#include "snake.h"
#include <cstdlib>
#include <map>
using namespace std;

map<char, QPoint> char2point = {	//把方向对应的字符转成QPoint类型
    {'u', QPoint(0, -1)},
    {'d', QPoint(0, 1)},
    {'r', QPoint(1, 0)},
    {'l', QPoint(-1, 0)},
    };

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

bool Snake::move() {	//吃不到食物的情况下走一步
    QPoint dir_point = char2point[direction];
    QPoint next_head = snake_body.front() + dir_point;
    snake_body.pop_back();	//先删点，后判断撞没撞自己
    if (snake_body.contains(next_head)) {
        return 0;
    }
    snake_body.push_front(next_head);
    return 1;
}

bool Snake::move_and_grow() {	//吃到食物的情况下走一步，尾部不删点
    QPoint dir_point = char2point[direction];
    QPoint next_head = snake_body.front() + dir_point;
    if (snake_body.contains(next_head)) {
        return 0;
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
