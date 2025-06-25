#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QObject>
#include "Snake.h"
#include "Food.h"

// 游戏状态枚举
enum GameState {
    Menu,       // 菜单界面
    Playing,    // 游戏进行中
    GameOver    // 游戏结束
};

// 地图类型枚举
enum MapType {
    EmptyMap,       // 无障碍地图
    ObstacleMap     // 有障碍物地图
};

// SnakeGame 类：游戏的主控制器，负责管理游戏状态、计时、分数、地图、蛇和食物等
class SnakeGame : public QObject {
    Q_OBJECT
public:
// 游戏状态枚举
enum GameState {
    Menu,       // 菜单界面
    Playing,    // 游戏进行中
    GameOver    // 游戏结束
};


// 地图类型枚举
enum MapType {
    EmptyMap,       // 无障碍地图
    ObstacleMap     // 有障碍物地图
};
    // 构造函数
    explicit SnakeGame(QObject *parent = nullptr);

    // 设置蛇的移动方向
    void setSnakeDirection(Snake::Direction dir);

    // 加载地图
    void loadMap(int mapIndex);

    // 开始游戏
    void startGame();

    // 重新开始游戏
    void restartGame();

    // 根据按键改变方向
    void changeDirection(int key);

    // 每帧更新游戏状态（定时器触发）
    void update();

    // 获取当前得分
    int getScore() const;

    // 判断游戏是否结束
    bool isGameOver() const;

    // 获取蛇对象的常引用
    const Snake& getSnake() const { return snake; }

    // 获取食物对象的常引用
    const Food& getFood() const;

    // 获取当前游戏状态
    GameState getGameState() const;

    // 设置游戏状态
    void setGameState(GameState state);

    // 获取当前难度（用于调整速度等）
    int getDifficulty() const;

    // 设置游戏难度
    void setDifficulty(int difficulty);

    // 获取已用时间（秒）
    int getElapsedTime() const;

    // 设置已用时间
    void setElapsedTime(int time);

    // 设置最高分
    void setHighScore(int score);

    // 获取最高分
    int getHighScore() const;

    // 设置当前选中的地图类型
    void setSelectedMap(MapType mapType);

    // 获取障碍物位置列表
    const QList<QPoint>& getObstacles() const;

signals:
    // 用于控制计时器：停止
    void stopGameTimer();

    // 用于控制计时器：开始
    void startGameTimer();

    // 每次更新界面时触发（例如定时器帧更新）
    void gameUpdated();

    // 游戏结束时触发
    void gameOver();

private:
    // 加载最高分（从文件或配置）
    void loadHighScore();

    // 保存最高分（到文件或配置）
    void saveHighScore();

    // 检查碰撞（墙、自身、障碍）
    void checkCollisions();

    // 生成新食物
    void spawnFood();

    // 成员变量
    Snake snake;               // 蛇对象
    Food food;                 // 食物对象
    int score;                 // 当前得分
    bool gameOverFlag;         // 游戏结束标志
    GameState gameState;       // 当前游戏状态
    int difficulty;            // 游戏难度等级
    int elapsedTime;           // 当前游戏用时（秒）
    int highScore;             // 历史最高分
    MapType selectedMap;       // 当前选中的地图类型
    QList<QPoint> obstacles;   // 障碍物位置列表
    bool waitingForFirstMove; // 等待玩家首次操作的标志（避免游戏一开始自动移动）
};

#endif // SNAKEGAME_H
