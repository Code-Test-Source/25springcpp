#ifndef SNAKE_H
#define SNAKE_H

#include <QPoint>
#include <deque>

class Snake {
public:
    enum Direction { Up, Down, Left, Right };
    Snake();
    void reset();
    void setDirection(Direction dir);
    Direction getDirection() const;
    void move();
    void grow();
    bool checkSelfCollision() const;
    const std::deque<QPoint>& getBody() const;
    QPoint getHead() const;

private:
    std::deque<QPoint> body;
    Direction direction;
    bool growFlag;
};

#endif // SNAKE_H