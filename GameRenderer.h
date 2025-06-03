#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <QWidget>
#include "SnakeGame.h"

class GameRenderer : public QWidget {
    Q_OBJECT
public:
    explicit GameRenderer(SnakeGame* game, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    SnakeGame* game;
};

#endif // GAMERENDERER_H