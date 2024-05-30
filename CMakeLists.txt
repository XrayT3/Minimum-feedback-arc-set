cmake_minimum_required(VERSION 3.26)
project(SP_CC_T)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wall -Wextra -pedantic")

add_executable(SP_CC_T main.cpp)
