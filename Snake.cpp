#include "Snake.h"

Snake::Snake() {
    reset();
}

void Snake::reset() {
    body.clear();
    body.push_back(QPoint(10, 10));
    direction = Right;
    growFlag = false;
}

void Snake::setDirection(Direction dir) {
    // Prevent reversing direction
    if ((direction == Up && dir == Down) ||
        (direction == Down && dir == Up) ||
        (direction == Left && dir == Right) ||
        (direction == Right && dir == Left)) {
        return;
    }
    direction = dir;
}

Snake::Direction Snake::getDirection() const {
    return direction;
}

void Snake::move() {
    QPoint head = getHead();
    switch (direction) {
        case Up:    head.ry() -= 1; break;
        case Down:  head.ry() += 1; break;
        case Left:  head.rx() -= 1; break;
        case Right: head.rx() += 1; break;
    }
    body.push_front(head);
    if (growFlag) {
        growFlag = false;
    } else {
        body.pop_back();
    }
}

void Snake::grow() {
    growFlag = true;
}

bool Snake::checkSelfCollision() const {
    const QPoint& head = getHead();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) {
            return true;
        }
    }
    return false;
}

const std::deque<QPoint>& Snake::getBody() const {
    return body;
}

QPoint Snake::getHead() const {
    return body.front();
}