
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

#enable compiling with C99 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 11)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${SEE_OBJ_LIB} PROPERTIES LINKER_LANGUAGE C)

#target_link_libraries(${SEE_OBJ_LIB} Threads::Threads)
