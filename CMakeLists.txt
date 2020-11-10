cmake_minimum_required(VERSION 3.17)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

find_package(OpenCV REQUIRED)
find_program(CLANG_FORMAT_BIN clang-format)
if (NOT CLANG_FORMAT_BIN)
    message(STATUS "clang-format not found.")
else ()
    message(STATUS "clang-format found: ${CLANG_FORMAT_BIN}")
endif ()
include_directories(${OpenCV_INCLUDE_DIRS} lib/)


file(GLOB_RECURSE lib
        ${PROJECT_SOURCE_DIR}/lib/*.hpp
        ${PROJECT_SOURCE_DIR}/lib/*.cpp
        )

file(GLOB_RECURSE src
        ${PROJECT_SOURCE_DIR}/src/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        )

set(MATTERIALIZE_SRC_FILES ${src} ${lib})

add_executable(matterialize ${MATTERIALIZE_SRC_FILES})

target_link_libraries(matterialize ${OpenCV_LIBS})


set(MATTERIALIZE_TEST_FILES ${PROJECT_SOURCE_DIR}/tests/main.cpp
        ${PROJECT_SOURCE_DIR}/tests/matting/background_cut/types/ImageTest.cpp)
add_executable(matterialize_tests ${MATTERIALIZE_TEST_FILES})

target_link_libraries(matterialize_tests gtest ${OpenCV_LIBS})
target_include_directories(matterialize_tests PRIVATE src)


if (CLANG_FORMAT_BIN)
    set(ALL_FILES ${MATTERIALIZE_SRC_FILES} ${MATTERIALIZE_TEST_FILES})
    add_custom_target(clang-format-check-formatting COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror -style=file ${ALL_FILES})
    add_custom_target(clang-format-project-files COMMAND ${CLANG_FORMAT_BIN} -i -style=file ${ALL_FILES})
endif ()