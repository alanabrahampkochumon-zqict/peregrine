# Files
set(AllocatorDir "include/allocators/")
set(AllocatorHeaderFiles "Arena.h")
set(AllocatorTemplateFiles "Arena.tpp")
list(TRANSFORM AllocatorHeaderFiles PREPEND ${AllocatorDir})
list(TRANSFORM AllocatorTemplateFiles PREPEND ${AllocatorDir})

set(UtilDir "include/utils/")
set(UtilFiles "Constants.h")
list(TRANSFORM UtilFiles PREPEND ${UtilDir})

set(IncludeDir "include/")
set(IncludeFiles "DoxygenGroups.h")
list(TRANSFORM IncludeFiles PREPEND ${IncludeDir})

add_library(MemoryManager INTERFACE ${AllocatorHeaderFiles} ${AllocatorTemplateFiles} ${UtilFiles})

target_sources(
    MemoryManager
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES 
    ${AllocatorHeaderFiles}
    ${AllocatorTemplateFiles}
    ${UtilFiles}
    ${IncludeFiles}
)

target_include_directories(
    MemoryManager
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(MemoryManager INTERFACE StrictWarnings)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES 
    ${AllocatorHeaderFiles}
    ${UtilFiles}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Template Files" FILES 
    ${AllocatorTemplateFiles} 
)