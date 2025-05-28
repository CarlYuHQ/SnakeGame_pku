#include "gamewidget.h"
#include <QMessageBox>

const int Width = 50;
const int Height = 50;
const int Init_len = 4;
const QPoint Init_head({8, 8});
const char Init_dir = 'r';

GameWidget::GameWidget(QWidget* parent)
    : QWidget{ parent }, timer(new QTimer(this)), width(Width), height(Height), game(Snakegame(Width, Height, Init_len, Init_head, Init_dir))
{
    setFixedSize(width * gridSize, height * gridSize);
    connect(timer, &QTimer::timeout, this, &GameWidget::onTimeout);
    timer->start(t);
    srand(time(NULL));
    foodGenerate_time = rand() % (max_foodGenerate_time - min_foodGenerate_time) + min_foodGenerate_time;
    // 生成50个障碍物（25硬+25软）
    for (int i = 0; i < 25; i++) {
        game.generateObstacle(Obstacle::HARD);
        game.generateObstacle(Obstacle::SOFT);
    }
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    drawGrid(painter);
    drawSnake(painter);
    drawFood(painter);
    drawObstacles(painter);
    drawBullets(painter);
}

void GameWidget::drawGrid(QPainter& painter) {
    painter.setPen(QColor(200, 200, 200));
    for (int x = 0; x <= width; x++) {
        painter.drawLine(x * gridSize, 0, x * gridSize, height * gridSize);
    }
    for (int y = 0; y <= height; y++) {
        painter.drawLine(0, y * gridSize, width * gridSize, y * gridSize);
    }
}

void GameWidget::drawSnake(QPainter& painter) {
    const Snake& snake = game.getSnake();

    // 护盾优先级高于无敌
    if (snake.func_hasShield()) { // 使用访问方法
        painter.setBrush(QColor(0, 255, 255)); // 青色
    } else if (snake.func_isInvincible()) {
        painter.setBrush(QColor(255, 215, 0)); // 金色
    } else {
        painter.setBrush(Qt::green);
    }

    for (const QPoint& p : snake.getSnake()) {
        painter.drawRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
    }
}

void GameWidget::drawFood(QPainter& painter) {
    painter.setBrush(Qt::red);
    for (const Food& f : game.getFood()) {
        QPoint p = f.getLocation();
        painter.drawEllipse(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
    }
}

void GameWidget::drawObstacles(QPainter& painter) {
    for (const Obstacle& obs : game.getObstacles()) {
        QPoint p = obs.getLocation();
        // 硬障碍物：深灰色矩形
        if (obs.getType() == Obstacle::HARD) {
            painter.setBrush(QColor(100, 100, 100)); // 深灰色
            painter.drawRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
        }
        // 软障碍物：黄色矩形
        else {
            painter.setBrush(Qt::yellow);
            painter.drawRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
        }
    }
}

void GameWidget::drawBullets(QPainter& painter) {
    painter.setBrush(Qt::blue);
    for (const Bullet& b : game.getBullets()) {
        QPoint p = b.getPosition();
        painter.drawRect(p.x() * gridSize, p.y() * gridSize, 5, 5); // 小矩形表示子弹
    }
}

void GameWidget::onTimeout() {
    if (game_over) {
        return;
    }
    while (!changedir_lst.empty()) {    //清除同向/反向等无意义操作
        char dir_now = game.getSnake().getDirection();
        char dir_next = changedir_lst.front();
        if (dir_now == 'u' || dir_now == 'd') {
            if (dir_next == 'u' || dir_next == 'd') {
                changedir_lst.pop_front();
            }
            else {
                game.changeDirection(dir_next);
                changedir_lst.pop_front();
                break;
            }
        }
        else {
            if (dir_next == 'l' || dir_next == 'r') {
                changedir_lst.pop_front();
            }
            else {
                game.changeDirection(dir_next);
                changedir_lst.pop_front();
                break;
            }
        }
    }
    bool alive = game.step();
    if (!alive) {   //死了退出
        game_over = 1;
        timer->stop();
        update();
        QMessageBox::information(this, "Game Over", "press R to try again");
        return;
    }
    if (++time_cnt == foodGenerate_time) {  //每次到达设定好的随机生成食物时间就生成食物
        time_cnt = 0;
        game.generateFood(food_remaining_time);
        foodGenerate_time = rand() % (max_foodGenerate_time - min_foodGenerate_time) + min_foodGenerate_time;
    }
    update();

    new_step = 1;
}

void GameWidget::keyPressEvent(QKeyEvent* event)    //在一个step内的所有操作存下来，顺次执行，但若之后的某个step输入新操作，则直接清空
{
    if (event->key() == Qt::Key_R) {
        if (game_over) {
            changedir_lst.clear();
            game.reset(Init_len, Init_head, Init_dir);
            timer->start(t);
            game_over = 0;
            game.snake.hasShield=false;
            game.snake.isInvincible=false;
            update();
        }
        return;
    }
    if (new_step == 1) {
        changedir_lst.clear();
        new_step = 0;
    }
    switch (event->key()) {
    case Qt::Key_W:    changedir_lst.push_back('u'); break;
    case Qt::Key_S:  changedir_lst.push_back('d'); break;
    case Qt::Key_A:  changedir_lst.push_back('l'); break;
    case Qt::Key_D: changedir_lst.push_back('r'); break;
    case Qt::Key_Space: // 发射子弹
        game.fireBullet();
        break;
    case Qt::Key_I:     // 无敌技能
        game.getSnake().activateInvincible(5);
        break;
    case Qt::Key_P:     // 护盾技能
        game.getSnake().activateShield();
        update();
        break;
    }
}
