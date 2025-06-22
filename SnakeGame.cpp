#include "SnakeGame.h"
#include <QKeyEvent>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>
#include <QtGlobal>
#include <cstdlib> // For srand and rand
#include <QUrl>
#include <QDir>
#include <ctime>   // For time

const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

SnakeGame::SnakeGame(QObject *parent)
    : QObject(parent), score(0), gameOverFlag(false), gameState(Menu), difficulty(1), elapsedTime(0), highScore(0) {
    loadHighScore();
    qDebug() << "High Score loaded:" << highScore;
    srand(time(0)); // Seed random number generator
    // Start a timer for elapsed time
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (gameState == Playing) {
            elapsedTime++;
        }
    });
    timer->start(1000); // Update every second
    selectedMap = EmptyMap; // Default map
}

void SnakeGame::startGame() {
    snake.reset();
    score = 0;
    gameOverFlag = false;
    elapsedTime = 0;
    gameState = Playing;

    // Clear previous obstacles
    obstacles.clear();

    // Load map based on selection
    if (selectedMap == ObstacleMap) {
        // Generate a smaller number of random obstacles
        int numObstacles = 50; // Reduced number of obstacles
        for (int i = 0; i < numObstacles; ++i) {
            int ox, oy;
            do {
                ox = rand() % GRID_WIDTH;
                oy = rand() % GRID_HEIGHT;
            } while (obstacles.contains(QPoint(ox, oy)) || (ox == GRID_WIDTH / 2 && oy == GRID_HEIGHT / 2)); // Avoid placing obstacles on snake start or other obstacles
            obstacles.append(QPoint(ox, oy));
        }
    }

    spawnFood();
    emit gameUpdated();

    // Add flag to wait for first move
    waitingForFirstMove = true;
    // Stop the game timer initially, it will be started on the first key press
    // Assuming gameTimer is a member variable in GameRenderer and controlled by SnakeGame
    // This requires a signal/slot connection or passing the timer object. Let's assume a signal for now.
    emit stopGameTimer();
}

void SnakeGame::restartGame() {
    gameState = Menu;
    emit gameUpdated(); // Emit signal to trigger repaint in GameRenderer
}

void SnakeGame::setSelectedMap(MapType mapType) {
    selectedMap = mapType;
}

void SnakeGame::changeDirection(int key) {
    if (gameState != Playing) return; // Only allow direction changes in Playing state

    // If waiting for the first move and a direction key is pressed
    if (waitingForFirstMove) {
        Snake::Direction initialDir;
        bool isDirectionKey = false;
        switch (key) {
            case Qt::Key_Up:
                initialDir = Snake::Up;
                isDirectionKey = true;
                break;
            case Qt::Key_Down:
                initialDir = Snake::Down;
                isDirectionKey = true;
                break;
            case Qt::Key_Left:
                initialDir = Snake::Left;
                isDirectionKey = true;
                break;
            case Qt::Key_Right:
                initialDir = Snake::Right;
                isDirectionKey = true;
                break;
            default:
                break;
        }

        if (isDirectionKey) {
            waitingForFirstMove = false;
            emit startGameTimer(); // Start the game timer
            snake.setDirection(initialDir);
            return; // Exit after handling the first move
        }
    }

    // Normal direction change logic
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
    if (gameState != Playing || waitingForFirstMove) return; // Don't update if waiting for first move

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

GameState SnakeGame::getGameState() const {
    return gameState;
}

void SnakeGame::setGameState(GameState state) {
    gameState = state;
}

int SnakeGame::getDifficulty() const {
    return difficulty;
}

void SnakeGame::setDifficulty(int difficulty) {
    this->difficulty = difficulty;
}

int SnakeGame::getElapsedTime() const {
    return elapsedTime;
}

int SnakeGame::getHighScore() const {
    return highScore;
}

void SnakeGame::setElapsedTime(int time) {
    this->elapsedTime = time;
}

void SnakeGame::checkCollisions() {
    QPoint head = snake.getHead();
    // Wall collision
    if (head.x() < 0 || head.x() >= GRID_WIDTH || head.y() < 0 || head.y() >= GRID_HEIGHT) {
        gameOverFlag = true;
        gameState = GameOver;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        emit gameOver();
        return;
    }
    // Self collision
    if (snake.checkSelfCollision()) {
        gameOverFlag = true;
        gameState = GameOver;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        emit gameOver();
        return;
    }
    // Obstacle collision
    if (obstacles.contains(head)) {
        gameOverFlag = true;
        gameState = GameOver;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
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
    food.respawn(GRID_WIDTH, GRID_HEIGHT, snake.getBody(), obstacles);
}

void SnakeGame::loadHighScore() {
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/snake_highscore.txt";
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        highScore = in.readLine().toInt();
        file.close();
    }
}

void SnakeGame::saveHighScore() {
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/snake_highscore.txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << highScore;
        file.close();
    }
}

const QList<QPoint>& SnakeGame::getObstacles() const {
    return obstacles;
}