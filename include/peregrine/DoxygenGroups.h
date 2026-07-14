#pragma once
/**
 * @file DoxygenGroups.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Doxygen group for organizing Peregrine Memory library into modular units.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


// clang-format off
/**
 * @defgroup PMM_Lib Peregrine Memory Manager
 * @brief High performance memory manager suite.
 * @{
 */
    
    /**
     * @defgroup PMM_Constants Memory Literals & Constants
     * @brief Literals for common memory units.
     * @ingroup PMM_Lib
     */


    /**
     * @defgroup PMM_Arena Arena Allocator
     * @brief Arena/Linear Allocator.
     * @ingroup PMM_Lib
     */

    /**
     * @defgroup PMM_Stack Stack Allocator
     * @brief Stack Allocator.
     * @ingroup PMM_Lib
     */


    /**
     * @defgroup PMM_TArena Temporary Arena
     * @brief Temporary Arena(Snapshot).
     * @ingroup PMM_Lib
     */

    /**
     * @defgroup PMM_Telemetry Telemetry
     * @brief Telemetry statistics for Allocators.
     * @ingroup PMM_Lib
     */

    /**
     * @defgroup PMM_Macro Macro Definitions
     * @brief Preprocessor macro definitions.
     * @ingroup PMM_Lib
     */

    /**
     * @defgroup PMM_Storage_Policy Storage Policy
     * @brief Storage policies used by allocators for changing internal behavior.
     * @ingroup PMM_Lib
     */
    
/** @} */ // End of PMM_Lib

// clang-format on
