include(peregrine/Peregrine.cmake)

add_library(Peregrine INTERFACE ${HeaderFiles})

target_compile_features(Peregrine INTERFACE cxx_std_20)
set_target_properties(Peregrine PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

target_include_directories(
        Peregrine
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Peregrine INTERFACE StrictWarnings)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES
        ${HeaderFiles}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CMake Files" FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/allocators/Allocators.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/telemetry/Telemetry.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/utils/Utils.cmake
)