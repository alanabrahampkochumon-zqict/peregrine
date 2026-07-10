add_executable(Demo)

target_sources(
    Demo
    PRIVATE
        Main.cpp
)

target_link_libraries(
    Demo PRIVATE
    MemoryManager
    StrictWarnings
)