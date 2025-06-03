#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <QWidget>
#include <QTimer>
#include "SnakeGame.h"

enum SnakeHeadShape {
    SquareHead,
    TriangleHead,
    CircleHead,
    HexagonHead
};

enum SnakeBodyColor {
    GreenBody,
    BlueBody,
    YellowBody
};

class GameRenderer : public QWidget {
    Q_OBJECT
public:
    explicit GameRenderer(SnakeGame* game, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    void renderMenu(QPainter& painter);
    void renderPlaying(QPainter& painter);
    void renderGameOver(QPainter& painter);
    void handleMenuKeyPress(int key);
    void handlePlayingKeyPress(int key);
    void handleGameOverKeyPress(int key);

    SnakeGame* game;
    QTimer* gameTimer; // Declare gameTimer as a member variable
    QColor snakeHeadColor;
    QColor snakeBodyColor;

    SnakeHeadShape selectedHeadShape = SquareHead; // Default head shape
    SnakeBodyColor selectedBodyColor = GreenBody; // Default body color

    void stopGameTimer();
    void startGameTimer();
};

#endif // GAMERENDERER_H