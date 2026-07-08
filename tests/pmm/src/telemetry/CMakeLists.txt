add_library(
        TelemetryTests OBJECT
)

target_sources(
        TelemetryTests PRIVATE
        ArenaTelemetryTests.cpp
)

target_include_directories(
        TelemetryTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(
        TelemetryTests PRIVATE
        GTest::gtest_main
        StrictWarnings
        MemoryManager
)


# Enables GTest's Friend tests
# Unless the consuming application has GTest imported, DON'T enable this!!
target_compile_definitions(AllocatorTests PRIVATE ENABLE_PMM_TESTS)