cmake_minimum_required(VERSION 3.17)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} lib/)

file(GLOB_RECURSE lib_src
        ${PROJECT_SOURCE_DIR}/lib/*.h
        ${PROJECT_SOURCE_DIR}/lib/*.cpp
        )

add_executable(matterialize
        src/main.cpp
        src/matting/matter.cpp src/matting/matter.hpp
        src/matting/opencv_matter.cpp src/matting/opencv_matter.hpp
        src/matting/background_negation_matter.cpp src/matting/background_negation_matter.hpp
        src/camera/opencv_webcam.cpp src/camera/opencv_webcam.hpp
        src/matting/background_cut/background_cut_matter.cpp src/matting/background_cut/background_cut_matter.hpp
        src/matting/background_cut/models/pixel_bg_model.cpp src/matting/background_cut/models/pixel_bg_model.hpp
        src/matting/background_cut/types/mask.cpp src/matting/background_cut/types/mask.hpp
        src/matting/background_cut/types/pixel_variances.cpp src/matting/background_cut/types/pixel_variances.hpp
        src/matting/background_cut/types/probability.cpp src/matting/background_cut/types/probability.hpp
        src/matting/background_cut/types/segmentation_result.cpp src/matting/background_cut/types/segmentation_result.hpp
        src/matting/background_cut/types/image.cpp src/matting/background_cut/types/image.hpp
        src/matting/background_cut/models/global_bg_model.cpp src/matting/background_cut/models/global_bg_model.hpp
        src/matting/background_cut/models/gmm_global_color_model.cpp src/matting/background_cut/models/gmm_global_color_model.hpp
        src/matting/background_cut/models/global_fg_model.cpp src/matting/background_cut/models/global_fg_model.hpp
        src/matting/background_cut/models/color_model.cpp src/matting/background_cut/models/color_model.hpp
        src/util/converter.hpp src/util/converter.cpp
        ${lib_src}
        src/camera/fake_webcam.hpp src/camera/fake_webcam.cpp
        src/camera/opencv_webcam_controls.cpp src/camera/opencv_webcam_controls.hpp
        src/matting/background_cut/models/contrast_model.cpp src/matting/background_cut/models/contrast_model.hpp src/matting/background_cut/types/contrast_term.hpp src/matting/background_cut/types/contrast_term.cpp src/matting/background_cut/types/color_term.cpp)

target_link_libraries(matterialize ${OpenCV_LIBS})

add_executable(matterialize_tests
        tests/main.cpp
        tests/matting/background_cut/types/ImageTest.cpp)

target_link_libraries(matterialize_tests gtest ${OpenCV_LIBS})
target_include_directories(matterialize_tests PRIVATE src)

