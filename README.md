# ArgParser: A lightweight C++ command-line parser

ArgParser is a lightweight command line parser written in C++. The goal of ArgParser is to provide a convenient easy-to-use library for building a command-line program.

ArgParser is lightweight because it has no dependency.

Feel free to pull request or open an issue if you have any problem.

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

To provide a flag, `--flag value` and `--flag=value` are both supported.

See `bin/helloworld.cpp` for the source codes of the above example. `bin/*.cpp` are other examples for you to play around.

## Compile and sse

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

``` bash
$ ./bin/main
argparser world!
```

## Others

### clang-format

`clang-format` is an opt-in if you would like to use.

``` bash
sudo apt install clang-format
```

### Doxygen

[doxygen](https://www.doxygen.nl/index.html) is supported if needed.