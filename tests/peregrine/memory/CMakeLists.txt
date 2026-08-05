add_library(
        MemoryTests OBJECT
        MemoryTests.cpp
)
target_compile_features(MemoryTests PRIVATE cxx_std_20)

target_include_directories(
        MemoryTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(
        MemoryTests PRIVATE
        PeregrineTestSetup
)


##############################################
##            DEATH TESTS
add_library(
        MemoryDeathTests OBJECT
        death-tests/MemoryTests.cpp
)
target_compile_features(MemoryDeathTests PRIVATE cxx_std_20)

target_include_directories(
        MemoryDeathTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(
        MemoryDeathTests PRIVATE
        PeregrineTestSetup
)