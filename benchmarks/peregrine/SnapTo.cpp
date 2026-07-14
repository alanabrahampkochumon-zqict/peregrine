#include <benchmark/benchmark.h>
#include <cstdlib>

auto snapToBasic = [](std::size_t val, std::size_t snap) -> std::size_t {
    if (val % snap == 0)
    {
        return val;
    }
    return ((val / snap) + 1) * snap;
};

auto snapToBranchless = [](std::size_t val, std::size_t snap) -> std::size_t {
    return ((val / snap) + static_cast<std::size_t>(val % snap != 0)) * snap;
};

std::pair<std::size_t, std::size_t> div(std::size_t dividend, std::size_t divisor);
std::pair<std::size_t, std::size_t> div(std::size_t dividend, std::size_t divisor)
{ return { dividend / divisor, dividend % divisor }; }

auto snapToSingleOp = [](long long val, long long snap) -> long long {
    const auto combo = std::div(val, snap);
    return (combo.quot + static_cast<long long>(combo.rem != 0)) * snap;
};

auto snapToSingleOpCustom = [](std::size_t val, std::size_t snap) -> std::size_t {
    const auto combo = div(val, snap);
    return (combo.first + static_cast<std::size_t>(combo.second != 0)) * snap;
};

static void BM_SnapToBasic(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(snapToBasic(24, 24));
    }
}

static void BM_SnapToBranchless(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(snapToBranchless(24, 24));
    }
}

static void BM_SnapToSingleOp(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(snapToSingleOp(24, 24));
    }
}

static void BM_SnapToSingleOpCustom(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(snapToSingleOpCustom(24, 24));
    }
}


// Register the function as a benchmark
BENCHMARK(BM_SnapToBasic);
BENCHMARK(BM_SnapToBranchless);
BENCHMARK(BM_SnapToSingleOp);
BENCHMARK(BM_SnapToSingleOpCustom);
// Run the benchmark
BENCHMARK_MAIN();
