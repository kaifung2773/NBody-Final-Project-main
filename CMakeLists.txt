cmake_minimum_required(VERSION 3.12)
project(NBody LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(SFML_ROOT "$ENV{HOME}/SFML")
include_directories(
    ${SFML_ROOT}/include
    ${CMAKE_SOURCE_DIR}/include   
    ${CMAKE_SOURCE_DIR}/src 
)
link_directories(${SFML_ROOT}/lib64)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB PROJECT_SRCS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(NBody
  main.cpp
  ${PROJECT_SRCS}
)

target_link_libraries(NBody
    OpenCL
    sfml-graphics
    sfml-window
    sfml-system
)

set_target_properties(NBody PROPERTIES
    BUILD_RPATH ${SFML_ROOT}/lib64
)