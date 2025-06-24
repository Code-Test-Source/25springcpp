#include "Snake.h"
#include "SnakeGame.h"
#include "GameRenderer.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QConicalGradient>
#include <cmath>
#include <QPainterPath>
#include "Food.h"

const int CELL_SIZE = 25;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int HEAD_EYE_SIZE = 4;

GameRenderer::GameRenderer(SnakeGame* game, QWidget* parent)
    : QWidget(parent), game(game),
      gameTimer(new QTimer(this)),
      foodAnimationTimer(new QTimer(this))
      
{
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 50);
    setFocusPolicy(Qt::StrongFocus);
    // 初始化颜色
    snakeHeadColor = QColor(0, 200, 0);
    snakeBodyColor = QColor(0, 150, 0);
    // 设置背景色
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(30, 30, 40));
    setAutoFillBackground(true);
    setPalette(pal);
    // 游戏主定时器
    connect(gameTimer, &QTimer::timeout, this, [this]() {
    if (this->game->getGameState() == SnakeGame::GameState::Playing) 
    {
        this->game->update();
    }
    });
    // 食物动画定时器
    connect(foodAnimationTimer, &QTimer::timeout, this, [this]() {
        foodScale = 0.9 + 0.1 * std::sin(foodRotation);
        foodRotation += 0.1;
        update();
    });
    foodAnimationTimer->start(100);
    connect(game, &SnakeGame::gameUpdated, this, QOverload<>::of(&QWidget::update));
    connect(game, &SnakeGame::gameOver, this, QOverload<>::of(&QWidget::update));
    connect(game, &SnakeGame::stopGameTimer, this, &GameRenderer::stopGameTimer);
    connect(game, &SnakeGame::startGameTimer, this, &GameRenderer::startGameTimer);
    gameTimer->start(120);
}
void GameRenderer::stopGameTimer() {
    gameTimer->stop();
}
void GameRenderer::startGameTimer() {
    int interval = 100;
    switch (game->getDifficulty()) {
        case 1: interval = 150; break;
        case 2: interval = 100; break;
        case 3: interval = 50; break;
    }
    gameTimer->start(interval);
}
void GameRenderer::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    switch (game->getGameState()) {
        case SnakeGame::GameState::Menu:
            renderMenu(painter);
            break;
        case SnakeGame::GameState::Playing:
            renderPlaying(painter);
            break;
        case SnakeGame::GameState::GameOver:
            renderGameOver(painter);
            break;
    }
}
// 菜单渲染实现
void GameRenderer::renderMenu(QPainter& painter) {
    menuItems.clear();
    // 渐变背景
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(50, 50, 70));
    gradient.setColorAt(1, QColor(20, 20, 40));
    painter.fillRect(rect(), gradient);
    // 标题
    painter.setPen(QColor(220, 220, 255));
    painter.setFont(QFont("Arial", 28, QFont::Bold));
    painter.drawText(rect().adjusted(0, 20, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "SNAKE GAME");
    // 菜单项
    painter.setFont(QFont("Arial", 16));
    int yPos = 80;
    const int lineHeight = 30;
    // 高分显示
    painter.setPen(QColor(255, 215, 100));
    painter.drawText(rect().adjusted(0, yPos, 0, 0), Qt::AlignTop | Qt::AlignHCenter, 
                    QString("High Score: %1").arg(game->getHighScore()));
    yPos += lineHeight * 2;
    switch (currentMenuState) {
        case MainMenu:
            addMenuItem(painter, "Start Game (S)", yPos, Qt::Key_S);
            yPos += lineHeight;
            addMenuItem(painter, "Select Difficulty (D)", yPos, Qt::Key_D);
            yPos += lineHeight;
            addMenuItem(painter, "Select Appearance (A)", yPos, Qt::Key_A);
            yPos += lineHeight;
            addMenuItem(painter, "Select Map (M)", yPos, Qt::Key_M);
            yPos += lineHeight * 2;
            addMenuItem(painter, "Quit (Q)", yPos, Qt::Key_Q);
            break;
        case DifficultyMenu:
            painter.setPen(QColor(180, 255, 180));
            painter.drawText(rect().adjusted(0, yPos, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "Select Difficulty:");
            yPos += lineHeight * 2;
            addMenuItem(painter, "1. Easy", yPos, Qt::Key_1);
            yPos += lineHeight;
            addMenuItem(painter, "2. Medium", yPos, Qt::Key_2);
            yPos += lineHeight;
            addMenuItem(painter, "3. Hard", yPos, Qt::Key_3);
            yPos += lineHeight * 2;
            addMenuItem(painter, "Back to Main Menu (B)", yPos, Qt::Key_B);
            break;
        case AppearanceMenu: {
            QString headShapeString;
            switch (selectedHeadShape) {
                case SquareHead: headShapeString = "Square"; break;
                case TriangleHead: headShapeString = "Triangle"; break;
                case CircleHead: headShapeString = "Circle"; break;
                case HexagonHead: headShapeString = "Hexagon"; break;
            }
            QString bodyColorString;
            switch (selectedBodyColor) {
                case GreenBody: bodyColorString = "Green"; break;
                case BlueBody: bodyColorString = "Blue"; break;
                case YellowBody: bodyColorString = "Yellow"; break;
                case RainbowBody: bodyColorString = "Rainbow"; break;
            }
            painter.setPen(QColor(180, 255, 180));
            painter.drawText(rect().adjusted(0, yPos, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "Select Appearance:");
            yPos += lineHeight * 2;
            addMenuItem(painter, QString("Head: %1 (H)").arg(headShapeString), yPos, Qt::Key_H);
            yPos += lineHeight;
            addMenuItem(painter, QString("Color: %1 (C)").arg(bodyColorString), yPos, Qt::Key_C);
            yPos += lineHeight * 2;
            addMenuItem(painter, "Back to Main Menu (B)", yPos, Qt::Key_B);
            break;
        }
        case MapMenu:
            painter.setPen(QColor(180, 255, 180));
            painter.drawText(rect().adjusted(0, yPos, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "Select Map:");
            yPos += lineHeight * 2;
            addMenuItem(painter, "1. Empty Map (1)", yPos, Qt::Key_1);
            yPos += lineHeight;
            addMenuItem(painter, "2. Obstacle Map (2)", yPos, Qt::Key_2);
            yPos += lineHeight * 2;
            addMenuItem(painter, "Back to Main Menu (B)", yPos, Qt::Key_B);
            break;
    }
    // 预览
    painter.translate(width()/2, height() - 100);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制预览蛇
    for (int i = 0; i < 5; i++) {
        QRect segmentRect(-i * 15, 0, 20, 20);
        drawSnakeSegment(painter, segmentRect, i, 5);
    }
    
    // 绘制蛇头
    QRect headRect(15, 0, 20, 20);
    drawSnakeHead(painter, headRect, QPoint(1, 0));
    
    // 绘制食物
    QRect foodRect(60, 0, 20, 20);
    drawFood(painter, foodRect);
    
    painter.resetTransform();
}
// 游戏进行界面
void GameRenderer::renderPlaying(QPainter& painter) {
    painter.setRenderHint(QPainter::Antialiasing);
    // 背景
    QLinearGradient bgGradient(0, 0, width(), height());
    bgGradient.setColorAt(0, QColor(40, 40, 60));
    bgGradient.setColorAt(1, QColor(20, 20, 30));
    painter.fillRect(rect(), bgGradient);
    // 网格
    painter.setPen(QPen(QColor(60, 60, 80, 100), 1));
    for (int x = 0; x <= GRID_WIDTH; ++x)
        painter.drawLine(x * CELL_SIZE, 0, x * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
    for (int y = 0; y <= GRID_HEIGHT; ++y)
        painter.drawLine(0, y * CELL_SIZE, GRID_WIDTH * CELL_SIZE, y * CELL_SIZE);
    // 蛇身
    const auto& body = game->getSnake().getBody();
    for (size_t i = 1; i < body.size(); ++i) {
        QRect segmentRect(
            body[i].x() * CELL_SIZE,
            body[i].y() * CELL_SIZE,
            CELL_SIZE, CELL_SIZE
        );
        drawSnakeSegment(painter, segmentRect, i, body.size());
    }
    // 障碍物
    const auto& obstacles = game->getObstacles();
    for (const QPoint& obstacle : obstacles) {
        QRect obstacleRect(
            obstacle.x() * CELL_SIZE,
            obstacle.y() * CELL_SIZE,
            CELL_SIZE, CELL_SIZE
        );
        drawObstacle(painter, obstacleRect);
    }
    // 蛇头
    if (!body.empty()) {
        QPoint headPos = body[0];
        QRect headRect(
            headPos.x() * CELL_SIZE,
            headPos.y() * CELL_SIZE,
            CELL_SIZE, CELL_SIZE
        );
        QPoint direction;
    switch(game->getSnake().getDirection()) {
        case Snake::Up:    direction = QPoint(0, -1); break;
        case Snake::Down:  direction = QPoint(0, 1); break;
        case Snake::Left:  direction = QPoint(-1, 0); break;
        case Snake::Right: direction = QPoint(1, 0); break;
}
        drawSnakeHead(painter, headRect, direction);
    }
    // 食物
    QPoint foodPos = game->getFood().getPosition();
    QRect foodRect(
        foodPos.x() * CELL_SIZE,
        foodPos.y() * CELL_SIZE,
        CELL_SIZE, CELL_SIZE
    );
    drawFood(painter, foodRect);
    // 分数和时间
    painter.setPen(QColor(220, 220, 255));
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    
    // 分数背景
    painter.setBrush(QColor(30, 30, 50, 200));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(10, GRID_HEIGHT * CELL_SIZE + 10, 120, 30, 5, 5);
    
    // 时间背景
    painter.drawRoundedRect(width() - 130, GRID_HEIGHT * CELL_SIZE + 10, 120, 30, 5, 5);
    
    // 分数文本
    painter.setPen(QColor(255, 215, 100));
    painter.drawText(20, GRID_HEIGHT * CELL_SIZE + 30, 
                    QString("Score: %1").arg(game->getScore()));
    
    // 时间文本
    painter.drawText(width() - 120, GRID_HEIGHT * CELL_SIZE + 30, 
                    QString("Time: %1s").arg(game->getElapsedTime()));
}
// 游戏结束界面
void GameRenderer::renderGameOver(QPainter& painter) {
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(120, 0, 0, 220));
    gradient.setColorAt(1, QColor(60, 0, 60, 220));
    painter.fillRect(rect(), gradient);
    // 纹理效果
    painter.setPen(QPen(QColor(255, 255, 255, 30), 2));
    for (int x = 0; x < width(); x += 30) painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += 30) painter.drawLine(0, y, width(), y);
    // 游戏结束文字
    painter.setFont(QFont("Arial", 28, QFont::Bold));
    QRect textRect = rect().adjusted(0, 50, 0, 0);
    
    // 阴影
    painter.setPen(QColor(0, 0, 0, 150));
    painter.drawText(textRect.translated(2, 2), Qt::AlignTop | Qt::AlignHCenter, "GAME OVER");
    
    // 前景
    painter.setPen(QColor(255, 100, 100));
    painter.drawText(textRect, Qt::AlignTop | Qt::AlignHCenter, "GAME OVER");
    // 分数信息
    painter.setFont(QFont("Arial", 18));
    QRect infoRect = textRect.adjusted(0, 80, 0, 0);
    QStringList infoText = {
        QString("Score: %1").arg(game->getScore()),
        QString("Time: %1s").arg(game->getElapsedTime()),
        game->getScore() > game->getHighScore() ? 
            "NEW HIGH SCORE!" : 
            QString("High Score: %1").arg(game->getHighScore())
    };
    for (int i = 0; i < infoText.size(); ++i) {
        QRect lineRect = infoRect.adjusted(0, i * 40, 0, 0);
        painter.setPen(QColor(0, 0, 0, 150));
        painter.drawText(lineRect.translated(2, 2), Qt::AlignHCenter | Qt::AlignTop, infoText[i]);
        painter.setPen(i == 2 && game->getScore() > game->getHighScore() ? 
                      QColor(255, 215, 0) : QColor(200, 200, 255));
        painter.drawText(lineRect, Qt::AlignHCenter | Qt::AlignTop, infoText[i]);
    }
    // 操作按钮
    QRect resetRect(width()/2 - 100, height() - 120, 200, 40);
    painter.setBrush(QColor(80, 80, 80, 200));
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    painter.drawRoundedRect(resetRect, 10, 10);
    painter.setPen(QColor(200, 230, 255));
    painter.drawText(resetRect, Qt::AlignCenter, " Restart (R)");
    QRect quitRect(width()/2 - 100, height() - 70, 200, 40);
    painter.drawRoundedRect(quitRect, 10, 10);
    painter.drawText(quitRect, Qt::AlignCenter, " Quit (Q)");
}
// 输入处理
void GameRenderer::keyPressEvent(QKeyEvent* event) {
    switch (game->getGameState()) {
        case SnakeGame::GameState::Menu:
            handleMenuKeyPress(event->key());
            break;
        case SnakeGame::GameState::Playing:
            handlePlayingKeyPress(event->key());
            break;
        case SnakeGame::GameState::GameOver:
            handleGameOverKeyPress(event->key());
            break;
    }
}
void GameRenderer::handleMenuKeyPress(int key) {
    switch (currentMenuState) {
        case MainMenu:
            switch (key) {
                case Qt::Key_S:
                    game->startGame();
                    break;
                case Qt::Key_D:
                    currentMenuState = DifficultyMenu;
                    update();
                    break;
                case Qt::Key_A:
                    currentMenuState = AppearanceMenu;
                    update();
                    break;
                case Qt::Key_M:
                    currentMenuState = MapMenu;
                    update();
                    break;
                case Qt::Key_Q:
                    qApp->quit();
                    break;
            }
            break;
        case DifficultyMenu:
            switch (key) {
                case Qt::Key_1:
                    game->setDifficulty(1);
                    currentMenuState = MainMenu;
                    break;
                case Qt::Key_2:
                    game->setDifficulty(2);
                    currentMenuState = MainMenu;
                    break;
                case Qt::Key_3:
                    game->setDifficulty(3);
                    currentMenuState = MainMenu;
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    break;
            }
            update();
            break;
        case AppearanceMenu:
            switch (key) {
                case Qt::Key_H:
                    selectedHeadShape = static_cast<HeadShape>(
                        (selectedHeadShape + 1) % 4);
                    update();
                    break;
                case Qt::Key_C:
                    selectedBodyColor = static_cast<SnakeBodyColor>(
                        (selectedBodyColor + 1) % 4);
                    updateSnakeColors();
                    update();
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    update();
                    break;
            }
            break;
        case MapMenu:
            switch (key) {
                case Qt::Key_1:
                    game->loadMap(1);
                    currentMenuState = MainMenu;
                    break;
                case Qt::Key_2:
                    game->loadMap(2);
                    currentMenuState = MainMenu;
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    break;
            }
            update();
            break;
    }
}
void GameRenderer::handlePlayingKeyPress(int key) {
    // 将按键事件传递给SnakeGame，它会处理方向改变和第一个按键的逻辑
    game->changeDirection(key);
    // 同时，我们也需要处理ESC键
    if (key == Qt::Key_Escape) {
        game->setGameState(SnakeGame::GameState::Menu);
    }
}
void GameRenderer::handleGameOverKeyPress(int key) {
    switch (key) {
        case Qt::Key_R:
            game->restartGame();
            break;
        case Qt::Key_Q:
            qApp->quit();
            break;
    }
}
// 鼠标事件处理
void GameRenderer::mousePressEvent(QMouseEvent* event) {
    if (game->getGameState() == SnakeGame::GameState::Menu) {
        QPoint pos = event->pos();
        for (const MenuItem& item : menuItems) {
            if (item.rect.contains(pos)) {
                handleMenuKeyPress(item.key);
                return;
            }
        }
    }
    else if (game->getGameState() == SnakeGame::GameState::GameOver) {
        QPoint pos = event->pos();
        QRect resetRect(width()/2 - 100, height() - 120, 200, 40);
        if (resetRect.contains(pos)) {
            game->restartGame();
        }
        QRect quitRect(width()/2 - 100, height() - 70, 200, 40);
        if (quitRect.contains(pos)) {
            qApp->quit();
        }
    }
}
// 绘制辅助函数
void GameRenderer::addMenuItem(QPainter& painter, const QString& text, int y, int key) {
    QFontMetrics fm(painter.font());
    int textWidth = fm.horizontalAdvance(text);
    int x = (width() - textWidth) / 2;
    QRect itemRect(x - 10, y - 20, textWidth + 20, 30);
    painter.setBrush(QColor(80, 80, 120, 150));
    painter.setPen(QPen(QColor(180, 255, 180), 1));
    painter.drawRoundedRect(itemRect, 5, 5);
    painter.setPen(QColor(220, 255, 220));
    painter.drawText(itemRect, Qt::AlignCenter, text);
    menuItems.append({text, itemRect, key});
}
void GameRenderer::drawSnakeHead(QPainter& painter, const QRect& headRect, const QPoint& direction) {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 头部基础形状（圆角矩形）
    QPainterPath path;
    path.addRoundedRect(headRect, 8, 8);
    
    // 头部颜色渐变
    QLinearGradient grad(headRect.topLeft(), headRect.bottomRight());
    grad.setColorAt(0, snakeHeadColor.lighter(120));
    grad.setColorAt(1, snakeHeadColor.darker(120));
    painter.fillPath(path, grad);
    
    // 绘制眼睛
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    QPoint eyePos = headRect.center();
    
    // 根据方向调整眼睛位置
    if(direction == QPoint(1, 0)) { // 向右
        painter.drawEllipse(eyePos.x() + 4, eyePos.y() - 4, 5, 5);
        painter.drawEllipse(eyePos.x() + 4, eyePos.y() + 4, 5, 5);
    } else if(direction == QPoint(-1, 0)) { // 向左
        painter.drawEllipse(eyePos.x() - 9, eyePos.y() - 4, 5, 5);
        painter.drawEllipse(eyePos.x() - 9, eyePos.y() + 4, 5, 5);
    } else if(direction == QPoint(0, 1)) { // 向下
        painter.drawEllipse(eyePos.x() - 4, eyePos.y() + 4, 5, 5);
        painter.drawEllipse(eyePos.x() + 4, eyePos.y() + 4, 5, 5);
    } else { // 向上
        painter.drawEllipse(eyePos.x() - 4, eyePos.y() - 9, 5, 5);
        painter.drawEllipse(eyePos.x() + 4, eyePos.y() - 9, 5, 5);
    }
    
    // 绘制瞳孔
    painter.setBrush(Qt::black);
    if(direction == QPoint(1, 0)) { // 向右
        painter.drawEllipse(eyePos.x() + 5, eyePos.y() - 3, 2, 2);
        painter.drawEllipse(eyePos.x() + 5, eyePos.y() + 5, 2, 2);
    } else if(direction == QPoint(-1, 0)) { // 向左
        painter.drawEllipse(eyePos.x() - 7, eyePos.y() - 3, 2, 2);
        painter.drawEllipse(eyePos.x() - 7, eyePos.y() + 5, 2, 2);
    } else if(direction == QPoint(0, 1)) { // 向下
        painter.drawEllipse(eyePos.x() - 3, eyePos.y() + 5, 2, 2);
        painter.drawEllipse(eyePos.x() + 5, eyePos.y() + 5, 2, 2);
    } else { // 向上
        painter.drawEllipse(eyePos.x() - 3, eyePos.y() - 7, 2, 2);
        painter.drawEllipse(eyePos.x() + 5, eyePos.y() - 7, 2, 2);
    }
    
    painter.restore();
}
void GameRenderer::drawSnakeSegment(QPainter& painter, 
                                  const QRect& rect, 
                                  int segmentIndex, 
                                  int totalSegments) 
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 基础颜色（交替深浅）
    QColor baseColor = snakeBodyColor;
    if (segmentIndex % 2 == 0) {
        baseColor = baseColor.lighter(110);
    }
    
    // 创建渐变效果
    QLinearGradient bodyGradient(rect.topLeft(), rect.bottomRight());
    bodyGradient.setColorAt(0.0, baseColor.lighter(120));
    bodyGradient.setColorAt(0.5, baseColor);
    bodyGradient.setColorAt(1.0, baseColor.darker(120));
    
    // 绘制圆角矩形
    painter.setPen(QPen(baseColor.darker(150), 1));
    painter.setBrush(bodyGradient);
    painter.drawRoundedRect(rect, 5, 5);
    
    // 添加立体效果
    painter.setPen(QPen(QColor(255, 255, 255, 50), 1));
    painter.drawLine(rect.topLeft() + QPoint(2,2), rect.topRight() + QPoint(-2,2));
    painter.drawLine(rect.topLeft() + QPoint(2,2), rect.bottomLeft() + QPoint(2,-2));
    
    painter.restore();
}
void GameRenderer::drawFood(QPainter& painter, const QRect& rect) {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 基础形状（圆形）
    QPainterPath path;
    path.addEllipse(rect);
    
    // 动画效果（呼吸效果）
    qreal scaleFactor = 0.8 + 0.2 * std::sin(foodRotation);
    painter.translate(rect.center());
    painter.scale(scaleFactor, scaleFactor);
    painter.translate(-rect.center());
    
    // 颜色渐变（红色系）
    QRadialGradient grad(rect.center(), rect.width()/2);
    grad.setColorAt(0, QColor(255, 100, 100));
    grad.setColorAt(1, QColor(200, 50, 50));
    
    painter.setPen(QPen(QColor(150, 0, 0), 1));
    painter.setBrush(grad);
    painter.drawEllipse(rect);
    
    // 添加高光
    painter.setBrush(QColor(255, 255, 255, 100));
    painter.drawEllipse(rect.adjusted(2, 2, -2, -2));
    
    painter.restore();
}
void GameRenderer::drawObstacle(QPainter& painter, const QRect& rect) {
    painter.setBrush(QColor(100, 100, 100));
    painter.setPen(QColor(50, 50, 50));
    
    // 绘制岩石效果
    painter.drawRoundedRect(rect, 5, 5);
    
    // 添加纹理
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    painter.drawLine(rect.left() + 5, rect.top() + 5, rect.right() - 5, rect.bottom() - 5);
    painter.drawLine(rect.left() + 5, rect.bottom() - 5, rect.right() - 5, rect.top() + 5);
}
void GameRenderer::updateSnakeColors() {
    switch (selectedBodyColor) {
        case GreenBody:
            snakeHeadColor = QColor(100, 200, 100);
            snakeBodyColor = QColor(0, 150, 0);
            break;
        case BlueBody:
            snakeHeadColor = QColor(100, 100, 200);
            snakeBodyColor = QColor(0, 0, 150);
            break;
        case YellowBody:
            snakeHeadColor = QColor(200, 200, 100);
            snakeBodyColor = QColor(150, 150, 0);
            break;
        case RainbowBody:
            snakeHeadColor = QColor(200, 0, 0);
            snakeBodyColor = QColor(150, 0, 0);
            break;
    }
}
// 新增现代圆形蛇头绘制方法
void GameRenderer::drawHexagonHead(QPainter& painter, const QPoint& direction) {
     Q_UNUSED(direction); // 避免未使用参数警告
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 六边形头部绘制逻辑
    QPolygon hexagon;
    const int size = 12;
    hexagon << QPoint(-size, 0)
            << QPoint(-size/2, -size*0.866)
            << QPoint(size/2, -size*0.866)
            << QPoint(size, 0)
            << QPoint(size/2, size*0.866)
            << QPoint(-size/2, size*0.866);
    
    painter.setBrush(snakeHeadColor);
    painter.drawPolygon(hexagon);
    
    painter.restore();
}
void GameRenderer::drawModernCircleHead(QPainter& painter, const QPoint& direction, const Snake& snake) {
    // 头部主体
    QRadialGradient grad(0, 0, 15);
    grad.setColorAt(0, snakeHeadColor.lighter(120));
    grad.setColorAt(1, snakeHeadColor.darker(120));
    painter.setBrush(grad);
    painter.drawEllipse(-12, -12, 24, 24);
    // 3D 高光效果
    painter.setBrush(QColor(255, 255, 255, 150));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(-8, -10, 8, 8);
    // 动态眼睛方向计算
    QPoint eyeBasePos;
    switch (snake.getDirection()) {
    case Snake::Up:    eyeBasePos = QPoint(4, -4); break;
    case Snake::Down:  eyeBasePos = QPoint(4, 4); break;
    case Snake::Left:  eyeBasePos = QPoint(-4, 0); break;
    case Snake::Right: eyeBasePos = QPoint(4, 0); break;
    }
    // 绘制眼睛（带瞳孔）
    painter.setBrush(snakeEyeColor);
    painter.setPen(Qt::NoPen);
    
    // 主眼
    painter.drawEllipse(eyeBasePos.x() - 3, eyeBasePos.y() - 3, 6, 6);
    
    // 瞳孔（带动态偏移）
    qreal pupilOffset = 1.5;
    painter.setBrush(snakeEyePupil);
    painter.drawEllipse(eyeBasePos.x() - 1 + pupilOffset, 
                       eyeBasePos.y() - 1 + pupilOffset, 2, 2);
    // 第二只眼睛（根据方向对称）
    if (direction.x() != 0) { // 左右方向
        painter.drawEllipse(eyeBasePos.x() - 3, -eyeBasePos.y() - 3, 6, 6);
        painter.setBrush(snakeEyePupil);
        painter.drawEllipse(eyeBasePos.x() - 1 + pupilOffset, 
                           -eyeBasePos.y() - 1 + pupilOffset, 2, 2);
    } else { // 上下方向
        painter.drawEllipse(-eyeBasePos.x() - 3, eyeBasePos.y() - 3, 6, 6);
        painter.setBrush(snakeEyePupil);
        painter.drawEllipse(-eyeBasePos.x() - 1 + pupilOffset, 
                           eyeBasePos.y() - 1 + pupilOffset, 2, 2);
    }
}