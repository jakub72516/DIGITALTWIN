#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

#include "core/time.hpp"

namespace etwin::security {

enum class Role {
    Admin,
    Scientist,
    Operator,
    Viewer,
    Anonymous,
};

struct Principal {
    std::string id;
    std::vector<Role> roles;
    std::string organization;
};

struct Permission {
    std::string resource;
    std::string action;
};

enum class DataClassification {
    Public,
    Internal,
    Sensitive,
    Restricted,
};

struct AccessDecision {
    bool allowed;
    std::string reason;
};

class AccessControl {
public:
    void grant(const Principal& principal, const Permission& permission);
    void revoke(const Principal& principal, const Permission& permission);

    AccessDecision check(const Principal& principal, const Permission& permission) const;

    void set_classification(const std::string& resource, DataClassification classification);
    DataClassification classification_of(const std::string& resource) const;

    bool requires_clearance(DataClassification classification, const Principal& principal) const;

private:
    std::map<std::string, std::set<std::string>> grants_;
    std::map<std::string, DataClassification> classifications_;
};

} // namespace etwin::security
