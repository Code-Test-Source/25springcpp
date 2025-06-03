#include "Food.h"
#include <cstdlib>
#include <ctime>
#include <QList>

Food::Food() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    position = QPoint(5, 5);
}

void Food::respawn(int width, int height, const std::deque<QPoint>& snakeBody, const QList<QPoint>& obstacles) {
    while (true) {
        int x = std::rand() % width;
        int y = std::rand() % height;
        QPoint newPos(x, y);
        bool overlap = false;
        for (const auto& part : snakeBody) {
            if (part == newPos) {
                overlap = true;
                break;
            }
        }
        if (!overlap) {
            for (const auto& obstacle : obstacles) {
                if (obstacle == newPos) {
                    overlap = true;
                    break;
                }
            }
        }

        if (!overlap) {
            position = newPos;
            break;
        }
    }
}

QPoint Food::getPosition() const {
    return position;
}