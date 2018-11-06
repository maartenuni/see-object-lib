
cmake_minimum_required(VERSION 3.5)
project (SeeObject C CXX)

set (SEE_OBJECT_VERSION_MAJOR  "0")
set (SEE_OBJECT_VERSION_MINOR  "0")
set (SEE_OBJECT_VERSION_MICRO  "0")
string (CONCAT SEE_OBJECT_VERSION
                "${SEE_OBJECT_VERSION_MAJOR}.${SEE_OBJECT_VERSION_MINOR}"
                )
string (CONCAT SEE_OBJECT_VERSION_STRING
                "SeeObject-${SEE_OBJECT_VERSION_MAJOR}."
                "${SEE_OBJECT_VERSION_MINOR}."
                "${SEE_OBJECT_VERSION_MICRO}"
                )

if (MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_CC_FLAGS} /W4")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set (CMAKE_C_FLAGS "${CMAKE_CC_FLAGS} -W -Wextra -Wall -pedantic")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wextra -Wall -pedantic")
endif()

#find_package(Threads REQUIRED)

include (CheckIncludeFiles)
include (GenerateExportHeader)
include (InstallRequiredSystemLibraries)

check_include_files(stdio.h     HAVE_STDIO_H)
check_include_files(stdlib.h    HAVE_STDLIB_H)
check_include_files(string.h    HAVE_STRING_H)
check_include_files(sys/stat.h  HAVE_SYS_STAT_H)

set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${SEE_OBJECT_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${SEE_OBJECT_VERSION_MINOR}")
set (CPACK_PACKAGE_MAINTAINER    "MD")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER    "MD")
include (CPack)

set (SEE_OBJ_LIB see_object)

add_subdirectory(src)
add_subdirectory(test)

