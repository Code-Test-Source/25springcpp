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
    : QObject(parent), score(0), gameOverFlag(false), gameState(Menu), difficulty(1), elapsedTime(0), highScore(0){
    loadHighScore();
    qDebug() << "High Score loaded:" << highScore;
    srand(time(0)); // Seed random number generator
    // Start a timer for elapsed time
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
    // 只有在游戏进行中且已按下第一个按键时才计时
    if (gameState == Playing && !waitingForFirstMove) {
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
    obstacles.clear(); // 清空障碍物

    // 根据选择的地图类型生成障碍物
    if (selectedMap == ObstacleMap) {
        // 生成边界障碍物
        for (int x = 0; x < GRID_WIDTH; ++x) {
            obstacles.append(QPoint(x, 0));               // 上边界
            obstacles.append(QPoint(x, GRID_HEIGHT - 1)); // 下边界
        }
        for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
            obstacles.append(QPoint(0, y));              // 左边界
            obstacles.append(QPoint(GRID_WIDTH - 1, y)); // 右边界
        }

        // 添加一些内部障碍物（数量适中）
        int numObstacles = 15; // 例如15个内部障碍物
        for (int i = 0; i < numObstacles; ++i) {
            int ox, oy;
            do {
                // 确保障碍物不在边界（因为边界已经添加）和蛇起始位置
                ox = 1 + rand() % (GRID_WIDTH - 2);
                oy = 1 + rand() % (GRID_HEIGHT - 2);
            } while (
                obstacles.contains(QPoint(ox, oy)) || 
                (ox == GRID_WIDTH / 2 && oy == GRID_HEIGHT / 2) // 蛇起始位置（中心）
            );
            obstacles.append(QPoint(ox, oy));
        }
    }

    spawnFood();
    emit gameUpdated();
    waitingForFirstMove = true;
    elapsedTime = 0; 
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
            elapsedTime = 0; 
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

const Food& SnakeGame::getFood() const {
    return food;
}

SnakeGame::GameState SnakeGame::getGameState() const {
    return gameState;
}

void SnakeGame::setGameState(GameState state) {
    gameState = state;
}

void SnakeGame::setSnakeDirection(Snake::Direction dir) {
    snake.setDirection(dir);
}

void SnakeGame::loadMap(int mapIndex) {
    // 设置地图类型
    if (mapIndex == 0) {
        selectedMap = EmptyMap;
    } else if (mapIndex == 1) {
        selectedMap = ObstacleMap;
    }
    
    // 重置游戏状态
    restartGame();
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