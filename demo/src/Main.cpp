/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "utils/Constants.h"

#include <allocators/Arena.h>
#include <iostream>


// TODO: Add getters for stats since there are non-existent without the preprocessor macro
#define ENABLE_PMM_TELEMETRY

int main() {

    using namespace pmm::constants;

    // TODO: Reduce API Friction with the dual inits
    pmm::ArenaTelemetry telemetry{5_KB};
    pmm::Arena arena(5_KB, telemetry);


 }
