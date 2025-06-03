QT += widgets multimedia
CONFIG += c++20
TARGET = SnakeGameQt
TEMPLATE = app
SOURCES += main.cpp \
    Snake.cpp \
    SnakeGame.cpp \
    Food.cpp \
    GameRenderer.cpp
HEADERS += Snake.h \
    SnakeGame.h \
    Food.h \
    GameRenderer.h