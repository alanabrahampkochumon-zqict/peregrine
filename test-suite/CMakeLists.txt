add_executable(TestSuite)

set(TestDir "src/")
set(CommonFiles "ConstantTests.cpp")
list(TRANSFORM CommonFiles PREPEND ${TestDir})

set(AllocatorDir "src/allocators/")
set(AllocatorFiles "ArenaTests.cpp;")
list(TRANSFORM AllocatorFiles PREPEND ${AllocatorDir})

set(IncludeDir "include/")
set(IncludeFiles "Utils.h;DoxygenGroups.h")
list(TRANSFORM IncludeFiles PREPEND ${IncludeDir})

target_sources(
    TestSuite
    PRIVATE
    ${CommonFiles} ${AllocatorFiles}

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

include(GoogleTest)
gtest_discover_tests(TestSuite)