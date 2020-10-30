cmake_minimum_required(VERSION 3.17)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(matterialize main.cpp)

target_link_libraries(matterialize ${OpenCV_LIBS})