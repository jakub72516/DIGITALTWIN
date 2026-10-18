#include "execution/resource_monitor.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace etwin::execution {

ResourceMonitor::ResourceMonitor() : monitoring_(false) {}

ResourceSnapshot ResourceMonitor::snapshot() const {
    ResourceSnapshot snap;
    snap.cpu_usage_percent = 0.0;
    snap.memory_used_bytes = 0;
    snap.memory_total_bytes = 0;
    snap.io_read_bytes = 0.0;
    snap.io_write_bytes = 0.0;
    snap.active_threads = 0;
    snap.timestamp = std::chrono::steady_clock::now();

#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        snap.memory_used_bytes = pmc.WorkingSetSize;
    }
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        snap.memory_total_bytes = mem.ullTotalPhys;
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        snap.memory_used_bytes = usage.ru_maxrss * 1024;
        snap.io_read_bytes = static_cast<double>(usage.ru_inblock);
        snap.io_write_bytes = static_cast<double>(usage.ru_oublock);
    }
#endif

    return snap;
}

void ResourceMonitor::start_periodic_monitoring(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (monitoring_) return;
    monitoring_ = true;
    monitor_thread_ = std::thread([this, interval]() {
        while (monitoring_) {
            ResourceSnapshot snap = snapshot();
            {
                std::lock_guard<std::mutex> lk(mutex_);
                history_.push_back(snap);
            }
            std::this_thread::sleep_for(interval);
        }
    });
}

void ResourceMonitor::stop_monitoring() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        monitoring_ = false;
    }
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
}

void ResourceMonitor::record_metric(const PerformanceMetric& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.push_back(metric);
}

std::vector<ResourceSnapshot> ResourceMonitor::history() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return history_;
}

} // namespace etwin::execution
