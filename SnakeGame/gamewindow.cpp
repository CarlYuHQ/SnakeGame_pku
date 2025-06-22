#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QDebug>

const int Width = 35; // 55
const int Height = 28; // 50
const int Init_len = 4;
const QPoint Init_head({8, 8});
const char Init_dir = 'r';

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    timer(new QTimer(this)),
    width(Width),
    height(Height),
    game(Width, Height, Init_len, Init_head, Init_dir)
{
    ui->setupUi(this);
    setFixedSize(width * gridSize + scoreWidth, height * gridSize);

    ui->labelHighScore->setGeometry(width * gridSize + 250, 4 * gridSize, 100, 75);
    ui->labelHighScoreText->setGeometry(width * gridSize + 100, 4 * gridSize, 150, 75);
    ui->labelScore->setGeometry(width * gridSize + 250, 4 * gridSize + 100, 100, 75);
    ui->labelScoreText->setGeometry(width * gridSize + 100, 4 * gridSize + 100, 150, 75);

    ui->labelInvincibleRemaining->setGeometry(width * gridSize + 300, height * gridSize - 545, 100, 75);
    ui->labelInvincibleRemainingText->setGeometry(width * gridSize + 50, height * gridSize - 545, 250, 75);
    ui->bulletStatusLabel->setGeometry(width * gridSize + 100, height * gridSize - 440, 250, 75);

    ui->startButton->setGeometry(width * gridSize + 100, height * gridSize - 335, 200, 75);
    ui->pauseButton->setGeometry(width * gridSize + 100, height * gridSize - 230, 200, 75);
    ui->backButton->setGeometry(width * gridSize + 100, height * gridSize - 125, 200, 75);

    if(!snakeHeadImg.load(":/image/greenSnakeHead.png")) { // 使用资源系统
        qDebug() << "Failed to load snake head image!";
    }
    if(!blueSnakeHeadImg.load(":/image/blueSnakeHead.png")) { // 使用资源系统
        qDebug() << "Failed to load blue snake head image!";
    }

    // 加载食物图片
    if(!foodImg.load(":/image/apple.png")) { // 使用资源系统
        qDebug() << "Failed to load food image!";
    } else {
        // 缩放图片到网格大小
        foodImg = foodImg.scaled(gridSize, gridSize,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    }

    // 加载护盾图片
    if(!defenseImg.load(":/image/defense.png")) { // 使用资源系统
        qDebug() << "Failed to load defense image!";
    } else {
        // 缩放图片到网格大小
        defenseImg = defenseImg.scaled(gridSize, gridSize,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    }

    // 加载游戏背景图片
    if(!backgroundImg.load(":/image/background.png")) {
        qDebug() << "Failed to load background image!";
    } else {
        // 缩放背景到游戏区域大小
        backgroundImg = backgroundImg.scaled(
            width * gridSize,
            height * gridSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    }

    // 加载分数背景图片
    if(!scoreBackgroundImg.load(":/image/scoreBackground.png")) {
        qDebug() << "Failed to load scoreBackground image!";
    } else {
        // 缩放背景到分数区域大小
        scoreBackgroundImg = scoreBackgroundImg.scaled(
            scoreWidth,
            height * gridSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    }

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
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    drawGridImage(painter);
    drawScoreBackgroundImage(painter);
    drawSnakeImage(painter);
    drawFoodImage(painter);
    drawObstacles(painter);
    drawBullets(painter);
}

void GameWindow::drawGridImage(QPainter& painter) {
    // 绘制背景图片（如果有）
    if(!backgroundImg.isNull()) {
        painter.drawPixmap(0, 0, backgroundImg);
    } else {
        // 没有背景图片时使用默认背景色
    }

    // 绘制网格线
    painter.setPen(QColor(100, 100, 100));
    for (int x = 0; x <= width; x++) {
        painter.drawLine(x * gridSize, 0, x * gridSize, height * gridSize);
    }
    for (int y = 0; y <= height; y++) {
        painter.drawLine(0, y * gridSize, width * gridSize, y * gridSize);
    }
}

void GameWindow::drawScoreBackgroundImage(QPainter& painter) {
    // 绘制分数背景图片（如果有）
    if(!scoreBackgroundImg.isNull()) {
        painter.drawPixmap(width * gridSize, 0, scoreBackgroundImg);
    } else {
        // 没有背景图片时使用默认背景色
    }
}

void GameWindow::drawSnakeImage(QPainter& painter) {
    const Snake& snake = game.getSnake();
    const QList<QPoint>& snakeBody = snake.getSnake();
    if(snakeBody.isEmpty()) return;

    // 设置抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制蛇身（圆形）
    for(int i = 1; i < snakeBody.size(); ++i) { // 从第2节开始绘制蛇身
        const QPoint& p = snakeBody[i];
        QRectF segmentRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);

        // 根据状态设置颜色
        if(snake.func_hasShield()) {
            painter.setBrush(QColor(0, 255, 255)); // 青色护盾
        } else if(snake.func_isInvincible()) {
            painter.setBrush(QColor(18, 150, 219)); // 蓝色无敌
        } else {
            painter.setBrush(Qt::green); // 普通绿色
        }

        painter.setPen(Qt::NoPen); // 去掉边框
        painter.drawEllipse(segmentRect);
    }

    // 单独绘制蛇头图片
    if(snake.func_isInvincible()) {
        if(!blueSnakeHeadImg.isNull()) {
            const QPoint& head = snakeBody.first();
            QRect headRect(head.x() * gridSize, head.y() * gridSize, gridSize, gridSize);

            // 根据方向旋转图片
            QTransform transform;
            switch(snake.getDirection()) {
            case 'u': transform.rotate(270); break; // 上
            case 'd': transform.rotate(90); break;  // 下
            case 'l': transform.rotate(180); break; // 左
            case 'r': break;                       // 右(默认)
            }

            QPixmap rotatedHead = blueSnakeHeadImg.transformed(transform, Qt::SmoothTransformation);
            painter.drawPixmap(headRect, rotatedHead);
        } else {
            // 如果图片加载失败，用蓝色矩形作为蛇头
            painter.setBrush(QColor(18, 150, 219));
            const QPoint& head = snakeBody.first();
            painter.drawRect(head.x() * gridSize, head.y() * gridSize, gridSize, gridSize);
        }
    }
    else {
        if(!snakeHeadImg.isNull()) {
            const QPoint& head = snakeBody.first();
            QRect headRect(head.x() * gridSize, head.y() * gridSize, gridSize, gridSize);

            // 根据方向旋转图片
            QTransform transform;
            switch(snake.getDirection()) {
            case 'u': transform.rotate(270); break; // 上
            case 'd': transform.rotate(90); break;  // 下
            case 'l': transform.rotate(180); break; // 左
            case 'r': break;                       // 右(默认)
            }

            QPixmap rotatedHead = snakeHeadImg.transformed(transform, Qt::SmoothTransformation);
            painter.drawPixmap(headRect, rotatedHead);
        } else {
            // 如果图片加载失败，用绿色矩形作为蛇头
            painter.setBrush(Qt::green);
            const QPoint& head = snakeBody.first();
            painter.drawRect(head.x() * gridSize, head.y() * gridSize, gridSize, gridSize);
        }
    }
}

void GameWindow::drawFoodImage(QPainter& painter) {
    painter.setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放

    for (const Food& f : game.getFood()) {
        QPoint p = f.getLocation();
        QRect foodRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);

        if(f.getType() == 1) {
            if(!defenseImg.isNull()) {
                // 绘制护盾图片
                painter.drawPixmap(foodRect, defenseImg);
            } else {
                // 图片加载失败时用蓝色圆形替代
                painter.setBrush(QColor(18, 150, 219));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(foodRect);
            }
        }
        else {
            if(!foodImg.isNull()) {
                // 绘制食物图片
                painter.drawPixmap(foodRect, foodImg);
            } else {
                // 图片加载失败时用红色圆形替代
                painter.setBrush(Qt::red);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(foodRect);
            }
        }
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
        ui->bulletStatusLabel->setText(QString("子弹冷却: %1").arg(seconds));
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
    if (event->key() == Qt::Key_P) {
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
    case Qt::Key_Space: // 发射子弹
        if (game.canFireBullet()) {
            game.fireBullet();
        }
        break;
//    case Qt::Key_J:     // 无敌技能
//        game.getSnake().activateInvincible(5);
//        break;
//    case Qt::Key_K:     // 护盾技能
//        game.getSnake().activateShield();
//        update();
//        break;
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
