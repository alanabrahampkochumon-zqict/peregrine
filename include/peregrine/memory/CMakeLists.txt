include_guard()

add_library(PeregrineMemory OBJECT Memory.h)
target_compile_features(PeregrineMemory PUBLIC cxx_std_20)
set_target_properties(PeregrineMemory PROPERTIES LINKER_LANGUAGE CXX)

if (PMM_CURRENT_PLATFORM STREQUAL "Windows")
    list(APPEND SOURCE_FILES WinMemory.cpp)
    add_compile_definitions(PMM_PLATFORM_WINDOWS=1)
    message(STATUS "[Peregrine] Configuring memory exclusively for Windows.")
elseif (PMM_CURRENT_PLATFORM STREQUAL "Linux")
    list(APPEND SOURCE_FILES LinuxMemory.cpp) # TODO: Add Files
    add_compile_definitions(PMM_PLATFORM_LINUX=1)
    message(STATUS "[Peregrine] Configuring memory exclusively for Linux.")
elseif (PMM_CURRENT_PLATFORM STREQUAL "Darwin")
    list(APPEND SOURCE_FILES MacMemory.cpp) # TODO: Add Files
    add_compile_definitions(PMM_PLATFORM_MACOS=1)
    message(STATUS "[Peregrine] Configuring memory exclusively for Linux.")
    # TODO: Add Platform Support
    #elseif(PMM_CURRENT_PLATFORM STREQUAL "PS5")
    #    list(APPEND SOURCE_FILES PS5Memory.cpp)
    #    add_compile_definitions(PMM_PLATFORM_PLAYSTATION=1)
    #    message(STATUS "[Peregrine] Configuring exclusively for PS5.")

    #elseif(PMM_CURRENT_PLATFORM STREQUAL "Xbox")
    #    list(APPEND SOURCE_FILES XboxMemory.cpp)
    #    add_compile_definitions(PMM_PLATFORM_XBOX=1)
    #    message(STATUS "[Peregrine] Configuring exclusively for Xbox.")

    #elseif(PMM_CURRENT_PLATFORM STREQUAL "Switch")
    #    list(APPEND SOURCE_FILES SwitchMemory.cpp)
    #    add_compile_definitions(PMM_PLATFORM_SWITCH=1)
    #    message(STATUS "[Peregrine] Configuring exclusively for Switch.")
endif ()

target_sources(
        PeregrineMemory PRIVATE ${SOURCE_FILES}
)