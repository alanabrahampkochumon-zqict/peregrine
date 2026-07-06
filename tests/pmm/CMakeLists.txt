add_executable(TestSuite)

# Directories
set(TestDir "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(AllocatorDir "${TestDir}allocators/")
set(TelemetryDir "${TestDir}telemetry/")


# Files
set(CommonFiles "ConstantTests.cpp")
list(TRANSFORM CommonFiles PREPEND ${TestDir})

set(AllocatorFiles "ArenaTests.cpp;TempArenaTests.cpp;StackTests.cpp")
list(TRANSFORM AllocatorFiles PREPEND ${AllocatorDir})

set(TelemetryFiles "ArenaTelemetryTests.cpp;")
list(TRANSFORM TelemetryFiles PREPEND ${TelemetryDir})

set(IncludeFiles "Utils.h;DoxygenGroups.h")
list(TRANSFORM IncludeFiles PREPEND ${IncludeDir})


target_sources(
        TestSuite
        PRIVATE
        ${CommonFiles} ${AllocatorFiles} ${TelemetryFiles}

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS ${IncludeDir}
        FILES ${IncludeFiles}
)

target_include_directories(
        TestSuite PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        TestSuite PRIVATE
        GTest::gtest_main
        StrictWarnings
        MemoryManager
)

# Preprocessor Macros
# ENABLE_PMM_TELEMETRY -> Enable telemetry testing
target_compile_definitions(TestSuite PRIVATE ENABLE_PMM_TELEMETRY)
# Enables GTest's Friend tests
# Unless the consuming application has GTest imported, DON'T enable this!!
target_compile_definitions(TestSuite PRIVATE ENABLE_PMM_TESTS)

include(GoogleTest)
gtest_discover_tests(TestSuite)