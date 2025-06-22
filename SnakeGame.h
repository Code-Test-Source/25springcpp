#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QObject>
#include "Snake.h"
#include "Food.h"

enum GameState {
    Menu,
    Playing,
    GameOver
};

enum MapType {
    EmptyMap,
    ObstacleMap
};

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
    GameState getGameState() const;
    void setGameState(GameState state);
    int getDifficulty() const;
    void setDifficulty(int difficulty);
    int getElapsedTime() const;
    void setElapsedTime(int time);
    void setHighScore(int score);
    int getHighScore() const;
    void setSelectedMap(MapType mapType);
    const QList<QPoint>& getObstacles() const; // Add this line

signals:
    void stopGameTimer();
    void startGameTimer();

private:
    void loadHighScore();
    void saveHighScore();

signals:
    void gameUpdated();
    void gameOver();

private:
    Snake snake;
    Food food;
    int score;
    bool gameOverFlag;
    GameState gameState;
    int difficulty;
    int elapsedTime;
    int highScore;
    MapType selectedMap;
    QList<QPoint> obstacles; // Add this line
    bool waitingForFirstMove; // Add this line
    void checkCollisions();
    void spawnFood();
};

#endif // SNAKEGAME_H