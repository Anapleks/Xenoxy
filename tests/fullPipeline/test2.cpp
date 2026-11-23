#include <Xenoxy/Xenoxy.hpp>
#include <chrono>
#include <iostream>

int main()
{
    std::string src = R"(
fn main() -> int {
    var fruitCounts: int[] = [1, 20, 30, 40];
    fruitCounts[fruitCounts[0] + 1] = 15;
    fruitCounts[fruitCounts[0]] = 35;
    fruitCounts[2] = fruitCounts[2] * fruitCounts[1];
    return fruitCounts[2];
})";
/*
    Xenoxy array syntax:
    [expr, expr, expr, ..., expr] - array declaration with set values
    name[expr]                    - array indexing
    new type[expr]                - array declaration with given size
*/

    constexpr int ITERATIONS = 100000;

    std::int64_t totalLoadTime = 0;
    std::int64_t totalRunTime = 0;

    for(int i = 0; i < ITERATIONS; i++) {
        Xenoxy::Runtime runtime;
        auto start = std::chrono::steady_clock::now();
        runtime.Load(src);
        auto loadEnd = std::chrono::steady_clock::now();
        Xenoxy::ObjectInst retVal = runtime.Run();
        // std::cout<<*(std::int64_t*)retVal.Get()<<std::endl;
        auto runEnd = std::chrono::steady_clock::now();

        auto elapsedRun = std::chrono::duration_cast<std::chrono::microseconds>(runEnd - loadEnd);
        auto elapsedLoad = std::chrono::duration_cast<std::chrono::microseconds>(loadEnd - start);
        totalLoadTime += elapsedLoad.count();
        totalRunTime += elapsedRun.count();
    }

    std::cout<<"Test over "<<ITERATIONS<<" iterations:"<<std::endl;
    std::cout<<"AVG Load: "<<totalLoadTime/double(ITERATIONS)<<" us"<<std::endl;
    std::cout<<"AVG Run: "<<totalRunTime/double(ITERATIONS)<<" us"<<std::endl;
    // expected result: int 525
}
