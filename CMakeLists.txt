cmake_minimum_required(VERSION 3.14)
project(SnakeGameQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt libraries
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Add source files (to be created)
set(SOURCES
    main.cpp
    SnakeGame.h
    SnakeGame.cpp
    Snake.h
    Snake.cpp
    Food.h
    Food.cpp
    GameRenderer.h
    GameRenderer.cpp
)

add_executable(SnakeGameQt ${SOURCES})

target_link_libraries(SnakeGameQt PRIVATE Qt6::Widgets)