include_guard()

set(CurrentDir ${CMAKE_CURRENT_SOURCE_DIR}/peregrine)

include(${CurrentDir}/allocators/Allocators.cmake)
include(${CurrentDir}/telemetry/Telemetry.cmake)
include(${CurrentDir}/utils/Utils.cmake)

set(HeaderFiles ${AllocatorHeaders} ${TelemetryHeaders} ${UtilHeaders})
list(TRANSFORM HeaderFiles PREPEND "peregrine/")