#ifndef SNAKE_H
#define SNAKE_H

#include <QPoint>
#include <deque>

// Snake 类：表示贪吃蛇对象，负责管理蛇的身体、移动、增长与自撞检测等
class Snake {
public:
    // 枚举类型：蛇的移动方向
    enum Direction { Up, Down, Left, Right };

    // 构造函数：初始化蛇的位置与方向
    Snake();

    // 重置蛇的状态（用于重新开始游戏）
    void reset();

    // 设置蛇的移动方向
    void setDirection(Direction dir);

    // 获取当前的移动方向
    Direction getDirection() const;

    // 移动蛇体（添加新头部并移除尾部或增长）
    void move();

    // 吃食物后调用，使蛇在下一次移动时增长一节
    void grow();

    // 检查蛇是否与自身发生碰撞（头部撞到身体）
    bool checkSelfCollision() const;

    // 获取蛇的身体部分（返回常引用）
    const std::deque<QPoint>& getBody() const;

    // 获取蛇头位置（即身体的前端）
    QPoint getHead() const;

private:
    std::deque<QPoint> body;    // 使用双端队列存储蛇身的各个关节坐标，front 是蛇头，back 是蛇尾
    Direction direction;        // 当前移动方向
    bool growFlag;              // 是否在下一次移动时增长（由吃食物触发）
};

#endif // SNAKE_H
