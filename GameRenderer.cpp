#include "GameRenderer.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QConicalGradient>
#include <cmath>

const int CELL_SIZE = 25; // 增大单元格尺寸
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int HEAD_EYE_SIZE = 4; // 蛇眼大小

enum MenuState { MainMenu, DifficultyMenu, AppearanceMenu, MapMenu };
MenuState currentMenuState = MainMenu;

GameRenderer::GameRenderer(SnakeGame* game, QWidget* parent)
    : QWidget(parent), game(game) {
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 50); // 增加高度
    setFocusPolicy(Qt::StrongFocus);
    
    // 设置背景色
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(30, 30, 40));
    setAutoFillBackground(true);
    setPalette(pal);
    
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, [this, game]() {
        if (game->getGameState() == Playing) {
            game->update();
        }
    });
    
    // 食物动画定时器
    foodAnimationTimer = new QTimer(this);
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
    snakeHeadColor = QColor(0, 200, 0);
    snakeBodyColor = QColor(0, 150, 0);
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

void GameRenderer::renderMenu(QPainter& painter) {
    // 创建渐变背景
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(50, 50, 70));
    gradient.setColorAt(1, QColor(20, 20, 40));
    painter.fillRect(rect(), gradient);
    
    // 绘制标题
    painter.setPen(QColor(220, 220, 255));
    painter.setFont(QFont("Arial", 28, QFont::Bold));
    painter.drawText(rect().adjusted(0, 20, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "SNAKE GAME");
    
    // 绘制菜单选项
    painter.setFont(QFont("Arial", 16));
    painter.setPen(QColor(180, 255, 180));
    
    QString menuText;
    QString headShapeString;
    QString bodyColorString;
    
    switch (currentMenuState) {
        case MainMenu:
            menuText = QString("High Score: %1\n\n").arg(game->getHighScore());
            menuText += "Start Game (Press S)\n\n";
            menuText += "Select Difficulty (Press D)\n";
            menuText += "Select Appearance (Press A)\n";
            menuText += "Select Map (Press M)\n\n";
            menuText += "Press Q to Quit";
            break;
        case DifficultyMenu:
            menuText = "Select Difficulty:\n\n";
            menuText += "  1. Easy\n";
            menuText += "  2. Medium\n";
            menuText += "  3. Hard\n\n";
            menuText += "Back to Main Menu (Press B)";
            break;
        case AppearanceMenu:
            menuText = "Select Snake Appearance:\n\n";
            switch (selectedHeadShape) {
                case SquareHead: headShapeString = "Square"; break;
                case TriangleHead: headShapeString = "Triangle"; break;
                case CircleHead: headShapeString = "Circle"; break;
                case HexagonHead: headShapeString = "Hexagon"; break;
            }
            menuText += QString("  Head Shape: %1 (Press H to cycle)\n").arg(headShapeString);
            switch (selectedBodyColor) {
                case GreenBody: bodyColorString = "Green"; break;
                case BlueBody: bodyColorString = "Blue"; break;
                case YellowBody: bodyColorString = "Yellow"; break;
                case RainbowBody: bodyColorString = "Rainbow"; break;
            }
            menuText += QString("  Body Color: %1 (Press C to cycle)\n\n").arg(bodyColorString);
            menuText += "Back to Main Menu (Press B)";
            break;
        case MapMenu:
            menuText = "Select Map:\n\n";
            menuText += "  1. Empty Map (Press 1)\n";
            menuText += "  2. Obstacle Map (Press 2)\n\n";
            menuText += "Back to Main Menu (Press B)";
            break;
    }
    
    // 绘制菜单文本
    painter.drawText(rect().adjusted(0, 80, 0, 0), Qt::AlignTop | Qt::AlignHCenter, menuText);
    
    // 在底部绘制预览蛇
    painter.translate(width()/2, height() - 100);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制蛇身预览
    for (int i = 0; i < 5; i++) {
        QRect segmentRect(-i * 15, 0, 20, 20);
        drawSnakeSegment(painter, segmentRect, i, 5);
    }
    
    // 绘制蛇头预览
    QRect headRect(15, 0, 20, 20);
    drawSnakeHead(painter, headRect, QPoint(1, 0));
    
    // 绘制食物预览
    QRect foodRect(60, 0, 20, 20);
    drawFood(painter, foodRect);
    
    painter.resetTransform();
}

void GameRenderer::renderPlaying(QPainter& painter) {
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 绘制渐变背景
    QLinearGradient bgGradient(0, 0, width(), height());
    bgGradient.setColorAt(0, QColor(40, 40, 60));
    bgGradient.setColorAt(1, QColor(20, 20, 30));
    painter.fillRect(rect(), bgGradient);
    
    // 绘制网格
    painter.setPen(QPen(QColor(60, 60, 80, 100), 1));
    for (int i = 0; i <= GRID_WIDTH; ++i)
        painter.drawLine(i * CELL_SIZE, 0, i * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
    for (int i = 0; i <= GRID_HEIGHT; ++i)
        painter.drawLine(0, i * CELL_SIZE, GRID_WIDTH * CELL_SIZE, i * CELL_SIZE);
    
    // 绘制蛇
    const auto& body = game->getSnake().getBody();
    if (body.empty()) return;
    
    // 绘制蛇身
    for (size_t i = 1; i < body.size(); ++i) {
        QRect segmentRect(body[i].x() * CELL_SIZE, body[i].y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        drawSnakeSegment(painter, segmentRect, i, body.size());
    }
    
    // 绘制障碍物
    const auto& obstacles = game->getObstacles();
    for (const QPoint& obstacle : obstacles) {
        QRect obstacleRect(obstacle.x() * CELL_SIZE, obstacle.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        drawObstacle(painter, obstacleRect);
    }
    
    // 绘制蛇头
    QPoint headPos = body[0];
    QRect headRect(headPos.x() * CELL_SIZE, headPos.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    
    // 计算方向（修改后的部分）
    QPoint direction;
    if (body.size() > 1) {
        direction = body[0] - body[1];
    } else {
        // 根据当前方向设置
        switch (game->getSnake().getDirection()) {
            case Snake::Up:    direction = QPoint(0, -1); break;
            case Snake::Down:  direction = QPoint(0, 1); break;
            case Snake::Left:  direction = QPoint(-1, 0); break;
            case Snake::Right: direction = QPoint(1, 0); break;
        }
    }
    
    drawSnakeHead(painter, headRect, direction);
    
    // 绘制食物
    QPoint foodPos = game->getFood().getPosition();
    QRect foodRect(foodPos.x() * CELL_SIZE, foodPos.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    drawFood(painter, foodRect);
    
    // 绘制分数和时间
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
    painter.drawText(20, GRID_HEIGHT * CELL_SIZE + 30, QString("Score: %1").arg(game->getScore()));
    
    // 时间文本
    painter.drawText(width() - 120, GRID_HEIGHT * CELL_SIZE + 30, QString("Time: %1s").arg(game->getElapsedTime()));
}

void GameRenderer::renderGameOver(QPainter& painter) {
    // 渐变背景（暗红到暗紫）
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(120, 0, 0, 220));
    gradient.setColorAt(1, QColor(60, 0, 60, 220));
    painter.fillRect(rect(), gradient);

    // 添加纹理效果
    painter.setPen(QPen(QColor(255, 255, 255, 30), 2));
    for (int i = 0; i < width(); i += 30) {
        painter.drawLine(i, 0, i, height());
    }
    for (int i = 0; i < height(); i += 30) {
        painter.drawLine(0, i, width(), i);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 28, QFont::Bold));

    // 绘制游戏结束文本（带阴影效果）
    QRect textRect = rect().adjusted(0, 50, 0, 0);
    painter.setPen(QColor(0, 0, 0, 150));
    painter.drawText(textRect.translated(2, 2), Qt::AlignTop | Qt::AlignHCenter, "GAME OVER");
    painter.setPen(QColor(255, 100, 100));
    painter.drawText(textRect, Qt::AlignTop | Qt::AlignHCenter, "GAME OVER");

    // 绘制分数和时间（优化排版）
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
        painter.drawText(lineRect.translated(2, 2), Qt::AlignTop | Qt::AlignHCenter, infoText[i]);
        painter.setPen(i == 2 && game->getScore() > game->getHighScore() ? 
                      QColor(255, 215, 0) : QColor(200, 200, 255));
        painter.drawText(lineRect, Qt::AlignTop | Qt::AlignHCenter, infoText[i]);
    }

    // 绘制操作提示（使用图标式设计）
    QRect buttonRect(width()/2 - 100, height() - 120, 200, 40);
    painter.setBrush(QColor(80, 80, 80, 200));
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    painter.drawRoundedRect(buttonRect, 10, 10);
    painter.setPen(QColor(200, 230, 255));
    painter.drawText(buttonRect, Qt::AlignCenter, " Reset Game(R)");
    
    QRect quitRect(width()/2 - 100, height() - 70, 200, 40);
    painter.drawRoundedRect(quitRect, 10, 10);
    painter.drawText(quitRect, Qt::AlignCenter, " Quit(Q) ");
}

void GameRenderer::drawSnakeHead(QPainter& painter, const QRect& headRect, const QPoint& direction) {
    painter.save();
    
    // 设置头部颜色
    QColor headColor;
    switch (selectedBodyColor) {
        case GreenBody: headColor = QColor(0, 200, 0); break;
        case BlueBody: headColor = QColor(0, 150, 255); break;
        case YellowBody: headColor = QColor(255, 220, 0); break;
        case RainbowBody: headColor = QColor(0, 200, 150); break;
    }
    
    // 头部渐变
    QLinearGradient headGradient(headRect.topLeft(), headRect.bottomRight());
    headGradient.setColorAt(0, headColor.lighter(150));
    headGradient.setColorAt(1, headColor.darker(120));
    painter.setBrush(headGradient);
    painter.setPen(QPen(headColor.darker(150), 1.5));
    
    // 根据选择的形状绘制头部
    switch (selectedHeadShape) {
        case SquareHead:
            painter.drawRoundedRect(headRect, 15, 15);
            break;
            
        case TriangleHead: {
            QPolygon triangle;
            QPoint center = headRect.center();
            
            // 根据方向旋转三角形
            qreal angle = 0;
            if (direction.x() == 1) angle = 0;    // 右
            else if (direction.x() == -1) angle = 180; // 左
            else if (direction.y() == 1) angle = 90;  // 下
            else if (direction.y() == -1) angle = 270; // 上
            
            painter.translate(center);
            painter.rotate(angle);
            
            int size = CELL_SIZE / 2;
            triangle << QPoint(-size, -size)
                     << QPoint(size, 0)
                     << QPoint(-size, size);
            
            painter.drawPolygon(triangle);
            break;
        }
            
        case CircleHead:
            painter.drawEllipse(headRect);
            break;
            
        case HexagonHead: {
            QPolygon hexagon;
            int offset = CELL_SIZE / 4;
            hexagon << QPoint(headRect.left() + offset, headRect.top())
                    << QPoint(headRect.right() - offset, headRect.top())
                    << QPoint(headRect.right(), headRect.center().y())
                    << QPoint(headRect.right() - offset, headRect.bottom())
                    << QPoint(headRect.left() + offset, headRect.bottom())
                    << QPoint(headRect.left(), headRect.center().y());
            painter.drawPolygon(hexagon);
            break;
        }
    }
    
    // 绘制眼睛（除了三角形头部）
    if (selectedHeadShape != TriangleHead) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        
        int eyeSize = CELL_SIZE / 6;
        int eyeOffset = CELL_SIZE / 4;
        
        // 根据方向调整眼睛位置
        QPoint leftEye, rightEye;
        
        if (direction.x() == 1) { // 向右
            leftEye = headRect.topRight() + QPoint(-eyeOffset*2, eyeOffset);
            rightEye = headRect.bottomRight() + QPoint(-eyeOffset*2, -eyeOffset);
        } else if (direction.x() == -1) { // 向左
            leftEye = headRect.topLeft() + QPoint(eyeOffset*2, eyeOffset);
            rightEye = headRect.bottomLeft() + QPoint(eyeOffset*2, -eyeOffset);
        } else if (direction.y() == 1) { // 向下
            leftEye = headRect.bottomLeft() + QPoint(eyeOffset, -eyeOffset*2);
            rightEye = headRect.bottomRight() + QPoint(-eyeOffset, -eyeOffset*2);
        } else { // 向上
            leftEye = headRect.topLeft() + QPoint(eyeOffset, eyeOffset*2);
            rightEye = headRect.topRight() + QPoint(-eyeOffset, eyeOffset*2);
        }
        
        painter.drawEllipse(leftEye, eyeSize, eyeSize);
        painter.drawEllipse(rightEye, eyeSize, eyeSize);
        
        // 瞳孔
        painter.setBrush(Qt::black);
        painter.drawEllipse(leftEye, eyeSize/2, eyeSize/2);
        painter.drawEllipse(rightEye, eyeSize/2, eyeSize/2);
    }
    
    painter.restore();
}

void GameRenderer::drawSnakeSegment(QPainter& painter, const QRect& segmentRect, int segmentIndex, int totalSegments) {
    painter.save();
    
    // 设置身体颜色
    QColor segmentColor;
    
    switch (selectedBodyColor) {
        case GreenBody:
            segmentColor = QColor(0, 180, 0);
            break;
        case BlueBody:
            segmentColor = QColor(0, 120, 220);
            break;
        case YellowBody:
            segmentColor = QColor(220, 180, 0);
            break;
        case RainbowBody: {
            // 彩虹效果：根据位置改变颜色
            qreal hue = (segmentIndex * 360.0) / totalSegments;
            segmentColor = QColor::fromHsv(static_cast<int>(hue) % 360, 200, 200);
            break;
        }
    }
    
    // 身体渐变
    QLinearGradient bodyGradient(segmentRect.topLeft(), segmentRect.bottomRight());
    bodyGradient.setColorAt(0, segmentColor.lighter(130));
    bodyGradient.setColorAt(1, segmentColor.darker(130));
    
    painter.setBrush(bodyGradient);
    painter.setPen(QPen(segmentColor.darker(150), 1.5));
    
    // 绘制圆角矩形身体
    painter.drawRoundedRect(segmentRect, 10, 10);
    
    // 添加纹理效果
    painter.setPen(QPen(segmentColor.lighter(150), 1));
    painter.drawLine(segmentRect.center().x() - 3, segmentRect.center().y() - 3,
                     segmentRect.center().x() + 3, segmentRect.center().y() + 3);
    painter.drawLine(segmentRect.center().x() + 3, segmentRect.center().y() - 3,
                     segmentRect.center().x() - 3, segmentRect.center().y() + 3);
    
    painter.restore();
}

void GameRenderer::drawFood(QPainter& painter, const QRect& foodRect) {
    painter.save();
    
    // 应用食物动画效果
    painter.translate(foodRect.center());
    painter.scale(foodScale, foodScale);
    painter.rotate(foodRotation * 10);
    painter.translate(-foodRect.center());
    
    // 绘制苹果主体
    QRadialGradient foodGradient(foodRect.center(), foodRect.width()/2);
    foodGradient.setColorAt(0, QColor(255, 100, 100).lighter(150));
    foodGradient.setColorAt(1, QColor(220, 50, 50).darker(120));
    
    painter.setBrush(foodGradient);
    painter.setPen(QPen(QColor(180, 40, 40), 1.5));
    painter.drawEllipse(foodRect);
    
    // 绘制苹果叶子
    painter.setBrush(QColor(100, 220, 100));
    painter.setPen(QPen(QColor(80, 180, 80), 1));
    
    QPolygon leaf;
    leaf << foodRect.topRight() + QPoint(-5, 5) 
         << foodRect.topRight() + QPoint(5, -5) 
         << foodRect.topRight() + QPoint(15, 0);
    painter.drawPolygon(leaf);
    
    // 绘制苹果茎
    painter.setPen(QPen(QColor(150, 100, 50), 2));
    painter.drawLine(foodRect.topRight() + QPoint(-5, 5), 
                    foodRect.topRight() + QPoint(-10, -5));
    
    painter.restore();
}

void GameRenderer::drawObstacle(QPainter& painter, const QRect& obstacleRect) {
    painter.save();
    
    // 岩石纹理
    QLinearGradient obstacleGradient(obstacleRect.topLeft(), obstacleRect.bottomRight());
    obstacleGradient.setColorAt(0, QColor(100, 100, 120));
    obstacleGradient.setColorAt(1, QColor(70, 70, 90));
    
    painter.setBrush(obstacleGradient);
    painter.setPen(QPen(QColor(50, 50, 70), 1.5));
    painter.drawRoundedRect(obstacleRect, 5, 5);
    
    // 添加岩石纹理
    painter.setPen(QPen(QColor(120, 120, 140), 1));
    painter.drawLine(obstacleRect.topLeft() + QPoint(5, 5), obstacleRect.bottomRight() - QPoint(5, 5));
    painter.drawLine(obstacleRect.topRight() + QPoint(-5, 5), obstacleRect.bottomLeft() + QPoint(5, -5));
    
    painter.restore();
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
                default:
                    break;
            }
            break;
        case DifficultyMenu:
            switch (key) {
                case Qt::Key_1:
                    game->setDifficulty(1);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_2:
                    game->setDifficulty(2);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_3:
                    game->setDifficulty(3);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_Q:
                    qApp->quit();
                    break;
                default:
                    break;
            }
            break;
        case AppearanceMenu:
            switch (key) {
                case Qt::Key_H:
                    selectedHeadShape = static_cast<SnakeHeadShape>((selectedHeadShape + 1) % 4);
                    update();
                    break;
                case Qt::Key_C:
                    selectedBodyColor = static_cast<SnakeBodyColor>((selectedBodyColor + 1) % 4); // 更新为4种颜色
                    update();
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_Q:
                    qApp->quit();
                    break;
                default:
                    break;
            }
            break;
        case MapMenu:
            switch (key) {
                case Qt::Key_1:
                    game->setSelectedMap(MapType::EmptyMap);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_2:
                    game->setSelectedMap(MapType::ObstacleMap);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_B:
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_Q:
                    qApp->quit();
                    break;
                default:
                    break;
            }
            break;
    }
}

void GameRenderer::handlePlayingKeyPress(int key) {
    switch (key) {
        case Qt::Key_Up:
            game->changeDirection(Qt::Key_Up);
            break;
        case Qt::Key_Down:
            game->changeDirection(Qt::Key_Down);
            break;
        case Qt::Key_Left:
            game->changeDirection(Qt::Key_Left);
            break;
        case Qt::Key_Right:
            game->changeDirection(Qt::Key_Right);
            break;
        default:
            break;
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
        default:
            break;
    }
}

void GameRenderer::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    switch (game->getGameState()) {
        case Menu:
            renderMenu(painter);
            break;
        case Playing:
            renderPlaying(painter);
            break;
        case GameOver:
            renderGameOver(painter);
            break;
    }
}

void GameRenderer::keyPressEvent(QKeyEvent* event) {
    switch (game->getGameState()) {
        case Menu:
            handleMenuKeyPress(event->key());
            break;
        case Playing:
            handlePlayingKeyPress(event->key());
            break;
        case GameOver:
            handleGameOverKeyPress(event->key());
            break;
    }
}