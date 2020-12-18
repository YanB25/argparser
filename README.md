# ArgParser: A lightweight C++ command-line parser

ArgParser is a lightweight command line parser written in C++. The goal of ArgParser is to provide a most convenient and easy-to-use library for building command-line programs.

ArgParser is lightweight and has no external dependency.

Feel free to pull-request or open an issue if you have any problems.

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

    parser.parse(argc, argv);
    parser.print_promt(argc, argv);
    return 0;
}
```

That's it. Compile it and run, we get:

``` bash
$./helloworld
A Hello-world Command-line Tool

Usage:
        ./bin/helloworld [command]
        ./bin/helloworld [flag]

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

See `bin/helloworld.cpp` for the source codes of the above example. `bin/*.cpp` are other examples for you to play around.

## Concept

The central concepts of ArgParser are *commands* and *flags*. Any label started with `--` or `-` is regarded as a flag(e.g. `--time`), while others as a command(e.g. `run`). Defining a new *command* essentially enters a new private namespace. Namespaces can be nested arbitrarily. Only the specific flags defined within the current namespace(command) is recognized and parsed. Providing extra flags within other namespaces, including the parent namespace, is deemed a casual mistake.

*global flags* are also support to specify global setting/constraints for the program.

## Basic Usage

### Flag Registration

Depending on how you want to place the flags, Argparser supports both *registered flag* and *stored flag*.


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

### Flag Scope

Depending on the scope of the flag, ArgParser supports *scoped flag* and *global flag*.

**scoped flag**  The flags in the previous examples are all scoped flags(using `parser.flag(...)`). They only exist in the current namespace. 

**global flag**  ArgParser also supports global flags that exist in all levels of namespaces.

``` c++
parser.global_flag(&flag, "--global-flag", "-g", "A global flag");
parser.global_flag("--stored-global-flag", "-s", "A stored global flag");
```

You can always provide a global flag in all levels of commands.

``` bash
./program --global-flag=5 # okay
./program run --global-flag=5 # okay
./program run ycsb TypeA quick --global-flag=5 # also okay
```

### Explanation of Store

Use `parser.store()` to obtain a const reference, which is the only way to access the store.

The store provides two API, `store.has("--time")` and `store.get("--time")`. The former tells whether the flag exists in the store. The latter tries to retrieve an *internal representation* of the flag-value. If the retrieval fails, the program crashes immediately to avoid potential errors, so be aware.

`store.get(..)` gives an internal representation because ArgParser does not know the *type* of the flag. For example, parsing `--flag=true` to `std::string` or `bool` gives different results. You must use `store.get(...).to<T>` to obtain the final results, as did in the above codes.

### Basic Assumptions

ArgParser assumes several basic rules:

- If `parser.parse(argc, argv)` returns false, accessing the *registered flags* or `ParserStore` is undefined behaviour. MAKE SURE you checked the success of `parser.parse` before continuing your program.


- Any flag without default value MUST BE provided in the command-line. Otherwise, `parser.parse(argc, argv)` complains and returns false. It means that ArgParser does not support *optional flag* by design. 

- Conversion failure (e.g. provides `5.2` to an `int` flag) or additional/unknown flags provided are all regarded as errors, causing `parser.parse` complains and returns false. ArgParser imposes strict checks.
  
- Not suprising, local variables registered to ArgParser MUST BE valid during `parser.parse`.



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

## Compile and use

``` bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
# Unit tests are included by default. If you want to disable them, use
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
$ ./bin/arg
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