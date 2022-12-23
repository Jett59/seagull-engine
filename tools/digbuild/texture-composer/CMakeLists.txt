cmake_minimum_required(VERSION 3.20)

project(texture-composer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(texture-composer composer.cpp)
target_link_libraries(texture-composer ${CONAN_LIBS})
