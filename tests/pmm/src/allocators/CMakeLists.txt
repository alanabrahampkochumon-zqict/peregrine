add_library(AllocatorTests OBJECT)

target_sources(
        AllocatorTests
        PRIVATE
        ArenaTests.cpp
        StackTests.cpp
        TempArenaTests.cpp
)

target_include_directories(
        AllocatorTests PRIVATE
        ${ROOT_TEST_DIR}/include
)

target_link_libraries(
        AllocatorTests PRIVATE
        GTest::gtest_main
        StrictWarnings
        MemoryManager
)

# Enables GTest's Friend tests
# Unless the consuming application has GTest imported, DON'T enable this!!
target_compile_definitions(AllocatorTests PRIVATE ENABLE_PMM_TESTS)