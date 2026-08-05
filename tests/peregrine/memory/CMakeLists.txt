add_library(
        MemoryTests OBJECT
        WinMemoryTests.cpp
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