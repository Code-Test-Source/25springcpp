#ifndef GAMERENDERER_H
#define GAMERENDERER_H
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include "SnakeGame.h"

enum SnakeHeadShape {
    SquareHead,
    TriangleHead,
    CircleHead,
    HexagonHead
};

enum SnakeBodyColor {
    GreenBody,
    BlueBody,
    YellowBody,
    RainbowBody  // 新增彩虹色
};

class GameRenderer : public QWidget {
    Q_OBJECT
public:
    explicit GameRenderer(SnakeGame* game, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    void renderMenu(QPainter& painter);
    void renderPlaying(QPainter& painter);
    void renderGameOver(QPainter& painter);
    void handleMenuKeyPress(int key);
    void handlePlayingKeyPress(int key);
    void handleGameOverKeyPress(int key);
    
    // 绘制蛇头
    void drawSnakeHead(QPainter& painter, const QRect& headRect, const QPoint& direction);
    // 绘制蛇身段
    void drawSnakeSegment(QPainter& painter, const QRect& segmentRect, int segmentIndex, int totalSegments);
    // 绘制食物
    void drawFood(QPainter& painter, const QRect& foodRect);
    // 绘制障碍物
    void drawObstacle(QPainter& painter, const QRect& obstacleRect);
    
    SnakeGame* game;
    QTimer* gameTimer;
    QColor snakeHeadColor;
    QColor snakeBodyColor;
    SnakeHeadShape selectedHeadShape = CircleHead; // 默认改为圆形头部
    SnakeBodyColor selectedBodyColor = RainbowBody; // 默认改为彩虹色
    
    void stopGameTimer();
    void startGameTimer();
    
    // 食物动画相关
    qreal foodScale = 1.0;
    qreal foodRotation = 0.0;
    QTimer* foodAnimationTimer;
};
#endif // GAMERENDERER_H