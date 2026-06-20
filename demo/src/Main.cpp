/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <peregrine/allocators/Arena.h>
#include <peregrine/utils/Constants.h>

#include <iostream>


#define ENABLE_PMM_TELEMETRY

// void printArenaTelemetry(const pmm::ArenaTelemetry)


int main() {

    using namespace pmm::constants;

    // TODO: Reduce API Friction with the dual inits
    pmm::ArenaTelemetry telemetry{5_KB};
    pmm::Arena arena(5_KB, &telemetry);


 }
