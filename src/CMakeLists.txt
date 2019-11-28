
set (HEADER_INSTALL_DIR
    "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${SEE_OBJECT_VERSION_MAJOR}"
    )
set (HEADER_INSTALL_DIR "${HEADER_INSTALL_DIR}.${SEE_OBJECT_VERSION_MINOR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/see_object_config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/see_object_config.h"
               )

# replace with the "configure_package_config_file()" function
configure_file(
        see_object.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/see_object.pc"
        @ONLY)

set (SEE_OBJ_SRC
    see_functions.c
    see_init.c
    BST.c
    Clock.cpp
    CopyError.c
    DynamicArray.c
    Duration.cpp
    Error.c
    IncomparableError.c
    IndexError.c
    MsgBuffer.c
    MetaClass.c
    OverflowError.c
    RuntimeError.c
    SeeObject.c
    Serial.c
    Stack.c
    TimeoutError.c
    TimePoint.cpp
    atomic_operations.c
    utilities.c
    utilities.cpp
    )

set (SEE_OBJ_HDR_PUBLIC
    errors.h
    see_functions.h
    BST.h
    Clock.h
    CopyError.h
    DynamicArray.h
    Duration.h
    Error.h
    IncomparableError.h
    IndexError.h
    see_init.h
    MsgBuffer.h
    MetaClass.h
    OverflowError.h
    RuntimeError.h
    SeeObject.h
    Serial.h
    Stack.h
    TimeoutError.h
    TimePoint.h
    atomic_operations.h
    utilities.h
    ${CMAKE_CURRENT_BINARY_DIR}/see_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/see_object_config.h
    )

set (SEE_OBJ_HDR_PRIVATE
    cpp/Clock.hpp
    cpp/Duration.hpp
    cpp/TimePoint.hpp
    posix/PosixSerial.h
    windows/WindowsSerial.h
    )

set(SEE_OBJ_SRC_POSIX
    posix/PosixSerial.c
    )

set (SEE_OBJ_SRC_WINDOWS
    windows/WindowsRuntimeError.c
    windows/WindowsSerial.c
    )

set (SEE_OBJ_HDR_PUBLIC_WINDOWS
    windows/WindowsRuntimeError.h
    )

if(UNIX)
    list (APPEND SEE_OBJ_SRC ${SEE_OBJ_SRC_POSIX})
endif()

if (WIN32)
    list(APPEND SEE_OBJ_SRC ${SEE_OBJ_SRC_WINDOWS})
    list(APPEND SEE_OBJ_HDR_PUBLIC ${SEE_OBJ_HDR_PUBLIC_WINDOWS})
endif()

set (SEE_OBJ_HDR ${SEE_OBJ_HDR_PUBLIC} ${SEE_OBJ_HDR_PRIVATE})

add_library(${SEE_OBJ_LIB}
        SHARED ${SEE_OBJ_SRC}
        ${SEE_OBJ_HDR_PUBLIC}
        ${SEE_OBJ_HDR_PRIVATE}
        )

add_library(${SEE_OJB_LIB}::${SEE_OBJ_LIB} ALIAS ${SEE_OBJ_LIB})


generate_export_header(${SEE_OBJ_LIB} BASE_NAME SEE)

#enable compiling with C11 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 11)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${SEE_OBJ_LIB} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${SEE_OBJ_LIB}
    PROPERTIES
        VERSION     ${PROJECT_VERSION}
        SOVERSION   ${PROJECT_VERSION}
    )
set_target_properties(${SEE_OBJ_LIB}
    PROPERTIES 
        PUBLIC_HEADER "${SEE_OBJ_HDR_PUBLIC}"
        )

# make sure the config header file can be found.
target_include_directories(${SEE_OBJ_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

if (MSVC)
    target_compile_options(${SEE_OBJ_LIB} PRIVATE "/EHsc")
# Avoid warnings like: warning C4996: 'sprintf': This function or variable may be unsafe
    target_compile_definitions(${SEE_OBJ_LIB}
        PRIVATE _CRT_SECURE_NO_WARNINGS
        )

# Avoid warnings like:
# C4996: 'strdup': The POSIX name for this item is deprecated.
#
# Another way to silence the warning about strdup and friends
# could be something like:
# #define strdup _strdup
# But I don't like to alias it because that might affect other people too.
    target_compile_definitions(${SEE_OBJ_LIB}
        PRIVATE _CRT_NONSTDC_NO_DEPRECATE
        )
endif()

write_basic_package_version_file(
        "${project_name}ConfigVersion.cmake"
    VERSION
        ${SEE_OBJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
    )

# Add compiler warnings
if (NOT MSVC)
    target_compile_options(${SEE_OBJ_LIB}
        PRIVATE -W -Wall -Wextra -pedantic -Werror
        )
else()
    target_compile_options(${SEE_OBJ_LIB}
        PRIVATE "/W4"
        )
endif()

install (TARGETS ${SEE_OBJ_LIB} EXPORT "${SEE_OBJ_LIB}Targets"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${HEADER_INSTALL_DIR}
    PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_DIR}
    )

install (EXPORT "${SEE_OBJ_LIB}Targets"
    FILE "${SEE_OBJ_LIB}Targets.cmake"
    NAMESPACE "${SEE_OBJ_LIB}::"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SEE_OBJ_LIB}
    )

install(
    FILES
        "${SEE_OBJ_LIB}-config.cmake"
    DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/${SEE_OBJ_LIB}"
        )

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${SEE_OBJ_LIB}.pc"
         DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig"
        )

# This makes sure that the dll's are installed next to the binaries
# So we can run the unit tests.
set_target_properties(
        ${SEE_OBJ_LIB}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
