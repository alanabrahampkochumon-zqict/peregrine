include_guard()

add_library(PeregrineMemory OBJECT Memory.h WinMemory.cpp)
target_compile_features(PeregrineMemory PUBLIC cxx_std_20)

#set(MemoryDir "memory/")
#set(MemoryHeaders
#        Memory.h
#)
#
#list(TRANSFORM MemoryHeaders PREPEND ${MemoryDir})