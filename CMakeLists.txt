cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(dataplatform)
file(GLOB SRC_FILES src/*.cpp)
add_executable(dataplatform main.cpp ${SRC_FILES})
target_link_libraries(dataplatform PRIVATE Ws2_32)