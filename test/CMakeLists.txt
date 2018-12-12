
option(BUILD_UNIT_TESTS
    "Whether or not to build the unit-test requires CUnit"
    ON
    )

if (BUILD_UNIT_TESTS)
    #option(CUnit_DIR CACHE PATH)
    #find_package(CUnit)
    find_library(LIB_CUINIT cunit)
    #Find the headers on windows
    if (MSVC)
        find_path(HEADER_DIR_CUNIT CUnit/CUnit.h)
    endif()
        

    set(UNIT_TEST unit-test)
    set(UNIT_TEST_SOURCES
        unit_test.c
        meta_test.c
        see_object_test.c
        )
    set(UNIT_TEST_HEADERS suites.h)

    add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES} ${UNIT_TEST_HEADERS})

    set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 99)
    set_property(TARGET ${UNIT_TEST} PROPERTY C_STANDARD_REQUIRED ON)

    target_link_libraries(${UNIT_TEST} PRIVATE ${LIB_CUINIT})
    target_link_libraries(${UNIT_TEST} PRIVATE ${SEE_OBJ_LIB})
    
    if (MSVC)
        # Specify the include directory from which the include files are found
        target_include_directories(${UNIT_TEST} PRIVATE ${HEADER_DIR_CUNIT})
        # Set this unit test as debug target
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY
                VS_STARTUP_PROJECT ${UNIT_TEST}
            )
    endif()
    
endif()

