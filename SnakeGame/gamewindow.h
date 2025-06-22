#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>
#include <QPainter>
#include "snakegame.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTimeout();
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_backButton_clicked();

private:
    Ui::GameWindow *ui;
    QTimer* timer;
    int width;
    int height;
    int highestScore = 4;
    int scoreWidth = 400;
    int gridSize = 40;  // 20
    int min_foodGenerate_time = 2;
    int max_foodGenerate_time = 15;
    int food_remaining_time = 50;
    bool game_over = true;
    bool game_started = false;
    bool game_paused = false;
    int t = 200;
    int time_cnt = 0;
    int foodGenerate_time;
    QList<char> changedir_lst;
    bool new_step = true;
    Snakegame game;
    QLabel* bulletStatusLabel; // 添加子弹状态标签
    QPixmap snakeHeadImg;  // 绿色蛇头图片
    QPixmap blueSnakeHeadImg;  // 蓝色蛇头图片
    QPixmap foodImg;  // 食物图片
    QPixmap defenseImg;  // 护盾图片
    QPixmap backgroundImg;  // 游戏背景图片
    QPixmap scoreBackgroundImg;  // 分数背景图片

    void drawGridImage(QPainter& painter);
    void drawScoreBackgroundImage(QPainter& painter);
    void drawSnakeImage(QPainter& painter);
    void drawFoodImage(QPainter& painter);
    void drawObstacles(QPainter& painter);
    void drawBullets(QPainter& painter);
    void startGame();
    void togglePause();
    void saveHighestScore();
};

#endif // GAMEWINDOW_H
