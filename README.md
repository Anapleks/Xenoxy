# Xenoxy
Xenoxy is an interpreted programming language, designed with embedding in mind.

## Compilation

1. create a build directory:
```bash
mkdir -p build
```
2. Configure the cmake build directory using:
```bash
cmake -S . -B build
```
Tests are disabled by default, so if you prefer to build with test compilation enabled, instead use:
```bash
cmake -S . -B build -DXENOXY_COMPILE_TESTS=ON
```
3. Finally, build the project into the build directory:
```bash
cmake --build build
```

## Usage
### Instructions
Link the generated Xenoxy library into your project and add ```Xenoxy.h``` to your project's include path. <br><br>
To use Xenoxy, begin by creating a ```Xenoxy::Runtime``` class instance, and calling its ```.Load()``` function,<br>
with your Xenoxy source code specified as the only argument.<br><br>
Next, call your specified entrypoint function by calling the runtime's ```.Run()``` function. By default,
```Run()``` executes the ```main``` function. Alternatively, you can specify an entrypoint by passing its name as an argument.
Once Xenoxy is done with the function call, it will print to stdout the resulting value. Currently, the embedding interface
does not provide a way to retrieve this value directly.
### Example
```cpp
#include <string>
#include <Xenoxy.h>

int main() {
    std::string src = R"(
fn main() -> int {
    var a: int = 15;
    return a;
})";

    Xenoxy::Runtime runtime;
    runtime.Load(src);
    runtime.Run();
}
```

### Docs
More detailed syntax and documentation for Xenoxy can be found [here](docs/docsMain.md)

## Known Limitations
- No access to returned value
- Very limited embedding interface
- Only one fuly supported type (signed 64-bit integer)