#pragma once
/**
 * @file Preprocessors.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 11, 2026
 *
 * @brief Preprocessor Macro definitions.
 *
 * @copyright Copyright(c) 2026 Alan Abraham P Kochumon
 */



#include <iostream>


/**
 * @addtogroup PMM_Macro
 * @{
 */

/**
 * @brief Triggers a compiler-specific breakpoint.
 */
#ifdef _MSC_VER
    #define PMM_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define PMM_DEBUG_BREAK() __builtin_trap()
#else
    #include <signal.h>
    #define PMM_DEBUG_BREAK() raise(SIGTRAP)
#endif


namespace pmm::internal
{
    /**
     * @brief An internal function used for triggering compile time error with assertions.
     *        Since all the functions are marked as noexcept, throwing inside `std::is_constant_evaluated`
     *        will not work. So by calling a non constexpr function inside a constexpr function, it will trigger
     *        a no-compile error.
     */
    inline void compileTimeErrorTrap() {}

} // namespace pmm::internal


/**
 * @brief Log the assertion message and status to console.
 *
 * @param condition The condition that triggered the assertion.
 * @param message   An optional message, stating the reason behind assertion failure.
 * @param file      The name of the file that triggered the assertion.
 * @param line      The line number of hte code that triggered the assertion.
 */
inline void logAssertion(const char* condition, const char* message, const char* file, const int line)
{
    std::cerr << "[PMM ASSERTION FAILED]\n"
              << "Condition: " << condition << '\n'
              << "Message:   " << (message ? message : "None") << '\n'
              << "Location:  " << file << ":" << line << '\n';
}


/**
 * @def PMM_ASSERT_MSG(condition, message)
 * @brief Asserts a condition is `true`, breaking into the debugger with a custom message if `false`.
 *
 * @note This macro is strictly active only in **Debug** builds. In **Release** builds
 *       (when `NDEBUG` is defined), it expands into a zero-cost void cast.
 * @note To ensure that the macro support compile-time evaluation an exception is thrown,
 *       instead of logging during compile time.
 *
 * @param condition The boolean expression to evaluate.
 * @param message   A string literal or streamable object providing context on the failure.
 */
#ifndef NDEBUG
    #define PMM_ASSERT_MSG(condition, message)                                                                         \
        do                                                                                                             \
        {                                                                                                              \
            if (std::is_constant_evaluated())                                                                          \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                    pmm::internal::compileTimeErrorTrap();                                                             \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                {                                                                                                      \
                    logAssertion(#condition, message, __FILE__, __LINE__);                                             \
                    PMM_DEBUG_BREAK();                                                                                 \
                }                                                                                                      \
            }                                                                                                          \
        } while (false)
#else
    #define PMM_ASSERT_MSG(condition, message) ((void) 0)
#endif

/**
 * @def PMM_ASSERT(condition)
 * @brief Asserts a condition is `true`, breaking into the debugger if `false`.
 *
 * @note This macro is strictly active only in **Debug** builds. In **Release** builds
 *       (when `NDEBUG` is defined), it expands into a zero-cost void cast.
 * @note To ensure that the macro support compile-time evaluation an exception is thrown,
 *       instead of logging during compile time.
 *
 * @param condition The boolean expression to evaluate.
 */
#ifndef NDEBUG
    #define PMM_ASSERT(condition)                                                                                      \
        do                                                                                                             \
        {                                                                                                              \
            if (std::is_constant_evaluated())                                                                          \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                    pmm::internal::compileTimeErrorTrap();                                                             \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                if (!(condition))                                                                                      \
                {                                                                                                      \
                    logAssertion(#condition, "", __FILE__, __LINE__);                                                  \
                    PMM_DEBUG_BREAK();                                                                                 \
                }                                                                                                      \
            }                                                                                                          \
        } while (false)
#else
    #define PMM_ASSERT(condition) ((void) 0)
#endif


/**
 * @brief Compiler specific inlining syntax.
 */
#ifdef _MSC_VER
    #define PMM_INLINE __forceinline inline
#elif defined(__GNUC__) || defined(__clang__)
    #define PMM_INLINE inline __attribute__((always_inline))
#else
    #define PMM_INLINE inline
#endif

/** @} */
