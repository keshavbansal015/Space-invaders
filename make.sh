#!/bin/bash

g++ -std=c++11 main.cpp -o main -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lglew -framework OpenGL