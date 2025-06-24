#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QList>
#include <QMouseEvent>
#include "SnakeGame.h"


enum SnakeBodyColor {
    GreenBody,
    BlueBody,
    YellowBody,
    RainbowBody
};

class GameRenderer : public QWidget {
    Q_OBJECT
public:
    enum HeadShape {
        CircleHead,
        HexagonHead,
        SquareHead,
        TriangleHead
    };

    enum BodyColorScheme {
        ClassicBlue,
        Rainbow,
        CyberPunk
    };
// 菜单项结构
    struct MenuItem {
        QString text;
        QRect rect;
        int key;
    };
    void setBodyColorScheme(BodyColorScheme scheme) { bodyColorScheme = scheme; }
    explicit GameRenderer(SnakeGame* game, QWidget* parent = nullptr);
    enum MenuState { MainMenu, DifficultyMenu, AppearanceMenu, MapMenu };
    
    // 添加成员函数声明
    void stopGameTimer();
    void startGameTimer();
    void updateSnakeColors();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    
private:
    SnakeGame* game; 
 // 添加成员变量
    BodyColorScheme bodyColorScheme = ClassicBlue;
    bool enableScaleDetail = true;
    bool enableMotionGlow = true;
    
    // 添加函数声明
    void drawHexagonHead(QPainter& painter, const QPoint& direction);
    void drawModernCircleHead(QPainter& painter, const QPoint& direction, const Snake& snake);
// 添加成员变量
    QColor snakeEyeColor = Qt::white;
    QColor snakeEyePupil = Qt::black;
    qreal headShine = 0.0;
    MenuState currentMenuState = MainMenu;
    void renderMenu(QPainter& painter);
    void renderPlaying(QPainter& painter);
    void renderGameOver(QPainter& painter);
    void handleMenuKeyPress(int key);
    void handlePlayingKeyPress(int key);
    void handleGameOverKeyPress(int key);
    
    // 绘制函数
    void drawSnakeHead(QPainter& painter, const QRect& headRect, const QPoint& direction);
    void drawSnakeSegment(QPainter& painter, const QRect& segmentRect, int segmentIndex, int totalSegments);
    void drawFood(QPainter& painter, const QRect& foodRect);
    void drawObstacle(QPainter& painter, const QRect& obstacleRect);
    void addMenuItem(QPainter& painter, const QString& text, int y, int key);
    QTimer* gameTimer;
    QColor snakeHeadColor= QColor(85, 170, 255);
    QColor snakeBodyColor;
    HeadShape selectedHeadShape = CircleHead;
    SnakeBodyColor selectedBodyColor = RainbowBody;
    
    // 食物动画相关
    qreal foodScale = 1.0;
    qreal foodRotation = 0.0;
    QTimer* foodAnimationTimer;
    QList<MenuItem> menuItems;
};

#endif // GAMERENDERER_H