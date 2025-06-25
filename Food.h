#ifndef FOOD_H
#define FOOD_H

#include <deque>
#include <QPoint>
#include <QList>

// Food 类：表示游戏中的食物对象，负责食物的位置管理与重新生成
class Food {
public:
    // 构造函数：初始化食物
    Food();

    /**
     * 重新生成食物位置
     * @param width 地图宽度
     * @param height 地图高度
     * @param snakeBody 当前蛇的身体，用于避免生成在蛇身上
     * @param obstacles 当前地图中的障碍物，用于避免生成在障碍物位置
     */
    void respawn(int width, int height, const std::deque<QPoint>& snakeBody, const QList<QPoint>& obstacles);

    // 获取当前食物的位置
    QPoint getPosition() const;

private:
    QPoint position; // 当前食物的坐标
};

#endif // FOOD_H
