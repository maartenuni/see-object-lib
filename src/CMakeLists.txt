
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
    see_init.c
    Error.c
    MetaClass.c
    SeeObject.c
    atomic_operations.c
    #priv/see_mutex.cpp
    )

set (SEE_OBJ_HDR_PUBLIC
    errors.h
    Error.h
    see_init.h
    MetaClass.h
    SeeObject.h
    atomic_operations.h
    ${CMAKE_CURRENT_BINARY_DIR}/see_export.h
    )

set (SEE_OBJ_HDR_PRIVATE
    #priv/see_mutex.h
    )

set (SEE_OBJ_HDR ${SEE_OBJ_HDR_PUBLIC} ${SEE_OBJ_HDR_PRIVATE})

add_library(${SEE_OBJ_LIB}
        SHARED ${SEE_OBJ_SRC}
        ${SEE_OBJ_HDR_PUBLIC}
        ${SEE_OBJ_HDR_PRIVATE}
        )
add_library(${SEE_OJB_LIB}::${SEE_OBJ_LIB} ALIAS ${SEE_OBJ_LIB})


generate_export_header(${SEE_OBJ_LIB} BASE_NAME SEE)

#enable compiling with C99 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 99)
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
        PRIVATE -W -Wall -Wextra -pedantic
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
