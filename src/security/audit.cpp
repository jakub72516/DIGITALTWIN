#include "security/audit.hpp"

#include <algorithm>

namespace etwin::security {

AuditLog::AuditLog(std::string path) : path_(std::move(path)) {}
AuditLog::AuditLog() : path_("") {}

void AuditLog::log(const AuditEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    events_.push_back(event);
}

void AuditLog::log(AuditEventType type, const std::string& actor,
                   const std::string& resource, const std::string& action,
                   bool success) {
    AuditEvent event;
    event.type = type;
    event.actor = actor;
    event.resource = resource;
    event.action = action;
    event.timestamp = etwin::core::Instant::now();
    event.success = success;
    log(event);
}

std::vector<AuditEvent> AuditLog::query(const std::string& actor,
                                        const etwin::core::TimeRange& range) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AuditEvent> result;
    for (const auto& e : events_) {
        if (e.actor == actor && e.timestamp >= range.start() && e.timestamp < range.end()) {
            result.push_back(e);
        }
    }
    return result;
}

std::vector<AuditEvent> AuditLog::all() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return events_;
}

void AuditLog::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (path_.empty()) return;
    std::ofstream f(path_, std::ios::app);
    for (const auto& e : events_) {
        f << static_cast<int>(e.type) << " " << e.actor << " " << e.resource
          << " " << e.action << " " << e.timestamp.to_iso8601() << "\n";
    }
    events_.clear();
}

} // namespace etwin::security
