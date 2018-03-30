
set (SEE_OBJ_LIB see_object)

set (SEE_OBJ_SRC
    see_class.c
    see_object.c
    #priv/see_mutex.cpp
    )

set (SEE_OBJ_HDR
    see_class.h
    see_object.h
    #see_object_class.h
    #priv/see_mutex.h
    )

add_library(see_object SHARED ${SEE_OBJ_SRC} ${SEE_OBJ_HDR})

#enable compiling with C99 standard
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 99)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${SEE_OBJ_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${SEE_OBJ_LIB} PROPERTIES LINKER_LANGUAGE C)

