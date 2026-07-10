include(allocators/Allocators.cmake)
include(telemetry/Telemetry.cmake)
include(utils/Utils.cmake)

set(HeaderFiles ${AllocatorHeaders} ${TelemetryHeaders} ${UtilHeaders})

add_library(MemoryManager INTERFACE ${HeaderFiles})


target_compile_features(MemoryManager INTERFACE cxx_std_20)
set_target_properties(MemoryManager PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

target_include_directories(
        MemoryManager
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(MemoryManager INTERFACE StrictWarnings)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES
        ${HeaderFiles}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CMake Files" FILES
        allocators/Allocators.cmake
        telemetry/Telemetry.cmake
        utils/Utils.cmake
)