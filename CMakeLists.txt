cmake_minimum_required(VERSION 3.16)
project(matterialize)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

message("CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message("VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")

find_package(OpenCV CONFIG REQUIRED)
find_package(Pistache CONFIG REQUIRED)
set(Pistache_LIBS pistache_static gcov) # Don't really know why but gcov is needed
find_package(RapidJSON CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
set(GTest_LIBS GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

include_directories(lib)

file(GLOB_RECURSE lib
        ${PROJECT_SOURCE_DIR}/lib/*.hpp
        ${PROJECT_SOURCE_DIR}/lib/*.cpp
        )

file(GLOB_RECURSE src
        ${PROJECT_SOURCE_DIR}/src/files/*.hpp
        ${PROJECT_SOURCE_DIR}/src/files/*.cpp
        )

file(GLOB_RECURSE tests
        ${PROJECT_SOURCE_DIR}/tests/*.hpp
        ${PROJECT_SOURCE_DIR}/tests/*.cpp)

set(MATTERIALIZE_SRC_FILES ${src} ${lib})

add_executable(matterialize-server ${MATTERIALIZE_SRC_FILES} ${PROJECT_SOURCE_DIR}/src/mains/matterialize.cpp)
target_link_libraries(matterialize-server PRIVATE ${OpenCV_LIBS})
target_link_libraries(matterialize-server PRIVATE ${Pistache_LIBS} ${RAPIDJSON_INCLUDE_DIRS})


add_executable(benchmatte ${MATTERIALIZE_SRC_FILES} ${PROJECT_SOURCE_DIR}/src/mains/benchmatte.cpp)
target_link_libraries(benchmatte ${OpenCV_LIBS})
add_executable(matterialize_tests ${tests})

target_link_libraries(matterialize_tests ${GTest_LIBS} ${OpenCV_LIBS})
target_include_directories(matterialize_tests PRIVATE src)

file(GLOB_RECURSE mains
        ${PROJECT_SOURCE_DIR}/src/mains/*.hpp
        ${PROJECT_SOURCE_DIR}/src/mains/*.cpp
        )

find_program(CLANG_FORMAT_BIN clang-format)
if (NOT CLANG_FORMAT_BIN)
    message(STATUS "clang-format not found.")
else ()
    message(STATUS "clang-format found: ${CLANG_FORMAT_BIN}")
endif ()

if (CLANG_FORMAT_BIN)
    set(ALL_FILES ${src} ${tests} ${mains})
    add_custom_target(clang-format-check-formatting COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror -style=file ${ALL_FILES})
    add_custom_target(clang-format-project-files COMMAND ${CLANG_FORMAT_BIN} -i -style=file ${ALL_FILES})
endif ()


