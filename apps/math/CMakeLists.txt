add_library(PSMath INTERFACE
        psm/PSMath.h
        psm/vec/Vec2.h
        psm/vec/Vec3.h
        psm/Rotations.h
        psm/Utils.h
)

target_include_directories(PSMath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})