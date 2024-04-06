cmake_minimum_required(VERSION 3.13)
project(redis-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

file(GLOB SOURCE_FILES "src/*.cpp")
add_executable(server ${SOURCE_FILES})