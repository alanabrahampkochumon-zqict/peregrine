include_guard()

message(STATUS "Configuring Strict Warnings")
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(StrictWarnings INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:/WX;/W4;/permissive-;/fp:strict;/wd4723;>
    )
elseif (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(StrictWarnings INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-Werror>
            $<$<CXX_COMPILER_ID:Clang>:
            # Backwards compatibility warning suppression
            /clang:-Wno-c++98-compat;
            /clang:-Wno-c++98-compat-pedantic;
            /clang:-Wno-pre-c++14-compat;
            /clang:-Wno-pre-c++17-compat;
            /clang:-Wno-c++20-compat;

            /clang:-Wno-global-constructors; # Gtest
            /clang:-Wno-covered-switch-default;
            /clang:-Wno-switch-default; # Gtest
            /clang:-Wno-unique-object-duplication;
            /clang:-Wno-header-hygiene;
            /clang:-Wno-float-equal;
            /clang:-Wno-unsafe-buffer-usage;
            /clang:-Wno-padded; # Gtest
            /clang:-Wno-implicit-int-float-conversion;
            /clang:-Wno-double-promotion;
            /clang:-Wno-implicit-int-conversion-on-negation
            /clang:-Wno-reserved-macro-identifier; # Required by cl-clang on CLion due to macro conflicts
            /clang:-Wno-unused-macros; # Required by cl-clang on CLion due to macro conflicts
            >
    )
    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
else ()
    target_compile_options(StrictWarnings INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Werror;-pedantic-errors>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
    )
endif ()