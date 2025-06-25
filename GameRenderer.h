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

// 枚举：蛇体颜色类型（用于自定义皮肤）
enum SnakeBodyColor {
    GreenBody,
    BlueBody,
    YellowBody,
    RainbowBody
};

// GameRenderer 类：游戏的图形界面渲染类，负责所有画面绘制与界面交互
class GameRenderer : public QWidget {
    Q_OBJECT
public:
    enum GameState { Menu, Playing, GameOver };
    // 蛇头样式枚举
    enum HeadShape {
        CircleHead,     // 圆形蛇头
        HexagonHead,    // 六边形蛇头
        SquareHead,     // 正方形蛇头
        TriangleHead    // 三角形蛇头
    };

    // 蛇体配色方案枚举
    enum BodyColorScheme {
        ClassicBlue,
        Rainbow,
        CyberPunk
    };

    // 菜单项结构体：用于管理主菜单中的选项项与交互区域
    struct MenuItem {
        QString text;   // 显示文本
        QRect rect;     // 点击区域
        int key;        // 关联键值（用于处理按键控制菜单）
    };

    // 构造函数：接收 SnakeGame 指针与父窗口指针
    explicit GameRenderer(SnakeGame* game, QWidget* parent = nullptr);

    // 设置当前使用的配色方案
    void setBodyColorScheme(BodyColorScheme scheme) { bodyColorScheme = scheme; }

    // 菜单界面状态枚举
    enum MenuState {
        MainMenu,        // 主菜单
        DifficultyMenu,  // 难度选择菜单
        AppearanceMenu,  // 外观设置菜单
        MapMenu          // 地图选择菜单
    };

    // 启动与停止游戏主循环的定时器（与 SnakeGame 配合）
    void startGameTimer();
    void stopGameTimer();

    // 更新蛇颜色样式（响应用户设置）
    void updateSnakeColors();

protected:
    // Qt事件：窗口绘制
    void paintEvent(QPaintEvent* event) override;

    // Qt事件：键盘输入处理
    void keyPressEvent(QKeyEvent* event) override;

    // Qt事件：鼠标点击处理
    void mousePressEvent(QMouseEvent* event) override;

private:
    // === 蛇头绘制函数 ===
    void drawCircleHead(QPainter& painter);
    void drawSquareHead(QPainter& painter);
    void drawTriangleHead(QPainter& painter);
    void drawHexagonHead(QPainter& painter);
    void drawHexagonHead(QPainter& painter, const QPoint& direction);
    void drawModernCircleHead(QPainter& painter, const QPoint& direction, const Snake& snake);
    void drawEyes(QPainter& painter, const QPoint& offset);

    // === 蛇体绘制函数 ===
    void drawSnakeSegment(QPainter& painter, const QRect& segmentRect, int segmentIndex, int totalSegments);
    void drawSnakeHead(QPainter& painter, const QRect& headRect, const QPoint& direction);

    // === 其他元素绘制 ===
    void drawFood(QPainter& painter, const QRect& foodRect);
    void drawObstacle(QPainter& painter, const QRect& obstacleRect);

    // === 菜单渲染与交互 ===
    void renderMenu(QPainter& painter);
    void renderPlaying(QPainter& painter);
    void renderGameOver(QPainter& painter);
    void addMenuItem(QPainter& painter, const QString& text, int y, int key);
    void handleMenuKeyPress(int key);
    void handlePlayingKeyPress(int key);
    void handleGameOverKeyPress(int key);

    // === 成员变量 ===
    SnakeGame* game;                      // 游戏主逻辑控制器
    QTimer* gameTimer;                    // 游戏刷新计时器
    QTimer* foodAnimationTimer;          // 食物动画计时器

    BodyColorScheme bodyColorScheme = ClassicBlue; // 当前选中的身体配色
    HeadShape selectedHeadShape = CircleHead;      // 当前蛇头形状
    SnakeBodyColor selectedBodyColor = RainbowBody;// 当前蛇体颜色
    MenuState currentMenuState = MainMenu;         // 当前菜单状态

    // 控制项：视觉细节开关
    bool enableScaleDetail = true;
    bool enableMotionGlow = true;

    // 蛇眼配色
    QColor snakeEyeColor = Qt::white;
    QColor snakeEyePupil = Qt::black;

    // 蛇头高光程度
    qreal headShine = 0.0;

    // 蛇体与蛇头配色
    QColor snakeHeadColor = QColor(85, 170, 255);
    QColor snakeBodyColor;

    // 食物动画参数
    qreal foodScale = 1.0;       // 食物缩放（呼吸动画）
    qreal foodRotation = 0.0;    // 食物旋转角度

    // 菜单项列表（用于渲染与点击响应）
    QList<MenuItem> menuItems;
};

#endif // GAMERENDERER_H
