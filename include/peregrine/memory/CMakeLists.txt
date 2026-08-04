include_guard()

add_library(PeregrineMemory OBJECT Memory.h WinMemory.cpp)

#set(MemoryDir "memory/")
#set(MemoryHeaders
#        Memory.h
#)
#
#list(TRANSFORM MemoryHeaders PREPEND ${MemoryDir})