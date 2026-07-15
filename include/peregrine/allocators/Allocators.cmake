include_guard()

set(AllocatorDir "allocators/")
set(AllocatorHeaders
        Arena.h
        Arena.tpp
        Stack.h
        Stack.tpp
        StackType.h
        TempArena.h
        TempArena.tpp
)
list(TRANSFORM AllocatorHeaders PREPEND ${AllocatorDir})