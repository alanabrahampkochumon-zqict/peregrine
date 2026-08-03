include_guard()

set(MemoryDir "memory/")
set(MemoryHeaders
        Memory.h
)

list(TRANSFORM MemoryHeaders PREPEND ${MemoryDir})