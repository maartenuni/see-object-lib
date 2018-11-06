
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/see_object_config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/see_object_config.h"
               )
configure_file(
        see_object.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/see_object.pc"
        @ONLY)

set (SEE_OBJ_SRC
    #see_class.c
    SeeObject.c
    atomic_operations.c
    #priv/see_mutex.cpp
    )

set (SEE_OBJ_HDR
    #see_class.h
    SeeObject.h
    atomic_operations.h
    #see_object_class.h

    #priv/see_mutex.h
    )

add_library(${SEE_OBJ_LIB} SHARED ${SEE_OBJ_SRC} ${SEE_OBJ_HDR})

generate_export_header(${SEE_OBJ_LIB})

#enable compiling with C99 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 11)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${SEE_OBJ_LIB} PROPERTIES LINKER_LANGUAGE C)

# make sure the configure file can be found.
target_include_directories(${SEE_OBJ_LIB} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

install (TARGETS ${SEE_OBJ_LIB}
            LIBRARY DESTINATION "lib/${SEE_OBJ_LIB}/"
            )
install (FILES ${SEE_OBJ_HDR} DESTINATION "include/${SEE_OBJ_LIB}")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${SEE_OBJ_LIB}_export.h"
         DESTINATION "include/${SEE_OBJ_LIB}"
        )
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${SEE_OBJ_LIB}.pc"
         DESTINATION "share/pkgconfig"
        )

#target_link_libraries(${SEE_OBJ_LIB} Threads::Threads)
