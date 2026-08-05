include_guard()

# Sets up the platform macros for a given target via options.
# If the option is not provided then it is set to the platform being compiled.
# Options:
# - Auto (Automatically detect compiling platform)
# - Windows
# - Darwin (MacOS)
# - Linux
# - Xbox
# - Switch
# - PS5

# NOTE: Platforms like PS5, XBox, and Switch are proprietary and defined under "Generic" Category
#       so autodetect will not work on those platforms.

# Usage: -DPMM_TARGET_PLAFORM=Darwin
# The platform value can be queried using PMM_CURRENT_PLATFORM

set(PMM_TARGET_PLATFORM "Auto" CACHE STRING "Target platform: Auto, Windows, Darwin, Linux, Xbox, Switch, or PS5")
set_property(CACHE PMM_TARGET_PLATFORM PROPERTY STRINGS "Auto" "Windows" "Darwin" "Linux" "Xbox" "Switch" "PS5")


if (PMM_TARGET_PLATFORM STREQUAL "Auto")
    message(STATUS "[Peregrine] No target specified. Auto-detecting host platform...")
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(PMM_CURRENT_PLATFORM "Windows")
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(PMM_CURRENT_PLATFORM "Darwin")
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(PMM_CURRENT_PLATFORM "Linux")
    else ()
        message(FATAL_ERROR "[Peregrine] Unsupported host OS for auto-detection!\nDefine a target platform using -D<Platform>")
    endif ()
else ()
    message(STATUS "[Peregrine] Manual override active. Target Platform set to: ${PMM_TARGET_PLATFORM}")
    set(PMM_CURRENT_PLATFORM ${PMM_TARGET_PLATFORM})
endif ()