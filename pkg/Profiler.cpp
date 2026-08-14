#include "Profiler.hpp"

#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <new>
#include <string>

#if defined(__APPLE__)
    #include <malloc/malloc.h>
    #include <sys/resource.h>
    #include <mach/mach.h>
#elif defined(__linux__)
    #include <malloc.h>
    #include <fstream>
    #include <sstream>
#endif

namespace {

std::atomic<long long> g_totalAllocatedBytes{0};
std::atomic<long long> g_currentAllocatedBytes{0};
std::atomic<long long> g_allocationCount{0};

inline std::size_t usableSize(void* ptr) {
#if defined(__APPLE__)
    return malloc_size(ptr);
#elif defined(__linux__)
    return malloc_usable_size(ptr);
#else
    return 0;
#endif
}

}  // namespace

void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();

    std::size_t usable = usableSize(ptr);

    g_totalAllocatedBytes.fetch_add(static_cast<long long>(usable), std::memory_order_relaxed);
    g_currentAllocatedBytes.fetch_add(static_cast<long long>(usable), std::memory_order_relaxed);
    g_allocationCount.fetch_add(1, std::memory_order_relaxed);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (!ptr) return;
    std::size_t size = usableSize(ptr);
    g_currentAllocatedBytes.fetch_sub(static_cast<long long>(size), std::memory_order_relaxed);
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t /*size*/) noexcept { operator delete(ptr); }
void* operator new[](std::size_t size) { return operator new(size); }
void operator delete[](void* ptr) noexcept { operator delete(ptr); }
void operator delete[](void* ptr, std::size_t size) noexcept { operator delete(ptr, size); }

namespace pkg {

namespace {

std::atomic<bool> g_traceBuild{true};

std::chrono::steady_clock::time_point g_startTime;
long long g_totalAllocAtStart = 0;
int g_threadsAtStart = 0;

#if defined(__linux__)
long long readStatusKb(const char* field) {
    std::ifstream status("/proc/self/status");
    std::string line;
    std::size_t fieldLen = std::string(field).size();
    while (std::getline(status, line)) {
        if (line.compare(0, fieldLen, field) == 0) {
            std::istringstream iss(line.substr(fieldLen));
            long long kb = 0;
            iss >> kb;
            return kb;
        }
    }
    return 0;
}

int readThreadCount() {
    return static_cast<int>(readStatusKb("Threads:"));
}
#endif

long long peakRssKb() {
#if defined(__APPLE__)
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return static_cast<long long>(ru.ru_maxrss) / 1024;
#elif defined(__linux__)
    return readStatusKb("VmHWM:");
#else
    return 0;
#endif
}

int threadCount() {
#if defined(__APPLE__)
    thread_act_array_t threadList;
    mach_msg_type_number_t threadCount = 0;
    kern_return_t kr = task_threads(mach_task_self(), &threadList, &threadCount);
    if (kr != KERN_SUCCESS) {
        return 0;
    }
    for (mach_msg_type_number_t i = 0; i < threadCount; ++i) {
        mach_port_deallocate(mach_task_self(), threadList[i]);
    }
    vm_deallocate(mach_task_self(), reinterpret_cast<vm_address_t>(threadList),
                  threadCount * sizeof(thread_act_t));
    return static_cast<int>(threadCount);
#elif defined(__linux__)
    return readThreadCount();
#else
    return 0;
#endif
}

double mb(long long bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0);
}

const char* threadLeakStatus(int before, int after) {
    return (after > before) ? "possible thread leak" : "OK";
}

}  // namespace

void StartProfiling() {
    g_totalAllocAtStart = g_totalAllocatedBytes.load(std::memory_order_relaxed);
    g_threadsAtStart = threadCount();
    g_startTime = std::chrono::steady_clock::now();
}

void StopProfiling() {
    auto elapsed = std::chrono::steady_clock::now() - g_startTime;
    double elapsedMs = std::chrono::duration<double, std::milli>(elapsed).count();

    long long totalAllocSinceStart =
        g_totalAllocatedBytes.load(std::memory_order_relaxed) - g_totalAllocAtStart;
    long long currentHeap = g_currentAllocatedBytes.load(std::memory_order_relaxed);
    long long peakRss = peakRssKb();
    long long allocations = g_allocationCount.load(std::memory_order_relaxed);
    int threadsNow = threadCount();

    std::fprintf(stderr, "──────────────────────────────────────────\n");
    std::fprintf(stderr, "runtime:          %.3fms\n", elapsedMs);
    std::fprintf(stderr, "total allocated:  %.3f MB\n", mb(totalAllocSinceStart));
    std::fprintf(stderr, "heap alloc (now): %.3f MB\n", mb(currentHeap));
    std::fprintf(stderr, "heap sys (peak):  %.3f MB\n", mb(peakRss * 1024));
    std::fprintf(stderr, "allocations:      %lld  (no GC in C++ — shown instead of GC cycles)\n",
                 allocations);
    std::fprintf(stderr, "threads:          %d → %d  %s\n", g_threadsAtStart, threadsNow,
                 threadLeakStatus(g_threadsAtStart, threadsNow));
    std::fprintf(stderr, "──────────────────────────────────────────\n");
}

void Debug(const char* format, ...) {
    if constexpr (!kDebugEnabled) {
        return;
    }
    std::fprintf(stderr, "\n[debug] ");
    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
    std::fprintf(stderr, "\n");
}

void SetTrace(bool on) {
    g_traceBuild.store(on, std::memory_order_relaxed);
}

bool Trace() {
    return g_traceBuild.load(std::memory_order_relaxed);
}

}  // namespace pkg