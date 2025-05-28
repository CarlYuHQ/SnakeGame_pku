#pragma once

#include "snakegame.h"
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private slots:
    void onTimeout();
private:
    QTimer* timer;
    Snakegame game;
    int gridSize = 20;
    int width;
    int height;
    int min_foodGenerate_time = 2;
    int max_foodGenerate_time = 15;
    int food_remaining_time = 50;
    bool game_over = 0;
    int t = 200;
    int time_cnt = 0;
    int foodGenerate_time;
    QList<char> changedir_lst;
    bool new_step = 1;

    void drawGrid(QPainter& painter);
    void drawSnake(QPainter& painter);
    void drawFood(QPainter& painter);
    void drawObstacles(QPainter& painter);
    void drawBullets(QPainter& painter);
signals:
};
