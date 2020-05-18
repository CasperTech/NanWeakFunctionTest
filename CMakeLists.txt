cmake_minimum_required(VERSION 3.12)
project(NanWeakFunctionTest)

set(CMAKE_CXX_STANDARD 17)

include(NodeJS.cmake)
nodejs_init()

add_nodejs_module(${PROJECT_NAME} main.cpp test.cpp testObject.cpp testObject.h)

