#include <iostream>

#include "argparser/argparser.hpp"

void register_arg(argparser::Parser &parser);

int main(int argc, const char *argv[])
{
    auto& parser = argparser::init("lots of commands");
    register_arg(parser);
    parser.parse(argc, argv);
    parser.print_promt(argc, argv);
}

void register_arg(argparser::Parser &parser)
{
    auto &atomic_add = parser.command(
        "atomic-add", argparser::very_long_sentence);
    atomic_add.flag(
        "--threads", "-T", "The number of threads to run simutaneously");
    atomic_add.flag("--time", "-t", "The duration (second) to run");

    auto &cas =
        parser.command("cas", argparser::long_sentence);
    cas.flag("--threads", "-T", "The number of threads");
    cas.flag("--time", "-t", "The duration (second) to run");

    auto &add = parser.command("add", argparser::short_sentence);
    add.flag("--time", "-t", "The duration (second) to run");

    auto &lock = parser.command(
        "lock", argparser::very_short_sentence);
    lock.flag("--threads", "-T", "The number of threads");
    lock.flag("--time", "-t", "The duration (second) to run");

    auto &prefetch =
        parser.command("prefetch", "The effect of __builtin_prefetch(addr)");
    prefetch.flag(
        "--ops", "", "The number of operations of random memory write");

    auto &spin_lock = parser.command(
        "spin-lock", "The performance of lock implemented by CAS");
    spin_lock.flag("--threads", "-T", "The number of threads");
    spin_lock.flag("--time", "-t", "The duration (second) to run");
    spin_lock.flag(
        "--yieldable",
        "-Y",
        "Whether or not the spin lock allow the current thread to yield.",
        "false");

    auto &mem =
        parser.command("mem",
                       "Randomly access memory with cacheline-aligned "
                       "addresses. Lat and Tpt will be reported. This command "
                       "is also useful for testing numa effect.");
    mem.flag("--time", "-t", "The duration (microsecond) to run");

    auto &malloc =
        parser.command("malloc", "The performance of running malloc");
    malloc.flag("--threads", "-T", "The number of threads");
    malloc.flag("--time", "-t", "The duration (millisecond) to run");
    malloc.flag(
        "--size", "-S", "The size of each malloc. e.g. --size 4k or --size 64");

    auto &timing =
        parser.command("timing",
                       "The performance of std::chrono::steady_clock::now() "
                       "and std::chrono::duration_cast<>()");
    timing.flag("--time", "-t", "The duration (second) to run");

    auto &fbench = parser.command("file", "The performance of writing to fd ");
    fbench.flag(
        "--size",
        "-S",
        "The total size for IO. e.g. --size 4M or --size 1G. Default to 4K.");
    fbench.flag(
        "--io-size",
        "-s",
        "The size of IO unit. e.g. --size 4K or --size 64. Default to 4K",
        "4k");
    fbench.flag("--stride",
                "-D",
                "The stride of each IO requests. Default to 4K",
                "4k");
    fbench.flag("--repeat",
                "-R",
                "continuously run `repeat` times. Default to 1.",
                "1");
    fbench.flag(
        "--fsync-interval",
        "-T",
        "Call fsync after each `fsync-interval` IO requests. Set to 0 to call "
        "fsync at the end of each iteration of the benchmark. Set to -1 to "
        "call fsync only at the end of the whole benchmark. If "
        "`fsync-interval` > 0, fsync will be called at least once. Default to "
        "1.",
        "1");
    fbench.flag(
        "--file", "", "The name of the open file. e.g. --file /dev/pmem01");

// io uring
#ifdef YBENCH_USE_LIBURING
    auto &io_uring =
        parser.command("io_uring", "The benchmark of linux io_uring");
    io_uring.flag(
        "--size", "-S", "The total size for IO. e.g. --size 4M or --size 1G");
    io_uring.flag(
        "--io-size", "-s", "The size of IO unit. e.g. --size 4K or --size 64");
    io_uring.flag(
        "--file", "", "The name of the open file. e.g. --file /tmp/ybench");
#endif

#ifdef YBENCH_USE_AIO
    auto &aio_seq = parser.command(
        "aio-seq",
        "The performance of writing file sequentially with aio engine");
    aio_seq.flag("--threads", "-T", "The number of threads run simutaneously");
    aio_seq.flag("--time", "-t", "The duration (second)");
    aio_seq.flag("--size",
                 "-S",
                 "The size for each aio write. e.g. -S=4KB -S=1024 -S=1GB");
    aio_seq.flag(
        "--file", "", "The name of the open file. e.g. --file /dev/pmem01");
    auto &aio_inplace = parser.command(
        "aio-inplace",
        "The performance of writing file at the same position with aio engine");
    aio_inplace.flag(
        "--threads", "-T", "The number of threads run simutaneously");
    aio_inplace.flag("--time", "-t", "The duration (second)");
    aio_inplace.flag("--size",
                     "-S",
                     "The size for each aio write. e.g. -S=4KB -S=1024 -S=1GB");
    aio_inplace.flag(
        "--file", "", "The name of the open file. e.g. --file /dev/pmem01");
#endif
#ifdef YBENCH_USE_PMEM
    auto &pmem_sw = parser.command(
        "pm",
        "The performance of writing persistent memory sequentially with PMDK");
    pmem_sw.flag("--threads", "-T", "The number of threads run simutaneously");
    pmem_sw.flag("--size", "-S", "The size for each thread to write.");
    pmem_sw.flag("--io-size", "-s", "The IO size for each write request.");
    pmem_sw.flag(
        "--file", "", "The name of the open file. e.g. --file /dev/pmem01");
#endif
}