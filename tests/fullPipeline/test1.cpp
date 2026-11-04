#include <Xenoxy/Xenoxy.hpp>

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
    Xenoxy::Runtime runtime;
    runtime.Load(src);
    runtime.Run();
    // expected result: int 9223372036854775807
}
