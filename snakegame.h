#pragma once

#include "snake.h"
using namespace std;
class Food {
    int remaining_time;
    QPoint location;
public:
    Food(int remaining_time, const QPoint& location);
    void step();
    int getTime() const;
    QPoint getLocation() const;
};
class Snakegame{
    int width, height;
    Snake snake;
    QList<Food> food;
    bool game_over;
    void foodStep();
    bool deadPoint(const QPoint& p);
    QList<Food>::iterator foodPoint(const QPoint& p);
public:
    Snakegame(int width, int height, int init_len, const QPoint& init_head, char init_dir);
    void reset(int init_len, const QPoint& init_head, char init_dir);
    bool step();
    void changeDirection(char dir);
    void generateFood(int remainingtime);

    bool isGameOver() const;
    const Snake& getSnake() const;
    QList<Food> getFood() const;
    int getWidth() const;
    int getHeight() const;
};
