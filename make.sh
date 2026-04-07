#!/bin/bash

g++ -std=c++11 main.cpp -o game -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lglew -framework OpenGL

# g++ src/main.cpp src/renderer.cpp src/game.cpp src/sprites.cpp \
# -I./include -lGL -lGLEW -lglfw -o space_invaders

# g++ src/main.cpp src/renderer.cpp src/game.cpp src/sprites.cpp \
#     -I./include \
#     -I/opt/homebrew/include \
#     -L/opt/homebrew/lib \
#     -lGL -lGLEW -lglfw \
#     -o game

# g++ src/main.cpp src/renderer.cpp src/game.cpp src/sprites.cpp \
#     -I./include \
#     -I/opt/homebrew/include \
#     -L/opt/homebrew/lib \
#     -lGLEW -lglfw -framework OpenGL \
#     -o game

