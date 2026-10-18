#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>

namespace etwin::execution {

struct ResourceSnapshot {
    double cpu_usage_percent;
    size_t memory_used_bytes;
    size_t memory_total_bytes;
    double io_read_bytes;
    double io_write_bytes;
    size_t active_threads;
    std::chrono::steady_clock::time_point timestamp;
};

struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    std::chrono::steady_clock::time_point timestamp;
};

class ResourceMonitor {
public:
    ResourceMonitor();

    ResourceSnapshot snapshot() const;
    void start_periodic_monitoring(std::chrono::milliseconds interval);
    void stop_monitoring();

    void record_metric(const PerformanceMetric& metric);
    const std::vector<PerformanceMetric>& metrics() const { return metrics_; }

    std::vector<ResourceSnapshot> history() const;

private:
    mutable std::mutex mutex_;
    std::vector<PerformanceMetric> metrics_;
    std::vector<ResourceSnapshot> history_;
    bool monitoring_;
    std::thread monitor_thread_;
};

} // namespace etwin::execution
