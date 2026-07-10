option(ENABLE_PMM_TEST "Enable PMM Unit Tests")

set(ROOT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(ROOT_INCLUDE_FILES include/Utils.h)

if (ENABLE_PMM_TEST)
    message("Test suite is enabled")
    add_subdirectory(peregrine)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestSuite)
endif ()