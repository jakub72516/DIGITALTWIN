#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <future>
#include <thread>
#include <mutex>

namespace etwin::execution {

struct Task {
    std::string id;
    std::vector<std::string> dependencies;
    std::function<void()> work;
    bool completed;
    bool failed;
    std::string error;
};

class TaskScheduler {
public:
    void add_task(const Task& task);
    void add_task(Task&& task);

    bool run_sequential();
    bool run_parallel(size_t max_workers);

    const std::vector<Task>& tasks() const { return tasks_; }
    std::vector<std::string> completed_tasks() const;
    std::vector<std::string> failed_tasks() const;

    bool has_failures() const;

private:
    std::vector<Task> tasks_;
    std::map<std::string, size_t> task_index_;
    std::mutex mutex_;
};

} // namespace etwin::execution
