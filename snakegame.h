#pragma once
#include <QTimer>
#include "obstacle.h"
#include "snake.h"
#include "bullet.h"
using namespace std;
class Food {
    int remaining_time;
    bool foodType;
    QPoint location;
public:
    Food(int remaining_time, const QPoint& location, bool foodType);
    void step();
    int getTime() const;
    bool getType() const;
    QPoint getLocation() const;
};
class Snakegame{
    int width, height;

    QList<Food> food;
    bool game_over;
    void foodStep();
    bool deadPoint(const QPoint& p);
    QList<Food>::iterator foodPoint(const QPoint& p);
    QList<Bullet> bullets; // 新增子弹列表

    int bulletCooldown = 0; // 子弹冷却计数器
    const int BULLET_COOLDOWN_MAX = 50; // 10秒冷却（50*200ms）

    // 预定义障碍物形状
    const QList<QList<QPoint>> SHAPE_TEMPLATES = {
        // T字形
        {QPoint(0,0), QPoint(1,0), QPoint(2,0), QPoint(1,1), QPoint(1,2)},
        // L字形
        {QPoint(0,0), QPoint(0,1), QPoint(0,2), QPoint(1,2), QPoint(2,2)},
        // 十字形
        {QPoint(1,0), QPoint(0,1), QPoint(1,1), QPoint(2,1), QPoint(1,2)},
        // 矩形
        {QPoint(0,0), QPoint(0,1), QPoint(1,0), QPoint(1,1)},
        // Z字形
        {QPoint(0,0), QPoint(1,0), QPoint(1,1), QPoint(2,1)}
    };
public:
    Snake snake;
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

    bool canFireBullet() const { return bulletCooldown == 0; }
    // 添加子弹冷却相关函数
    int getBulletCooldown() const { return bulletCooldown; }
    int getMaxCooldown() const { return BULLET_COOLDOWN_MAX; }
    void resetBulletCooldown() { bulletCooldown = 0; }
    void setBulletCooldown(int value) { bulletCooldown = value; }
};
