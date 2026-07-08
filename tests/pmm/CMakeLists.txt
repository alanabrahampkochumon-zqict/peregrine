add_executable(TestSuite)

set(ROOT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Directories
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Subdirectories
add_subdirectory(${RootDir}/allocators)
add_subdirectory(${RootDir}/telemetry)

# Files
set(CommonFiles "ConstantTests.cpp")
list(TRANSFORM CommonFiles PREPEND ${RootDir})

set(IncludeFiles "Utils.h;DoxygenGroups.h")
list(TRANSFORM IncludeFiles PREPEND ${IncludeDir})


target_sources(
        TestSuite
        PRIVATE
        ${CommonFiles}

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

        # Subdirectory Objects
        AllocatorTests
        TelemetryTests
)

# Preprocessor Macros
# ENABLE_PMM_TELEMETRY -> Enable telemetry testing
target_compile_definitions(TestSuite PRIVATE ENABLE_PMM_TELEMETRY)
target_compile_definitions(AllocatorTests PRIVATE ENABLE_PMM_TELEMETRY)
target_compile_definitions(TelemetryTests PRIVATE ENABLE_PMM_TELEMETRY)

include(GoogleTest)
gtest_discover_tests(TestSuite)