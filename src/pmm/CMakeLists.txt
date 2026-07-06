# Directories
set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(AllocatorDir "${IncludeDir}peregrine/allocators/")
set(TelemetryDir "${IncludeDir}peregrine/telemetry/")
set(UtilDir "${IncludeDir}peregrine/utils/")


# Files
set(AllocatorHeaderFiles "Arena.h;TempArena.h;Stack.h")
list(TRANSFORM AllocatorHeaderFiles PREPEND ${AllocatorDir})

set(AllocatorTemplateFiles "Arena.tpp;TempArena.tpp;Stack.tpp")
list(TRANSFORM AllocatorTemplateFiles PREPEND ${AllocatorDir})

# Telemetry Files
set(TelemetryHeaderFiles "ArenaTelemetry.h;")
list(TRANSFORM TelemetryHeaderFiles PREPEND ${TelemetryDir})

set(TelemetryTemplateFiles "ArenaTelemetry.tpp;")
list(TRANSFORM TelemetryTemplateFiles PREPEND ${TelemetryDir})

set(UtilFiles "Constants.h;Utilities.h")
list(TRANSFORM UtilFiles PREPEND ${UtilDir})

#set(IncludeFiles "DoxygenGroups.h")
#list(TRANSFORM IncludeFiles PREPEND ${IncludeDir})

add_library(MemoryManager INTERFACE ${AllocatorHeaderFiles} ${AllocatorTemplateFiles} ${UtilFiles})

target_compile_features(MemoryManager INTERFACE cxx_std_20)
set_target_properties(MemoryManager PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

target_sources(
        MemoryManager
        INTERFACE
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
        ${AllocatorHeaderFiles}
        ${AllocatorTemplateFiles}
        ${TelemetryHeaderFiles}
        ${TelemetryTemplateFiles}
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

source_group(TREE ${IncludeDir} PREFIX "Header Files" FILES
        ${AllocatorHeaderFiles}
        ${TelemetryHeaderFiles}
        ${UtilFiles}
)

source_group(TREE ${IncludeDir} PREFIX "Template Files" FILES
        ${AllocatorTemplateFiles}
        ${TelemetryTemplateFiles}
)