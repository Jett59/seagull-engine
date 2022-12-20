cmake_minimum_required(VERSION 3.20)

project(seagull)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(seagull src/seagull.cpp src/shaders.cpp src/gameObject.cpp src/renderer.cpp)
target_link_libraries(seagull PRIVATE ${CONAN_LIBS})
target_include_directories(seagull PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(seagull PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

add_subdirectory(example-projects)
