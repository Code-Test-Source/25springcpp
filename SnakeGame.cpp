#include "SnakeGame.h"
#include <QKeyEvent>

const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

SnakeGame::SnakeGame(QObject *parent)
    : QObject(parent), score(0), gameOverFlag(false) {
    startGame();
}

void SnakeGame::startGame() {
    snake.reset();
    score = 0;
    gameOverFlag = false;
    spawnFood();
    emit gameUpdated();
}

void SnakeGame::restartGame() {
    startGame();
}

void SnakeGame::changeDirection(int key) {
    Snake::Direction dir = snake.getDirection();
    switch (key) {
        case Qt::Key_Up:
            dir = Snake::Up;
            break;
        case Qt::Key_Down:
            dir = Snake::Down;
            break;
        case Qt::Key_Left:
            dir = Snake::Left;
            break;
        case Qt::Key_Right:
            dir = Snake::Right;
            break;
        default:
            return;
    }
    snake.setDirection(dir);
}

void SnakeGame::update() {
    if (gameOverFlag) return;
    snake.move();
    checkCollisions();
    emit gameUpdated();
}

int SnakeGame::getScore() const {
    return score;
}

bool SnakeGame::isGameOver() const {
    return gameOverFlag;
}

const Snake& SnakeGame::getSnake() const {
    return snake;
}

const Food& SnakeGame::getFood() const {
    return food;
}

void SnakeGame::checkCollisions() {
    QPoint head = snake.getHead();
    // Wall collision
    if (head.x() < 0 || head.x() >= GRID_WIDTH || head.y() < 0 || head.y() >= GRID_HEIGHT) {
        gameOverFlag = true;
        emit gameOver();
        return;
    }
    // Self collision
    if (snake.checkSelfCollision()) {
        gameOverFlag = true;
        emit gameOver();
        return;
    }
    // Food collision
    if (head == food.getPosition()) {
        snake.grow();
        score += 10;
        spawnFood();
    }
}

void SnakeGame::spawnFood() {
    food.respawn(GRID_WIDTH, GRID_HEIGHT, snake.getBody());
}