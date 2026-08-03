#pragma once
/**
 * @file StackTestSetup.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Contains all test fixtures used by all stack allocator tests.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>
#include <peregrine/utils/Constants.h>



using namespace pmm::constants;
constexpr auto STACK_SIZE = 5_KB;
class LooseStack: public testing::Test
{

public:
    std::size_t stackSize{ STACK_SIZE };
    pmm::Stack<pmm::stack::Loose> stack{ stackSize };
};


class StrictStack: public testing::Test
{
public:
    std::size_t stackSize{ STACK_SIZE };
    pmm::Stack<pmm::stack::Strict> stack{ stackSize };
};



