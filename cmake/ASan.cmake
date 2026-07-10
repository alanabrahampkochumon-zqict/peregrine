include_guard()

message(STATUS "Turning on Address Sanitizer for Debug Builds")
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/fsanitize=address;/Zi>)
    target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/INCREMENTAL:NO>)
elseif (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # TODO: Fix Cl-Clang Address Sanitizer
else ()
    target_compile_options(StrictWarnings INTERFACE
            $<$<CONFIG:Debug>:-fsanitize=address>
            $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GCC>>>:-fno-omit-frame-pointer>
    )
    target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
endif ()