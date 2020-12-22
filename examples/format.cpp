#include "argparser/argparser.hpp"
void register_arg(argparser::Parser& parser)
{
    auto& atomic_add = parser.command("atomic-add", "The performance of std::atomic<uint64_t>::fetch_add(1)");
    atomic_add.flag("--threads", "-T", "The number of threads to run simutaneously");
    atomic_add.flag("--time", "-t", "The duration (second) to run");

    auto& cas = parser.command("cas", "The performance of compare_exchange_strong");
    cas.flag("--threads", "-T", "The number of threads");
    cas.flag("--time", "-t", "The duration (second) to run");

    auto& add = parser.command("add", "The performance of uint64_t++");
    add.flag("--time", "-t", "The duration (second) to run");

    auto& lock = parser.command("lock", "The performance of std::lock_guard<std::mutex>");
    lock.flag("--threads", "-T", "The number of threads");
    lock.flag("--time", "-t", "The duration (second) to run");

    auto& prefetch = parser.command("prefetch", "The effect of __builtin_prefetch(addr)");
    prefetch.flag("--ops", "", "The number of operations of random memory write");

    auto& cas_lock = parser.command("cas-lock", "The performance of lock implemented by CAS");
    cas_lock.flag("--threads", "-T", "The number of threads");
    cas_lock.flag("--time", "-t", "The duration (second) to run");

    auto& mem = parser.command("mem", "The performance of accessing memory randomly.");
    mem.flag("--time", "-t", "The duration (microsecond) to run");

    auto& malloc = parser.command("malloc", "The performance of running malloc");
    malloc.flag("--threads", "-T", "The number of threads");
    malloc.flag("--time", "-t", "The duration (millisecond) to run");
    malloc.flag("--size", "-S", "The size of each malloc");

    auto& timing = parser.command("timing", "The performance of std::chrono::steady_clock::now() and std::chrono::duration_cast<>()");
    timing.flag("--time", "-t", "The duration (second) to run");
#ifdef YBENCH_USE_AIO
    auto& aio_seq = parser.command("aio-seq", "The performance of writing file sequentially with aio engine");
    aio_seq.flag("--threads", "-T", "The number of threads run simutaneously");
    aio_seq.flag("--time", "-t", "The duration (second)");
    aio_seq.flag("--size", "-S", "The size for each aio write. e.g. -S=4KB -S=1024 -S=1GB");
    aio_seq.flag("--file", "", "The name of the open file. e.g. --file /dev/pmem01");
    auto& aio_inplace = parser.command("aio-inplace", "The performance of writing file at the same position with aio engine");
    aio_inplace.flag("--threads", "-T", "The number of threads run simutaneously");
    aio_inplace.flag("--time", "-t", "The duration (second)");
    aio_inplace.flag("--size", "-S", "The size for each aio write. e.g. -S=4KB -S=1024 -S=1GB");
    aio_inplace.flag("--file", "", "The name of the open file. e.g. --file /dev/pmem01");
#endif
#ifdef YBENCH_USE_PMEM
    auto& pmem_sw = parser.command("pmem-sw", "The performance of writing persistent memory sequentially with PMDK");
    pmem_sw.flag("--threads", "-T", "The number of threads run simutaneously");
    pmem_sw.flag("--size", "-S", "The size for each thread to write.");
    pmem_sw.flag("--io-size", "-s", "The IO size for each write request.");
    pmem_sw.flag("--file", "", "The name of the open file. e.g. --file /dev/pmem01");
#endif
}
int main(int argc, const char** argv) {
    auto& parser = argparser::init("");
    register_arg(parser);
    parser.parse(argc, argv);
    parser.print_promt(argc, argv);
}