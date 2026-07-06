option(ENABLE_PMM_TEST "Enable PMM Unit Tests")

if (ENABLE_PMM_TEST)
    message("Test suite is enabled")
    add_subdirectory(pmm)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestSuite)
endif ()