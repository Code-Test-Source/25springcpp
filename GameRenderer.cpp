#include "GameRenderer.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

const int CELL_SIZE = 20;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

GameRenderer::GameRenderer(SnakeGame* game, QWidget* parent)
    : QWidget(parent), game(game) {
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 40);
    setFocusPolicy(Qt::StrongFocus);
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, game]() {
        if (!game->isGameOver()) {
            game->update();
        }
    });
    connect(game, &SnakeGame::gameUpdated, this, QOverload<>::of(&QWidget::update));
    connect(game, &SnakeGame::gameOver, this, QOverload<>::of(&QWidget::update));
    timer->start(120);
}

void GameRenderer::paintEvent(QPaintEvent*) {
    QPainter painter(this);
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
    for (size_t i = 0; i < body.size(); ++i) {
        QRect cell(body[i].x() * CELL_SIZE, body[i].y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        if (i == 0) {
            painter.setBrush(QColor(0, 180, 0)); // Head
        } else {
            painter.setBrush(QColor(0, 120, 0)); // Body
        }
        painter.setPen(Qt::NoPen);
        painter.drawRect(cell);
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
    // Draw game over
    if (game->isGameOver()) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Game Over! Press R to restart.");
    }
}

void GameRenderer::keyPressEvent(QKeyEvent* event) {
    if (game->isGameOver() && (event->key() == Qt::Key_R)) {
        game->restartGame();
        return;
    }
    switch (event->key()) {
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
            QWidget::keyPressEvent(event);
    }
}