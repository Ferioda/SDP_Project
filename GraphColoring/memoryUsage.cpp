#include <windows.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <processthreadsapi.h>
#include <Psapi.h>
#include "memoryUsage.h"


# define MEGA 1024 * 1024

void MemoryMonitor::thread_function(MemoryMonitor& monitor) {
    do {
        double mem_usage = MemoryMonitor::current_mem_usage();
        if (mem_usage > monitor.peak_usage)
            monitor.peak_usage = mem_usage;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    } while (!monitor.stopped);
}

// https://stackoverflow.com/a/42925322
MemoryMonitor::MemoryMonitor() : stopped(false), baseline(current_mem_usage()), peak_usage(0), thread(thread_function, std::ref(*this)) {

}

double MemoryMonitor::current_mem_usage() {
    PROCESS_MEMORY_COUNTERS memCounter;
    BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
        &memCounter,
        sizeof(memCounter));
    double mem_usage = static_cast<double>(memCounter.WorkingSetSize) / static_cast<double>(MEGA);  // return mem_usage in MB
    return mem_usage;
}

void MemoryMonitor::stop() {
    stopped = true;
    thread.join();
}

double MemoryMonitor::total() const { return peak_usage; }

double MemoryMonitor::delta() const { return peak_usage - baseline; }