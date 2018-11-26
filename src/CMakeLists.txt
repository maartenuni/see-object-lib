
set (HEADER_INSTALL_DIR
    "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}-${SEE_OBJECT_VERSION_MAJOR}"
    )
set (HEADER_INSTALL_DIR "${HEADER_INSTALL_DIR}.${SEE_OBJECT_VERSION_MINOR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/see_object_config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/see_object_config.h"
               )
configure_file(
        see_object.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/see_object.pc"
        @ONLY)

set (SEE_OBJ_SRC
    see_functions.c
    Array.c
    see_init.c
    MetaClass.c
    SeeObject.c
    atomic_operations.c
    #priv/see_mutex.cpp
    )

set (SEE_OBJ_HDR_PUBLIC
    errors.h
    see_functions.h
    Array.h
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


generate_export_header(${SEE_OBJ_LIB} BASE_NAME SEE)

#enable compiling with C99 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 99)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${SEE_OBJ_LIB} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${SEE_OBJ_LIB} PROPERTIES
        VERSION     ${PROJECT_VERSION}
        SOVERSION   ${PROJECT_VERSION}
        )
set_target_properties(${SEE_OBJ_LIB} PROPERTIES 
        PUBLIC_HEADER "${SEE_OBJ_HDR_PUBLIC}"
        )

# make sure the config header file can be found.
target_include_directories(${SEE_OBJ_LIB} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


install (TARGETS ${SEE_OBJ_LIB}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_DIR}
            )

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${SEE_OBJ_LIB}.pc"
         DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig"
        )

#target_link_libraries(${SEE_OBJ_LIB} Threads::Threads)
