#include <Xenoxy/Xenoxy.hpp>
#include <chrono>
#include <iostream>

int main()
{
    std::string src = R"(
fn pow(base: int, exp: int) -> int {
    var res: int = 1;
    var curr_base: int = base;
    until(exp == 0) {
        if(exp % 2 == 1) {
            res = res * curr_base;
        }
        curr_base = curr_base * curr_base;
        exp = exp / 2;
    }
    return res;
}

fn main() -> int {
    var apples: int = 10;
    apples = pow(2, 63) - 1;
    return apples;
})";

    constexpr int ITERATIONS = 10000;

    std::int64_t totalLoadTime = 0;
    std::int64_t totalRunTime = 0;

    for(int i = 0; i < ITERATIONS; i++) {
        Xenoxy::Runtime runtime;
        auto start = std::chrono::steady_clock::now();
        runtime.Load(src);
        auto loadEnd = std::chrono::steady_clock::now();
        runtime.Run();
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
