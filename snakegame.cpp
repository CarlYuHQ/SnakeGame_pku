#include "snakegame.h"
#include <cstdlib>
#include <ctime>
using namespace std;

Food::Food(int remaining_time, const QPoint& location) : remaining_time(remaining_time), location(location) {}

void Food::step() {	//食物剩余时间更新
    remaining_time--;
}

int Food::getTime() const {
    return remaining_time;
}

QPoint Food::getLocation() const {
    return location;
}

Snakegame::Snakegame(int width, int height,int init_len, const QPoint& init_head, char init_dir)
    : width(width), height(height), snake(Snake(init_len, init_head, init_dir)), game_over(0)
{
    srand(time(NULL));
}

void Snakegame::foodStep() {	//所有食物剩余时间更新，时间为0的删掉
    for (auto it = food.begin(); it != food.end();) {
        (*it).step();
        if ((*it).getTime() <= 0) {
            it = food.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool Snakegame::deadPoint(const QPoint& p) {	//判定这个点撞到会不会死，后期在这里加障碍物判定，撞到自己身上的情况在snake的move函数里判断了
    if (p.x() < 0 || p.x() >= width || p.y() < 0 || p.y() >= height) {	//超出场地范围
        return 1;
    }
    return 0;
}

QList<Food>::iterator Snakegame::foodPoint(const QPoint& p) {	//判断这个点有没有食物，若有返回食物的迭代器，若无返回end()
    for (auto it = food.begin(); it != food.end(); ++it) {
        if ((*it).getLocation() == p) {
            return it;
        }
    }
    return food.end();
}

void Snakegame::reset(int init_len, const QPoint& init_head, char init_dir) {
    game_over = 0;
    snake.reset(init_len, init_head, init_dir);
    food.clear();
}

bool Snakegame::step() {
    if (game_over) {
        return 0;
    }
    QPoint next = snake.getNext();
    if (deadPoint(next)) {	//如果下一步撞墙，直接死
        game_over = 1;
        return 0;
    }
    auto it_food = foodPoint(next);
    if (it_food != food.end()) {	//下一步有食物
        if (!snake.move_and_grow()) {	//撞自己了，死
            game_over = 1;
            return 0;
        }
        food.erase(it_food);	//吃掉食物后删除食物
    }
    else {	//下一步没有食物
        if (!snake.move()) {	//撞自己了，死
            game_over = 1;
            return 0;
        }
    }
    foodStep();	//先吃，再更新食物状态
    return 1;
}

void Snakegame::changeDirection(char dir) {
    snake.changeDirection(dir);
}

void Snakegame::generateFood(int remainingtime) {	//随机生成一个食物
    while (true) {
        int x = rand() % width;
        int y = rand() % height;
        QPoint p(x, y);
        if (!deadPoint(p) && !snake.getSnake().contains(p) && foodPoint(p) == food.end()) {	//当前位置是合法的，且不是蛇，且没有食物，才会放置食物
            food.push_back(Food(remainingtime, p));
            return;
        }
    }
}

bool Snakegame::isGameOver() const {
    return game_over;
}

const Snake& Snakegame::getSnake() const {
    return snake;
}

QList<Food> Snakegame::getFood() const {
    return food;
}

int Snakegame::getWidth() const {
    return width;
}

int Snakegame::getHeight() const {
    return height;
}
