cmake_minimum_required(VERSION 2.8)

PROJECT(UtilitiesLib)

FILE(GLOB allFiles *.cpp *.h)
list (REMOVE_ITEM allFiles "test.cpp")

add_library(utiliyLib SHARED ${allFiles})
add_executable(testExec test.cpp)
target_link_libraries(testExec utiliyLib)