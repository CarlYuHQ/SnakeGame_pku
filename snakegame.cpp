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
    : width(width),
    height(height),
    snake(Snake(init_len, init_head, init_dir)), // 先初始化 snake
    game_over(0) // 然后初始化 game_over
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

    // 检查硬障碍物
    for (const Obstacle& obs : obstacles) {
        if (obs.getType() == Obstacle::HARD && obs.getLocation() == p)
            return true;
    }
    return false;
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
    clearObstacles();  // 清空旧障碍物
    // 重新生成障碍物
    for (int i = 0; i < 25; i++) {
        generateObstacle(Obstacle::HARD);
        generateObstacle(Obstacle::SOFT);
    }
}

bool Snakegame::step() {

    // // 移动子弹
    // for (auto& bullet : bullets) {
    //     bullet.move();
    //     QPoint pos = bullet.getPosition();

    //     // 检查障碍物碰撞
    //     for (auto it = obstacles.begin(); it != obstacles.end();) {
    //         if (it->getLocation() == pos) {
    //             it = obstacles.erase(it);
    //         } else {
    //             ++it;
    //         }
    //     }
    // }

    // 更新子弹冷却
    if (bulletCooldown > 0) bulletCooldown--;

    // 移动子弹（双倍速度：每步移动两次）
    for (int i = 0; i < 2; i++) {
        // 先检测路径碰撞，再移动子弹
        for (auto it = bullets.begin(); it != bullets.end();) {
            // 获取子弹路径上的所有点
            QList<QPoint> path = it->getPath();

            bool hit = false;
            // 检查路径上的每个点是否有障碍物
            for (const QPoint& p : path) {
                // 检查出界
                if (p.x() < 0 || p.x() >= width || p.y() < 0 || p.y() >= height) {
                    hit = true;
                    break;
                }

                // 检查障碍物碰撞
                for (auto obsIt = obstacles.begin(); obsIt != obstacles.end();) {
                    if (obsIt->getLocation() == p) {
                        // 移除障碍物
                        obsIt = obstacles.erase(obsIt);
                        hit = true;
                        break;
                    } else {
                        ++obsIt;
                    }
                }

                if (hit) break;
            }

            if (hit) {
                // 击中障碍物或出界，移除子弹
                it = bullets.erase(it);
            } else {
                // 没有击中，移动子弹
                it->move();
                ++it;
            }
        }
    }

    if (game_over) {
        return 0;
    }
    QPoint next = snake.getNext();
    // 边界碰撞检测（无论是否无敌）
    if (next.x() < 0 || next.x() >= width || next.y() < 0 || next.y() >= height) {
        game_over = true;
        return false;
    }
    // 无敌状态下跳过边界和障碍物检测
    if (!snake.func_isInvincible()) {
        if (deadPoint(next)) { // 检查边界和硬障碍物
            game_over = true;
            return false;
        }
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

    // 处理软障碍物碰撞（无论是否无敌）
    QPoint head = snake.getHead();
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        if (it->getLocation() == head && it->getType() == Obstacle::SOFT) {
            // 检查无敌状态
            if(snake.func_isInvincible()){
                it = obstacles.erase(it);
                continue;
            }
            //检查护盾状态
            if (snake.func_hasShield()) {
                // 消耗护盾，移除障碍物，不缩短蛇身
                snake.hasShield = false;; // 关闭护盾
                it = obstacles.erase(it);
            } else {
                // 无护盾时缩短蛇身
                QList<QPoint> body = snake.getSnake();
                if (body.size() > 1) {
                    body.pop_back();
                    //snake.reset(body.size(), body.front(), snake.getDirection());
                    snake.setBody(body); // 使用新方法更新蛇身
                } else {
                    game_over = true;
                    return false;
                }
                it = obstacles.erase(it);
            }
        } else {
            ++it;
        }
    }

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

Snake& Snakegame::getSnake(){
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

// // 新增生成障碍物函数
// void Snakegame::generateObstacle(Obstacle::Type type) {
//     int attempts = 0;
//     QPoint snakeHead = snake.getHead(); // 获取蛇头位置

//     while (attempts++ < 100) {
//         int x = rand() % width;
//         int y = rand() % height;
//         QPoint p(x, y);

//         // 计算与蛇头的曼哈顿距离
//         int distance = abs(p.x() - snakeHead.x()) + abs(p.y() - snakeHead.y());
//         if (distance < 8) continue; // 距离小于5格则跳过

//         if (!deadPoint(p) &&
//             !snake.getSnake().contains(p) &&
//             foodPoint(p) == food.end() &&
//             std::none_of(obstacles.begin(), obstacles.end(), [&p](const Obstacle& obs) {
//                 return obs.getLocation() == p;
//             })) {
//             obstacles.append(Obstacle(type, p));
//             return;
//         }
//     }
// }

// 旋转点坐标
QPoint rotatePoint(const QPoint& p, int rotation) {
    switch (rotation) {
    case 1: // 90度
        return QPoint(-p.y(), p.x());
    case 2: // 180度
        return QPoint(-p.x(), -p.y());
    case 3: // 270度
        return QPoint(p.y(), -p.x());
    default: // 0度
        return p;
    }
}

void Snakegame::generateObstacle(Obstacle::Type type) {
    int attempts = 0;
    const int MAX_ATTEMPTS = 50;
    QPoint snakeHead = snake.getHead();
    // 预定义形状
    static const QList<QList<QPoint>> SHAPE_TEMPLATES = {
        // T字形
        {QPoint(0,0), QPoint(1,0), QPoint(2,0), QPoint(1,1), QPoint(1,2)},
        // L字形
        {QPoint(0,0), QPoint(0,1), QPoint(0,2), QPoint(1,2), QPoint(2,2)},
        // 十字形
        {QPoint(1,0), QPoint(0,1), QPoint(1,1), QPoint(2,1), QPoint(1,2)},
        // 矩形
        {QPoint(0,0), QPoint(0,1), QPoint(1,0), QPoint(1,1), QPoint(0,2), QPoint(1,2)},
        // Z字形
        {QPoint(0,0), QPoint(1,0), QPoint(1,1), QPoint(2,1), QPoint(2,2)}
    };

    while (attempts++ < MAX_ATTEMPTS) {
        // 随机选择形状
        int shapeIndex = rand() % SHAPE_TEMPLATES.size();
        QList<QPoint> shape = SHAPE_TEMPLATES[shapeIndex];

        // 随机旋转
        int rotation = rand() % 4;
        for (QPoint& p : shape) {
            p = rotatePoint(p, rotation);
        }

        // 计算边界
        int minX = 0, maxX = 0, minY = 0, maxY = 0;
        for (const QPoint& p : shape) {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
            minY = qMin(minY, p.y());
            maxY = qMax(maxY, p.y());
        }
        int shapeWidth = maxX - minX + 1;
        int shapeHeight = maxY - minY + 1;

        // 随机位置
        int x = rand() % (width - shapeWidth);
        int y = rand() % (height - shapeHeight);
        QPoint basePos(x, y);

        // 检查所有点是否有效
        bool valid = true;
        for (const QPoint& p : shape) {
            QPoint absPos = basePos + p - QPoint(minX, minY);

            // 检查边界
            if (absPos.x() < 0 || absPos.x() >= width ||
                absPos.y() < 0 || absPos.y() >= height) {
                valid = false;
                break;
            }

            // 检查与蛇头的距离
            int distance = qAbs(absPos.x() - snakeHead.x()) +
                           qAbs(absPos.y() - snakeHead.y());
            if (distance < 10) {
                valid = false;
                break;
            }

            // 检查是否与现有元素重叠
            if (snake.getSnake().contains(absPos) ||
                foodPoint(absPos) != food.end() ||
                deadPoint(absPos)) {
                valid = false;
                break;
            }

            // 检查是否与现有障碍物重叠
            for (const Obstacle& obs : obstacles) {
                if (obs.getLocation() == absPos) {
                    valid = false;
                    break;
                }
            }
        }

        // 如果有效，添加障碍物
        if (valid) {
            for (const QPoint& p : shape) {
                QPoint absPos = basePos + p - QPoint(minX, minY);
                obstacles.append(Obstacle(type, absPos));
            }
            return;
        }
    }

    // 多次尝试失败后，使用旧方法生成单个障碍物
    int x = rand() % width;
    int y = rand() % height;
    QPoint p(x, y);

    int distance = qAbs(p.x() - snakeHead.x()) + qAbs(p.y() - snakeHead.y());
    if (distance < 10) return;

    if (!deadPoint(p) &&
        !snake.getSnake().contains(p) &&
        foodPoint(p) == food.end() &&
        none_of(obstacles.begin(), obstacles.end(),
                [&p](const Obstacle& obs) { return obs.getLocation() == p; })) {
        obstacles.append(Obstacle(type, p));
    }
}



QList<Obstacle> Snakegame::getObstacles() const {
    return obstacles;
}

// void Snakegame::fireBullet() {
//     if (snake.getSnake().size() < 3) return;

//     // 消耗2格长度
//     QList<QPoint> body = snake.getSnake();
//     body.pop_back();
//     body.pop_back();
//     snake.reset(body.size(), body.front(), snake.getDirection());

//     // 创建子弹
//     Bullet bullet(snake.getHead(), snake.getDirection());
//     bullets.append(bullet);
// }

void Snakegame::fireBullet() {
    if (bulletCooldown > 0) return; // 冷却中不能发射

    // 创建子弹
    Bullet bullet(snake.getHead(), snake.getDirection());
    bullets.append(bullet);

    // 设置冷却时间
    bulletCooldown = BULLET_COOLDOWN_MAX;
}

QList<Bullet> Snakegame::getBullets() const {
    return bullets; // 返回子弹列表的副本
}
// void Bullet::move() {
//     // 双倍速度：每次移动两个单位
//     position += char2point[direction] * 2;
// }
