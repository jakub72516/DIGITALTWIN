#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <fstream>

#include "core/time.hpp"

namespace etwin::security {

enum class AuditEventType {
    DataAccess,
    ModelRun,
    ConfigChange,
    AccessDenied,
    Authentication,
    IntegrityCheck,
    SystemEvent,
};

struct AuditEvent {
    AuditEventType type;
    std::string actor;
    std::string resource;
    std::string action;
    std::string details;
    etwin::core::Instant timestamp;
    bool success;
};

class AuditLog {
public:
    explicit AuditLog(std::string path);
    AuditLog();

    void log(const AuditEvent& event);
    void log(AuditEventType type, const std::string& actor,
             const std::string& resource, const std::string& action,
             bool success = true);

    std::vector<AuditEvent> query(const std::string& actor,
                                  const etwin::core::TimeRange& range) const;

    std::vector<AuditEvent> all() const;

    void flush();

private:
    std::string path_;
    std::vector<AuditEvent> events_;
    mutable std::mutex mutex_;
};

} // namespace etwin::security
