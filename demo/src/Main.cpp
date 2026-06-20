/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#define ENABLE_PMM_TELEMETRY // Must be defined about all includes to enabled telemetry

#include <peregrine/allocators/Arena.h>
#include <peregrine/utils/Constants.h>

#include <iostream>



void printArenaTelemetry(const pmm::ArenaTelemetry& telemetry)
{
    std::cout << "Arena Telemetry\n";
    std::cout << "Total Memory: " << telemetry.getArenaSize() <<" bytes.\n";
    std::cout << "Used Memory: " << telemetry.getCurrentUsage() <<" bytes.\n";
    std::cout << "Free Memory: " << telemetry.getArenaSize() - telemetry.getCurrentUsage() <<" bytes.\n";
    std::cout << "Peak Memory Footprint: " << telemetry.getPeakUsage() <<" bytes.\n";
    std::cout << "Minimum Memory Footprint: " << telemetry.getMinUsage() <<" bytes.\n\n\n";
}

struct alignas(16) Vec4
{
    float x, y, z, w;
    Vec4(const float x, const float y, const float z, const float w): x(x), y(y), z(z), w(w) {};


    [[nodiscard]] Vec4 operator+(const Vec4& rhs) const noexcept
    {
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec4& vec)
    {
        os << "<" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ">";
        return os;
    }
};



int main() {

    using namespace pmm::constants;

    // TODO: Add byte formatter
    // TODO: Reduce API Friction with the dual inits
    constexpr auto arenaSize = 5_KB;
    pmm::ArenaTelemetry telemetry{arenaSize};
    pmm::Arena arena(arenaSize, &telemetry);

    const auto a = arena.alloc<int>(5);
    printArenaTelemetry(telemetry);
    const auto b = arena.alloc<int>(10);
    printArenaTelemetry(telemetry);

    const auto vecA = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    printArenaTelemetry(telemetry);

    const auto vecB = arena.alloc<Vec4>(5.0f, 6.0f, 7.0f, 8.0f);
    printArenaTelemetry(telemetry);

    std::cout << *vecA << " + " << *vecB << " = " << *vecA + *vecB << "\n\n";
    std::cout << "Result: " << *a + *b << "\n\n";

    std::cout << "Freeing arena...\n";

    arena.freeAll();

    printArenaTelemetry(telemetry);

 }
