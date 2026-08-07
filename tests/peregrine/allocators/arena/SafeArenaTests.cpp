//
// Created by Alan Abraham on 8/7/2026.
//
// TODO: Safe Arena Test add death test instead here
//

// TEST_F(ManagedArenaTests, ReturnsNullPtrWhenAllocatingMemoryGreaterThanArenaSize)
// {
//     constexpr auto size = 512;
//     Arena arena(size);
//
//     void* bytes = arena.allocBytes(size + 1);
//
//     EXPECT_EQ(nullptr, bytes);
// }

// Returns nullptr if arena is full for allocBytes, allocV, alloc
// TEST_F(ManagedArenaTests, Alloc_ReturnsNullPtrWhenAllocatingInAFullArena)
// {
//     constexpr auto size = 512;
//     Arena arena(size);
//
//     // Use the full capacity
//     [[maybe_unused]] const auto fullSize = arena.allocBytes(size);
//
//     const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     EXPECT_EQ(nullptr, vec);
// }

// TODO: check if this test exist in stack
// TEST_F(ManagedArenaTests, AllocV_FullArenaReturnsEmptySpan)
// {
//     constexpr auto size       = 1024;
//     constexpr auto blockCount = 10;
//     pmm::Arena arena(size);
//     static_cast<void>(arena.allocBytes(size - 1));
//
//     const auto vertices = arena.allocV<Vec4>(blockCount);
//
//     EXPECT_EQ(0, vertices.size());
//     EXPECT_EQ(0, vertices.size_bytes());
// }
