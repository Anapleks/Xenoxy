#include <Xenoxy/Xenoxy.hpp>
#include <chrono>
#include <iostream>

int main()
{
    std::string src = R"(
fn main() -> int {
    var fruitCounts: int[] = [10, 20, 30];
    fruitCounts[2] = 15;
    fruitCounts[1] = 35;
    return fruitCounts[2];
})";

    constexpr int ITERATIONS = 1;

    std::int64_t totalLoadTime = 0;
    std::int64_t totalRunTime = 0;

    for(int i = 0; i < ITERATIONS; i++) {
        Xenoxy::Runtime runtime;
        auto start = std::chrono::steady_clock::now();
        runtime.Load(src);
        auto loadEnd = std::chrono::steady_clock::now();
        Xenoxy::ObjectInst retVal = runtime.Run();
        std::cout<<*(std::int64_t*)retVal.Get()<<std::endl;
        auto runEnd = std::chrono::steady_clock::now();

        auto elapsedRun = std::chrono::duration_cast<std::chrono::microseconds>(runEnd - loadEnd);
        auto elapsedLoad = std::chrono::duration_cast<std::chrono::microseconds>(loadEnd - start);
        totalLoadTime += elapsedLoad.count();
        totalRunTime += elapsedRun.count();
    }

    std::cout<<"Test over "<<ITERATIONS<<" iterations:"<<std::endl;
    std::cout<<"AVG Load: "<<totalLoadTime/double(ITERATIONS)<<" us"<<std::endl;
    std::cout<<"AVG Run: "<<totalRunTime/double(ITERATIONS)<<" us"<<std::endl;
    // expected result: int 9223372036854775807
}
