
cmake_minimum_required(VERSION 2.8)
project (SeeObjects C CXX)

if (MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_CC_FLAGS} /W4")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set (CMAKE_C_FLAGS "${CMAKE_CC_FLAGS} -W -Wextra -Wall -pedantic")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wextra -Wall -pedantic")
endif()

include (CheckIncludeFiles)
include (GenerateExportHeader)

add_subdirectory(src)
