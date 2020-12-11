# ArgParser: A lightweight C++ command-line parser

ArgParser is a lightweight command line parser written in C++. The goal of ArgParser is to provide a convenient and easy-to-use library for building command-line programs.

ArgParser is lightweight and has no dependency.

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

The magic of the lib is that the command-line arguments are validated, parsed and stored into the variables you registered with `parser.flag(...)`. ArgParser even recognizes `vector` and `string` as well as the primitive types.

To provide a flag, use either `--flag value` or `--flag=value`.

See `bin/helloworld.cpp` for the source codes of the above example. `bin/*.cpp` are other examples for you to play around.

## Concept

The central concepts of ArgParser are *commands* and *flags*. Defining a new *command* essentially enters a new namespace. Namespaces can be nested arbitrarily. Only the specific flags defined within the current command(namespace) is recognized and parsed. Providing extra flags within other namespaces, including the parent namespace, is deemed a careless mistake.

*global flags* are also support to specify global setting/constraints for the program.

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