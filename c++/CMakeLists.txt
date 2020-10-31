cmake_minimum_required(VERSION 3.17)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(matterialize main.cpp src/matting/matter.cpp src/matting/matter.hpp src/matting/opencv_matter.cpp src/matting/opencv_matter.hpp src/matting/background_negation_matter.cpp src/matting/background_negation_matter.hpp)

target_link_libraries(matterialize ${OpenCV_LIBS})