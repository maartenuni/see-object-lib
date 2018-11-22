
option(BUILD_UNIT_TESTS
    "Whether or not to build the unit-test requires CUnit"
    ON
    )

if (BUILD_UNIT_TESTS)
    find_library(LIB_CUINIT cunit)

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

    target_link_libraries(${UNIT_TEST} ${LIB_CUINIT})
    target_link_libraries(${UNIT_TEST} ${SEE_OBJ_LIB})
endif()

