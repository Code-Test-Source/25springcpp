#ifndef FOOD_H
#define FOOD_H


#include <deque>
#include <QPoint>

class Food {
public:
    Food();
    void respawn(int width, int height, const std::deque<QPoint>& snakeBody, const QList<QPoint>& obstacles);
    QPoint getPosition() const;
private:
    QPoint position;
};

#endif // FOOD_H