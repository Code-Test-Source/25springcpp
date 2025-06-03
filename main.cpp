#include <QApplication>
#include "SnakeGame.h"
#include "GameRenderer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SnakeGame game;
    GameRenderer renderer(&game);
    renderer.show();
    return app.exec();
}