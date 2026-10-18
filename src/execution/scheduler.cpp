#include "execution/scheduler.hpp"

#include <algorithm>
#include <stdexcept>

namespace etwin::execution {

void TaskScheduler::add_task(const Task& task) {
    tasks_.push_back(task);
    task_index_[task.id] = tasks_.size() - 1;
}

void TaskScheduler::add_task(Task&& task) {
    task_index_[task.id] = tasks_.size();
    tasks_.push_back(std::move(task));
}

bool TaskScheduler::run_sequential() {
    std::vector<bool> done(tasks_.size(), false);
    size_t completed_count = 0;

    while (completed_count < tasks_.size()) {
        bool progressed = false;
        for (size_t i = 0; i < tasks_.size(); ++i) {
            if (done[i]) continue;

            bool deps_met = true;
            for (const auto& dep : tasks_[i].dependencies) {
                auto it = task_index_.find(dep);
                if (it == task_index_.end()) {
                    tasks_[i].failed = true;
                    tasks_[i].error = "Unknown dependency: " + dep;
                    done[i] = true;
                    completed_count++;
                    deps_met = false;
                    progressed = true;
                    break;
                }
                if (!done[it->second]) {
                    deps_met = false;
                    break;
                }
                if (tasks_[it->second].failed) {
                    tasks_[i].failed = true;
                    tasks_[i].error = "Dependency failed: " + dep;
                    done[i] = true;
                    completed_count++;
                    deps_met = false;
                    progressed = true;
                    break;
                }
            }

            if (!deps_met) continue;

            try {
                tasks_[i].work();
                tasks_[i].completed = true;
            } catch (const std::exception& e) {
                tasks_[i].failed = true;
                tasks_[i].error = e.what();
            } catch (...) {
                tasks_[i].failed = true;
                tasks_[i].error = "Unknown error";
            }
            done[i] = true;
            completed_count++;
            progressed = true;
        }

        if (!progressed && completed_count < tasks_.size()) {
            for (size_t i = 0; i < tasks_.size(); ++i) {
                if (!done[i]) {
                    tasks_[i].failed = true;
                    tasks_[i].error = "Circular dependency or deadlock";
                    done[i] = true;
                    completed_count++;
                }
            }
        }
    }

    return !has_failures();
}

bool TaskScheduler::run_parallel(size_t max_workers) {
    (void)max_workers;
    return run_sequential();
}

std::vector<std::string> TaskScheduler::completed_tasks() const {
    std::vector<std::string> result;
    for (const auto& t : tasks_) {
        if (t.completed) result.push_back(t.id);
    }
    return result;
}

std::vector<std::string> TaskScheduler::failed_tasks() const {
    std::vector<std::string> result;
    for (const auto& t : tasks_) {
        if (t.failed) result.push_back(t.id);
    }
    return result;
}

bool TaskScheduler::has_failures() const {
    for (const auto& t : tasks_) {
        if (t.failed) return true;
    }
    return false;
}

} // namespace etwin::execution
