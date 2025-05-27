#include "gamewidget.h"
#include <QMessageBox>

const int Width = 20;
const int Height = 20;
const int Init_len = 3;
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
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    drawGrid(painter);
    drawSnake(painter);
    drawFood(painter);
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
    painter.setBrush(Qt::green);
    for (const QPoint& p : game.getSnake().getSnake()) {
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
        }
        return;
    }
    if (new_step == 1) {
        changedir_lst.clear();
        new_step = 0;
    }
    switch (event->key()) {
    case Qt::Key_Up:    changedir_lst.push_back('u'); break;
    case Qt::Key_Down:  changedir_lst.push_back('d'); break;
    case Qt::Key_Left:  changedir_lst.push_back('l'); break;
    case Qt::Key_Right: changedir_lst.push_back('r'); break;
    }
}
