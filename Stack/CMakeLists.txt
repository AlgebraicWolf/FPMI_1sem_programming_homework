cmake_minimum_required(VERSION 3.14)
project(Stack)

set(CMAKE_CXX_STANDARD 14)

add_executable(Stack main.cpp MurMurHash3.cpp MurMurHash3.h)
add_library(StackLibrary stack.cpp stack.h)
add_library(MurMurHash3 MurMurHash3.cpp MurMurHash3.h)

target_link_libraries(Stack StackLibrary MurMurHash3)