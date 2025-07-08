cmake_minimum_required(VERSION 3.10)
project(chess_engine)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

file(GLOB SRC "src/*.cpp")
add_executable(chess_engine ${SRC})

