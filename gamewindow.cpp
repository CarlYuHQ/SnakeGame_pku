#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QDebug>

const int Width = 55;
const int Height = 50;
const int Init_len = 4;
const QPoint Init_head({8, 8});
const char Init_dir = 'r';

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    timer(new QTimer(this)),
    width(Width),
    height(Height),
    game(Snakegame(Width, Height, Init_len, Init_head, Init_dir))
{
    ui->setupUi(this);

    // 创建子弹状态标签
    bulletStatusLabel = new QLabel(this);
    bulletStatusLabel->setGeometry(1150, 350, 200, 50);
    bulletStatusLabel->setFont(QFont("黑体", 12));
    bulletStatusLabel->setText("子弹: 就绪");

    setFixedSize(width * gridSize + 300, height * gridSize);

    // 从txt文件读取最高分
    QFile scoreFile("../SnakeGame/highestScore.txt");
    if (scoreFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&scoreFile);
        in >> highestScore;  // 读取数字
        scoreFile.close();

        // 确保读取的值有效
        if (highestScore < 4) highestScore = 4;  // 4是初始蛇长
    } else {
        // 文件不存在时使用默认值
        highestScore = 4;
    }

    // 初始状态：游戏未开始，暂停按钮禁用
    ui->pauseButton->setEnabled(false);
    ui->pauseButton->setText("暂停");

    connect(timer, &QTimer::timeout, this, &GameWindow::onTimeout);

//    timer->start(t);
//    srand(time(NULL));
//    foodGenerate_time = rand() % (max_foodGenerate_time - min_foodGenerate_time) + min_foodGenerate_time;

    // 生成50个障碍物（25硬+25软）
//    for (int i = 0; i < 25; i++) {
//        game.generateObstacle(Obstacle::HARD);
//        game.generateObstacle(Obstacle::SOFT);
//    }
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    drawGrid(painter);
    drawSnake(painter);
    drawFood(painter);
    drawObstacles(painter);
    drawBullets(painter);
}

void GameWindow::drawGrid(QPainter& painter) {
    painter.setPen(QColor(200, 200, 200));
    for (int x = 0; x <= width; x++) {
        painter.drawLine(x * gridSize, 0, x * gridSize, height * gridSize);
    }
    for (int y = 0; y <= height; y++) {
        painter.drawLine(0, y * gridSize, width * gridSize, y * gridSize);
    }
}

void GameWindow::drawSnake(QPainter& painter) {
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

void GameWindow::drawFood(QPainter& painter) {
    for (const Food& f : game.getFood()) {
        QPoint p = f.getLocation();
        if(f.getType() == 1){
            painter.setBrush(Qt::blue);
        }
        else{
            painter.setBrush(Qt::red);
        }
        painter.drawEllipse(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
    }
}

void GameWindow::drawObstacles(QPainter& painter) {
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

void GameWindow::drawBullets(QPainter& painter) {
    painter.setBrush(Qt::blue);
    for (const Bullet& b : game.getBullets()) {
        QPoint p = b.getPosition();
        painter.drawRect(p.x() * gridSize, p.y() * gridSize, 5, 5); // 子弹当前位置

        // 绘制子弹路径（可视化）
        QList<QPoint> path = b.getPath();
        if (path.size() > 1) {
            painter.setPen(Qt::blue);
            painter.drawLine(
                path[0].x() * gridSize + gridSize/2,
                path[0].y() * gridSize + gridSize/2,
                path[1].x() * gridSize + gridSize/2,
                path[1].y() * gridSize + gridSize/2
                );
        }
    }
}

void GameWindow::onTimeout() {
    if (game_over || game_paused) {
        return;
    }
    // 更新子弹冷却状态
    int cooldown = game.getBulletCooldown();
    if (cooldown > 0) {
        game.setBulletCooldown(cooldown - 1); // 减少冷却计数
    }

    // 更新UI显示冷却状态
    if (game.getBulletCooldown() == 0) {
        ui->bulletStatusLabel->setText("子弹: 就绪");
    } else {
        int seconds = (game.getBulletCooldown() * t) / 1000; // 转换为秒
        ui->bulletStatusLabel->setText(QString("子弹冷却: %1秒").arg(seconds));
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
    int score = game.getSnake().getScore();

    if (!alive) {   //死了退出
        game_over = true;
        game_started = false;
        timer->stop();

        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("游戏结束");
        msgBox.setTextFormat(Qt::RichText);

        if (score > highestScore) {
            highestScore = score;
            saveHighestScore();
            msgBox.setText("<b>恭喜打破记录！</b>");
            msgBox.setInformativeText(QString("新记录：<font color='red'>%1</font>").arg(score));
        } else {
            msgBox.setText("<b>游戏结束！</b>");
            msgBox.setInformativeText(QString("本次得分：<font color='red'>%1</font><br>"
                                              "历史最高：<font color='blue'>%2</font>").arg(score).arg(highestScore));
        }

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        update();
        return;
    }
    if (++time_cnt == foodGenerate_time) {  //每次到达设定好的随机生成食物时间就生成食物
        time_cnt = 0;
        game.generateFood(food_remaining_time);
        foodGenerate_time = rand() % (max_foodGenerate_time - min_foodGenerate_time) + min_foodGenerate_time;
    }
    ui->labelHighScore->setNum(highestScore);
    ui->labelScore->setNum(score);
    ui->labelInvincibleRemaining->setNum(game.getSnake().getInvincibleRemaining());

    update();

    new_step = 1;
}

void GameWindow::keyPressEvent(QKeyEvent* event)    //在一个step内的所有操作存下来，顺次执行，但若之后的某个step输入新操作，则直接清空
{
    if (event->key() == Qt::Key_T) {
        togglePause();
        return;
    }
    if (event->key() == Qt::Key_R) {
        startGame();
        return;
    }

    if (game_paused || !game_started || game_over) {
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
    // case Qt::Key_Space: // 发射子弹
    //     game.fireBullet();
    //     break;
    case Qt::Key_Space: // 发射子弹
        if (game.canFireBullet()) {
            game.fireBullet();
        }
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

void GameWindow::closeEvent(QCloseEvent *event)
{
    timer->stop();
    if (QWidget *mainWindow = this->parentWidget()) {
        mainWindow->show();
    }
    event->accept();
}

void GameWindow::on_backButton_clicked()
{
    this->close();
}

void GameWindow::on_startButton_clicked()
{
    startGame();
}

void GameWindow::on_pauseButton_clicked()
{
    togglePause();
}

void GameWindow::startGame()
{
    if (game_over) {
        changedir_lst.clear();
        game.reset(Init_len, Init_head, Init_dir);
        timer->start(t);
        game_over = false;
        game_started = true;
        game_paused = false;
        game.snake.hasShield=false;
        game.snake.isInvincible=false;

        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->pauseButton->setText("暂停");

        time_cnt = 0;
        srand(time(NULL));
        foodGenerate_time = rand() % (max_foodGenerate_time - min_foodGenerate_time) + min_foodGenerate_time;
        game.resetBulletCooldown(); // 重置子弹冷却

        update();
    }
}

void GameWindow::togglePause()
{
    if (!game_started || game_over) return;

    game_paused = !game_paused;

    if (game_paused) {
        timer->stop();
        ui->pauseButton->setText("继续");
    } else {
        timer->start(t);
        ui->pauseButton->setText("暂停");
    }

    update();
}

void GameWindow::saveHighestScore()
{
    QFile scoreFile("../SnakeGame/highestScore.txt");
    if (scoreFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scoreFile);
        out << highestScore;
        scoreFile.close();
    }
}
