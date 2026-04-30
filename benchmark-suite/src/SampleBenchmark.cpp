#include <benchmark/benchmark.h>

auto sum = [](int a, int b)->int
{
        return a + b;
};

static void BM_Sum(benchmark::State& state)
{
    for (auto _ : state)
    {
        sum(5, 9);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Sum);
// Run the benchmark
BENCHMARK_MAIN();