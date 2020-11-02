cmake_minimum_required(VERSION 3.17)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} lib/)

file(GLOB_RECURSE lib_src
        ${PROJECT_SOURCE_DIR}/lib/*.h
        ${PROJECT_SOURCE_DIR}/lib/*.cpp
        )

add_executable(matterialize src/main.cpp src/matting/matter.cpp src/matting/matter.hpp src/matting/opencv_matter.cpp src/matting/opencv_matter.hpp src/matting/background_negation_matter.cpp src/matting/background_negation_matter.hpp src/camera/opencv_webcam.cpp src/camera/opencv_webcam.hpp src/matting/face_detection_matter.cpp src/matting/face_detection_matter.hpp src/matting/background_cut/background_cut_matter.cpp src/matting/background_cut/background_cut_matter.hpp src/matting/background_cut/models/pixel_bg_model.cpp src/matting/background_cut/models/pixel_bg_model.hpp src/matting/background_cut/types/flat_image.cpp src/matting/background_cut/types/flat_image.hpp src/matting/background_cut/types/flat_mask.cpp src/matting/background_cut/types/flat_mask.hpp src/matting/background_cut/types/pixel_variances.cpp src/matting/background_cut/types/pixel_variances.hpp src/matting/background_cut/types/probability.cpp src/matting/background_cut/types/probability.hpp src/matting/background_cut/types/segmentation_result.cpp src/matting/background_cut/types/segmentation_result.hpp src/matting/background_cut/types/image.cpp src/matting/background_cut/types/image.hpp src/util/converter.hpp src/util/converter.cpp ${lib_src})

target_link_libraries(matterialize ${OpenCV_LIBS})