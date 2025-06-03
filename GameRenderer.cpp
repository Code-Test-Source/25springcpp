#include "GameRenderer.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

const int CELL_SIZE = 20;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

enum MenuState { MainMenu, DifficultyMenu, AppearanceMenu, MapMenu };
MenuState currentMenuState = MainMenu;

GameRenderer::GameRenderer(SnakeGame* game, QWidget* parent)
    : QWidget(parent), game(game) {
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 40);
    setFocusPolicy(Qt::StrongFocus);
    gameTimer = new QTimer(this); // Initialize the member variable
    connect(gameTimer, &QTimer::timeout, this, [this, game]() {
        if (game->getGameState() == Playing) {
            game->update();
        }
    });
    connect(game, &SnakeGame::gameUpdated, this, QOverload<>::of(&QWidget::update));
    connect(game, &SnakeGame::gameOver, this, QOverload<>::of(&QWidget::update));
    connect(game, &SnakeGame::stopGameTimer, this, &GameRenderer::stopGameTimer);
    connect(game, &SnakeGame::startGameTimer, this, &GameRenderer::startGameTimer);
    gameTimer->start(120); // Default speed, will adjust based on difficulty
    snakeHeadColor = QColor(0, 180, 0);
    snakeBodyColor = QColor(0, 120, 0);
    selectedHeadShape = SquareHead; // Default shape
    selectedBodyColor = GreenBody; // Default color
}

void GameRenderer::stopGameTimer() {
    gameTimer->stop();
}

void GameRenderer::startGameTimer() {
    gameTimer->start(120); // Start with default speed, difficulty will adjust it later
}

void GameRenderer::renderMenu(QPainter& painter) {
    painter.fillRect(rect(), QColor(240, 240, 240));
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 20, QFont::Bold));

    QString menuText;
    QString headShapeString;
    QString bodyColorString;

    switch (currentMenuState) {
        case MainMenu:
            menuText = QString("Snake Game\nHigh Score: %1\n\n")
                           .arg(game->getHighScore());
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

    painter.drawText(rect(), Qt::AlignCenter, menuText);
}

void GameRenderer::renderPlaying(QPainter& painter) {
    // Draw background
    painter.fillRect(rect(), QColor(240, 240, 240));
    // Draw grid
    painter.setPen(QColor(220, 220, 220));
    for (int i = 0; i <= GRID_WIDTH; ++i)
        painter.drawLine(i * CELL_SIZE, 0, i * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
    for (int i = 0; i <= GRID_HEIGHT; ++i)
        painter.drawLine(0, i * CELL_SIZE, GRID_WIDTH * CELL_SIZE, i * CELL_SIZE);
    // Draw snake
    const auto& body = game->getSnake().getBody();
    if (body.empty()) return; // Don't draw if snake is empty

    // Set body color
    QColor currentBodyColor;
    switch (selectedBodyColor) {
        case GreenBody: currentBodyColor = QColor(0, 120, 0); break;
        case BlueBody: currentBodyColor = QColor(0, 0, 180); break;
        case YellowBody: currentBodyColor = QColor(180, 180, 0); break;
    }
    painter.setBrush(currentBodyColor);
    painter.setPen(Qt::NoPen);

    // Draw body segments
    for (size_t i = 1; i < body.size(); ++i) {
        QRect cell(body[i].x() * CELL_SIZE, body[i].y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        painter.drawRect(cell);
    }

    // Draw obstacles
    painter.setBrush(QColor(100, 100, 100)); // Obstacle color
    const auto& obstacles = game->getObstacles();
    for (const QPoint& obstacle : obstacles) {
        QRect cell(obstacle.x() * CELL_SIZE, obstacle.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        painter.drawRect(cell);
    }

    // Draw head
    QPoint headPos = body[0];
    QRect headRect(headPos.x() * CELL_SIZE, headPos.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    QColor currentHeadColor;
    switch (selectedBodyColor) { // Use a slightly different shade for the head based on body color
        case GreenBody: currentHeadColor = QColor(0, 180, 0); break;
        case BlueBody: currentHeadColor = QColor(0, 0, 255); break;
        case YellowBody: currentHeadColor = QColor(255, 255, 0); break;
    }
    painter.setBrush(currentHeadColor);

    switch (selectedHeadShape) {
        case SquareHead:
            painter.drawRect(headRect);
            break;
        case TriangleHead:
            // Drawing a simple triangle for now, can be improved
            {
                QPoint p1, p2, p3;
                // This is a basic triangle, need to adjust based on direction later
                p1 = headRect.topLeft();
                p2 = headRect.topRight();
                p3 = headRect.bottomLeft(); // Simple example, not direction aware
                QPolygon triangle;
                triangle << p1 << p2 << p3;
                painter.drawPolygon(triangle);
            }
            break;
        case CircleHead:
            painter.drawEllipse(headRect);
            break;
        case HexagonHead:
            // Drawing a simple hexagon, can be improved
            {
                QPolygon hexagon;
                // Basic hexagon points, not direction aware
                hexagon << headRect.topLeft() + QPoint(CELL_SIZE/4, 0)
                        << headRect.topRight() - QPoint(CELL_SIZE/4, 0)
                        << headRect.topRight()
                        << headRect.bottomRight() - QPoint(CELL_SIZE/4, 0)
                        << headRect.bottomLeft() + QPoint(CELL_SIZE/4, 0)
                        << headRect.bottomLeft();
                painter.drawPolygon(hexagon);
            }
            break;
    }

    // Draw food
    QPoint foodPos = game->getFood().getPosition();
    QRect foodRect(foodPos.x() * CELL_SIZE, foodPos.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    painter.setBrush(QColor(220, 50, 50));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(foodRect);
    // Draw score
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(10, GRID_HEIGHT * CELL_SIZE + 30, QString("Score: %1").arg(game->getScore()));
    // Draw timer
    painter.drawText(width() - 100, GRID_HEIGHT * CELL_SIZE + 30, QString("Time: %1s").arg(game->getElapsedTime()));
}

void GameRenderer::renderGameOver(QPainter& painter) {
    painter.fillRect(rect(), QColor(240, 240, 240));
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, QString("Game Over!\nScore: %1\nTime: %2s\n\nPress R to Restart\nPress Q to Quit").arg(game->getScore()).arg(game->getElapsedTime()));
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
                    update(); // Redraw menu
                    break;
                case Qt::Key_A:
                    currentMenuState = AppearanceMenu;
                    update(); // Redraw menu
                    break;
                case Qt::Key_M:
                    currentMenuState = MapMenu;
                    update(); // Redraw menu
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
                    game->setDifficulty(1); // Easy
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_2:
                    game->setDifficulty(2); // Medium
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_3:
                    game->setDifficulty(3); // Hard
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
                    // Cycle head shape
                    selectedHeadShape = static_cast<SnakeHeadShape>((selectedHeadShape + 1) % 4);
                    update();
                    break;
                case Qt::Key_C:
                    // Cycle body color
                    selectedBodyColor = static_cast<SnakeBodyColor>((selectedBodyColor + 1) % 3);
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
                    // Select Empty Map
                    game->setSelectedMap(MapType::EmptyMap);
                    currentMenuState = MainMenu;
                    update();
                    break;
                case Qt::Key_2:
                    // Select Obstacle Map
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