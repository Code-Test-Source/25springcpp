#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QObject>
#include "Snake.h"
#include "Food.h"

class SnakeGame : public QObject {
    Q_OBJECT
public:
    explicit SnakeGame(QObject *parent = nullptr);
    void startGame();
    void restartGame();
    void changeDirection(int key);
    void update();
    int getScore() const;
    bool isGameOver() const;
    const Snake& getSnake() const;
    const Food& getFood() const;

signals:
    void gameUpdated();
    void gameOver();

private:
    Snake snake;
    Food food;
    int score;
    bool gameOverFlag;
    void checkCollisions();
    void spawnFood();
};

#endif // SNAKEGAME_H