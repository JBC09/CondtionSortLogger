cmake_minimum_required(VERSION 3.31)
project(LogFind)

set(CMAKE_CXX_STANDARD 20)

add_executable(LogFind main.cpp
        src/Service.cpp
        include/Service.h
        src/ConditionLSortLogger.cpp
        include/ConditionLSortLogger.h)
