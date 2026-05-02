add_executable(Demo)

set(SourceDir "src/")
set(SourceFiles "Main.cpp")
list(TRANSFORM SourceFiles PREPEND ${SourceDir})

set(IncludeDir "include/")

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