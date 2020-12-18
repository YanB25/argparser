# ArgParser: A lightweight C++ command-line parser

ArgParser is a lightweight command line parser written in C++. The goal of ArgParser is to provide the most convenient and easy-to-use library for building command-line programs.

**ArgParser is a header-only library. The packed single-header file is available at [release](https://github.com/YanB25/argparser/releases) or  [argparser.hpp](release/argparser.hpp).**

The library is suitable for you, if you

- are looking for a correct, most convenient and easy-to-use command-line parser.

- do not care for extremely high performance and memory occupation. (ArgParser is not slow or using memory blindly, but it is not optimized for these goals.)

You are not suggested to use this library if you

- are writing codes for production environments.

**Feel free to pull-request or open an issue if you have any problems.**

## Quick Start

Building a hello-world command-line tool with ArgParser only takes several lines of code.

``` c++
int main(int argc, const char *argv[])
{
    auto &parser = argparser::init("A Hello-world Command-line Tool");

    std::vector<int> array;
    int size;
    parser.flag(&array, "--array", "-A", "An array of int as input", "1,2,3");
    parser.flag(&size, "--size", "-S", "The size of items", "0");

    int time;
    auto &run_parser =
        parser.command("run", "A sub-command, used to run something");
    run_parser.flag(&time, "--time", "-T", "The time to run", "5");

    // DO check the success status of this function
    // and print messages when fails.
    if (!parser.parse(argc, argv))
    {
        parser.print_promt(argc, argv);
        return 0;
    }
    return 0;
}
```

That's it. Compile it and run, we get:

``` bash
$./helloworld
A Hello-world Command-line Tool

Usage:
        ./examples/helloworld [command]
        ./examples/helloworld [flag]

Available Commands:
  run     A sub-command, used to run something

Flags:
  -A, --array
               An array of int as input
  -S, --size
               The size of items

```

The magic of the lib is that the command-line arguments are validated, parsed and stored into the local variables you registered with `parser.flag(...)`. ArgParser even recognizes `vector` and `string` in addition to the primitive types.

To provide a flag, use either `--flag value` or `--flag=value`.

See [examples/helloworld.cpp](examples/helloworld.cpp) for the source codes of the above example. `examples/*.cpp` are other examples for you to play around.

## Concept

The central concepts of ArgParser are *commands* and *flags*. Any label started with `--` or `-` is regarded as a flag(e.g. `--time`), while others as a command(e.g. `run`).

Defining a new *command* essentially enters a new private flag-space. Flag-spaces can be nested arbitrarily. Only the specific flags defined within the current space is recognized and parsed. Providing extra flags within other flag-spaces, including the parent/root flag-space, is deemed a casual mistake.

*global flags* are also supported to specify the global setting/constraints for the program.

## Basic Usage

### Flag Registration

Depending on who owns the storage of the flags, Argparser supports both *registered flag* and *stored flag*.


**Registered Flag**  You can either *register* an in-stack variable to ArgParser as in the previous example.

``` c++
int data;
// data is a registered flag
parser.flag(&data, "--data", "-d", "The data we need");
```

**Stored Flag**  If you do not provide an in-stack variable to ArgParser, the storage is automatically allocated in `Store` for your later retrieval.

``` c++
// note: no local variable provided
// --time is a stored flags
parser.flag("--time", "-t", "The time we need");
```

After the successful invocation of `parser.parse(..)`, all the registered flags are fine to access. You must retrieve the stored flags manually through the `store` API.


``` c++
if (!parser.parse(argc, argv))
{
    die();
}
// get the store for stored flags
auto& store = parser.store();
// retrieve the stored flags here.
int time = store.get("-t").to<int>();
std::cout << "data is " << data << ", time is " << time << std::endl;
```

### Commands

You can define *command* in ArgParser for hierarchical groups of actions and flags. Commands provide isolation: whenever a command is defined, a new `sub-parser` is generated. The flag is only recognized in the corresponding (sub-)parser.

``` c++
auto& sub_parser = parser.command("run", "Run the program");
sub_parser.flag("--time", "-t", "Duration of the program");
```

The above codes define a command `run` to the program and attach a flag `--time` *to the command*.

``` bash
./program --time       # ERROR: --time flag not found
./program run --time   # Good.
```

Please note that the child sub-parser will *NOT* inherit the flags from its parent parser. Providing a flag registered in the parent parser generates an "unknown flag" error.


### Flag Scope

Depending on the scope of the flag, ArgParser supports *scoped flag* and *global flag*.

**scoped flag**  They scoped flags only take effect in the current flag-space (the current command). The flags in the previous examples are all scoped flags (using `parser.flag(...)`). 

**global flag**  ArgParser also supports global flags that exist in all levels of flag-spaces. Global flags are especially helpful when setting global constraints/parameters to the whole program.

``` c++
parser.global_flag(&flag, "--global-flag", "-g", "A global flag");
parser.global_flag("--stored-global-flag", "-s", "A stored global flag");
```

The global flags are always parsed regardless of the current level of flag-space.

``` bash
./program --global-flag=5                         # okay
./program run --global-flag=5                     # okay
./program run ycsb type_a quick --global-flag=5   # also okay
```

### Introduction to Store API

The `Store` is only accessible through `parser.store()`, which gives a const reference to the un-copyable `store` object.

The store provides two API, `store.has("--time")` and `store.get("--time")`. The former tells whether the flag exists in the store. The latter retrieves an *internal representation* (explained later) of the flag's value. If the retrieval fails, the program crashes immediately to avoid potential errors, so be careful.

`store.get(..)` gives an internal representation because ArgParser does not know the *type* of the flag. For example, parsing `--flag=true` to `std::string` or `bool` gives different results. You must use `store.get(...).to<T>` to specify type conversion and obtain the final results, as did in the above codes.

### Basic Assumptions

ArgParser assumes several basic rules:

- If `parser.parse(argc, argv)` returns false, accessing the *registered flags* or `ParserStore` is undefined behaviour. MAKE SURE you checked the success of `parser.parse` before continuing your program.


- Any flag without default value MUST BE provided in the command-line. Otherwise, `parser.parse(argc, argv)` complains and returns false. It means that ArgParser does not support *optional flag* by design. 

- Conversion failure (e.g. provides `5.2` to an `int` flag) or additional/unknown flags provided are all regarded as errors, causing `parser.parse` complains and returns false. ArgParser imposes strict checks.
  
- Not suprising, local variables registered to ArgParser MUST BE valid during `parser.parse`.

## Advanced Usage

### Flag Type

ArgParser respects the *type* of the flag. A string flag `"true"` and a boolean flag `true` are different to ArgParser.

ArgParser smartly parse the command-line arguments according to the required type.

``` c++
// two ways ArgParser gets the type information
bool b;
parser.flag(&b, ...); // ArgParser knows it is a boolean

// ArgParser dest not know the type for now.
parser.flag("--flag", "-f"); 
auto& store = parser.store();
assert(store.get("--flag").convertable_to<bool>()); // assert convertable
bool b2 = store.get("--flag").to<bool>(); // ArgParser knowns you want a bool
```

Please note that `.to<T>()` will immediately crash the program if the conversion fails. You can use `.convertable_to<T>()` to check the compatibility.

### String and Vector

ArgParser also supports `string`, `vector`, even `vector<string>`.

``` c++
std::vector<int> array;
parser.flag(&array, "--array", "-a"); // okay

store.get("--another-array").to<std::vector<int>>(); // also okay
```

Provide a list in your command-line arguments, e.g.

``` bash
./program --array 1,2,3,4,5           # okay
./program --another-array=1,2,3,4,5   # also okay
```

### Custom Type

You can also use ArgParser to parse custom types if you provide your customed conversion from `std::string` to your type.

``` c++
class Bar;
// provide your conversion
template <>
Bar argparser::convert::to<Bar>(const std::string& input)
{
    // ...
    return bar;
}
```

Then you can use your *Bar-type* flag as usual.

``` c++
parser.flag(&bar, "--bar", "-b", "My special Bar type"); //okay
// or
Bar bar = store.get("--stored-bar").to<Bar>(); // okay!

```

## Why me

The unique advantage of ArgParser over other similar implementations is the support of *structural parsing*.

``` bash
# example of flat input
program
    ├── flag-a
    ├── flag-b
    └── flag-c

# example of structural input
program
    ├── cmd-1
    │   ├── flag-e
    │   └── flag-f
    ├── cmd-2
    │   └── flag-g
    ├── flag-a
    └── flag-b
```

In most command-line tools, *flags* are relevant only in the context of a *command*. ArgParser supports layer structure by parsing command recursively and collecting the relevant flags on the way. Any mismatch flags are deemed as an error.

## Correctness

The unit tests are under `test/` folder, using [gtest](https://github.com/google/googletest).

If you find a bug, you are welcome to open an issue or pull request.

## Compile and use

``` bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
# Unit tests are built by default. If you want to disable them, use
# cmake -DBUILD_UNITTEST=OFF ..
make -j
make test # Run the unit tests. They should pass.
```

To use this project, you can ...

### Install the lib globally

``` bash
sudo make install
# to uninstall:
# cat install_manifest.txt | sudo xargs rm -rf
```

then you are fine to enjoy the lib.

### Copy the files into your project

All the necessary files are in `include/argparser`. A `cp` will work.

## Run

After build,

``` bash
$ ./examples/arg
```

is a more interesting program to play around.
## Others

### clang-format

`clang-format` is an opt-in if you would like to use.

``` bash
sudo apt install clang-format
```

### Doxygen

[doxygen](https://www.doxygen.nl/index.html) is supported if needed.