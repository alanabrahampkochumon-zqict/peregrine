add_executable(Demo)


# Directory
set(SourceDir "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/include")


# Files
set(SourceFiles "Main.cpp")
list(TRANSFORM SourceFiles PREPEND ${SourceDir})


target_sources(
    Demo
    PRIVATE
    ${SourceFiles}
)

target_link_libraries(
    Demo PRIVATE
    MemoryManager
    StrictWarnings
)