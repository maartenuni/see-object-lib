
cmake_minimum_required(VERSION 3.5)

set (SEE_OBJECT_VERSION_MAJOR  "0") # allow breaks in API/ABI
set (SEE_OBJECT_VERSION_MINOR  "0") # add new features
set (SEE_OBJECT_VERSION_MICRO  "0") # patch

string (CONCAT SEE_OBJECT_VERSION
        "${SEE_OBJECT_VERSION_MAJOR}.${SEE_OBJECT_VERSION_MINOR}"
        )

string (CONCAT SEE_OBJECT_VERSION_STRING
        "SeeObject-${SEE_OBJECT_VERSION_MAJOR}."
        "${SEE_OBJECT_VERSION_MINOR}."
        "${SEE_OBJECT_VERSION_MICRO}"
        )

project (SeeObject
        VERSION     ${SEE_OBJECT_VERSION_MAJOR}.${SEE_OBJECT_VERSION_MINOR}
        LANGUAGES   C CXX
        )

include (CheckIncludeFiles)
include (CheckFunctionExists)
include (GenerateExportHeader)
include (InstallRequiredSystemLibraries)
include (GNUInstallDirs)
include (CMakePackageConfigHelpers)

check_include_files(stdio.h     HAVE_STDIO_H)
check_include_files(stdlib.h    HAVE_STDLIB_H)
check_include_files(string.h    HAVE_STRING_H)
check_include_files(sys/stat.h  HAVE_SYS_STAT_H)

check_function_exists(strerror_s HAVE_STRERROR_S)
check_function_exists(strerror_r HAVE_STRERROR_R)

set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${SEE_OBJECT_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${SEE_OBJECT_VERSION_MINOR}")
set (CPACK_PACKAGE_MAINTAINER    "MD")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER    "MD")

include (CPack)

set (SEE_OBJ_LIB see_object)

add_subdirectory(src)
add_subdirectory(test)


