#pragma once
#include <QTimer>
#include "obstacle.h"
#include "snake.h"
#include "bullet.h"
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
    QList<Bullet> bullets; // 新增子弹列表
public:
    Snakegame(int width, int height, int init_len, const QPoint& init_head, char init_dir);
    void reset(int init_len, const QPoint& init_head, char init_dir);
    bool step();
    void changeDirection(char dir);
    void generateFood(int remainingtime);

    bool isGameOver() const;
    Snake& getSnake();
    QList<Food> getFood() const;
    int getWidth() const;
    int getHeight() const;

    QList<Obstacle> obstacles; // 新增障碍物列表
    //
    void generateObstacle(Obstacle::Type type);
    QList<Obstacle> getObstacles() const;

    void fireBullet();
    QList<Bullet> getBullets() const;

    void clearObstacles() { obstacles.clear(); }//清除障碍物的方法
};
