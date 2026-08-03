#pragma once
/**
 * @file DoxygenGroups.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Doxygen groups for categorizing tests.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


// clang-format off
/**
 * @defgroup T_PMM_Lib Peregrine Memory Manager Test Suite
 * @brief Complete test suite for PMM.
 * @{
 */
    
    /**
     * @defgroup T_PMM_Constants Memory Literals Tests
     * @brief Verify Memory Literals.
     * @ingroup T_PMM_Lib
     */

    /**
    * @defgroup T_PMM_Arena Arena Allocator Tests
    * @brief Verify Arena Allocator Logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_TArena Temporary Arena Tests
    * @brief Verify Temporary Arena Logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Loose_Stack Loose Stack Allocator Tests
    * @brief Verify Stack<pmm::stack::Loose> Allocator Logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Strict_Stack Strict Stack Allocator Tests
    * @brief Verify Stack<pmm::stack::Loose> Allocator Logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Pool Pool Allocator Tests
    * @brief Verify Pool allocator allocation and deallocation logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Stack_Tel_Int Stack Telemetry Integration
    * @brief Verifies stack telemetry integration across different policies.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Telemetry Telemetry Tests
    * @brief Verify Memory Allocator's Telemetry Logic.
    * @ingroup T_PMM_Lib
    */

    /**
    * @defgroup T_PMM_Helpers Helper Function Tests
    * @brief Verifies Helper Functions.
    * @ingroup T_PMM_Lib
    */
    
/** @} */ // End of FGM_Math

// clang-format on
