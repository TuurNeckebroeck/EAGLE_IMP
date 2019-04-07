cmake_minimum_required(VERSION 3.13)
project(IMP_TUUR)

set(CMAKE_CXX_STANDARD 14)

add_executable(IMP_TUUR main.cpp main.h)

find_package( OpenCV REQUIRED )
target_link_libraries( IMP_TUUR ${OpenCV_LIBS} )