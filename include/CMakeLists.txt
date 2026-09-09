include(peregrine/Peregrine.cmake)

add_library(Peregrine STATIC ${HeaderFiles})

set_target_properties(Peregrine PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(Peregrine PUBLIC cxx_std_20)
set_target_properties(Peregrine PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

add_subdirectory(peregrine/memory)

target_include_directories(
        Peregrine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Peregrine PUBLIC StrictWarnings PeregrineMemory)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES
        ${HeaderFiles}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CMake Files" FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/allocators/Allocators.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/memory/CMakeLists.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/telemetry/Telemetry.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/utils/Utils.cmake
)