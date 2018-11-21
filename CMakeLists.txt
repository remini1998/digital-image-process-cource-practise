cmake_minimum_required(VERSION 2.8)
project(digital)

set(CMAKE_CXX_STANDARD 17)

add_executable(digtal main.cpp utils.h utils.cpp EasyBMP/EasyBMP.h EasyBMP/EasyBMP.cpp)