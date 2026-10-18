#include "security/access_control.hpp"

namespace etwin::security {

void AccessControl::grant(const Principal& principal, const Permission& permission) {
    grants_[principal.id].insert(permission.resource + ":" + permission.action);
}

void AccessControl::revoke(const Principal& principal, const Permission& permission) {
    auto it = grants_.find(principal.id);
    if (it != grants_.end()) {
        it->second.erase(permission.resource + ":" + permission.action);
    }
}

AccessDecision AccessControl::check(const Principal& principal, const Permission& permission) const {
    for (const auto& role : principal.roles) {
        if (role == Role::Admin) {
            return AccessDecision{true, "admin role"};
        }
    }

    auto it = grants_.find(principal.id);
    if (it == grants_.end()) {
        return AccessDecision{false, "no grants for principal"};
    }

    std::string key = permission.resource + ":" + permission.action;
    if (it->second.contains(key)) {
        return AccessDecision{true, "granted"};
    }
    return AccessDecision{false, "not granted"};
}

void AccessControl::set_classification(const std::string& resource, DataClassification classification) {
    classifications_[resource] = classification;
}

DataClassification AccessControl::classification_of(const std::string& resource) const {
    auto it = classifications_.find(resource);
    if (it == classifications_.end()) return DataClassification::Public;
    return it->second;
}

bool AccessControl::requires_clearance(DataClassification classification, const Principal& principal) const {
    switch (classification) {
        case DataClassification::Public:
            return true;
        case DataClassification::Internal:
            for (const auto& r : principal.roles) {
                if (r != Role::Anonymous) return true;
            }
            return false;
        case DataClassification::Sensitive:
            for (const auto& r : principal.roles) {
                if (r == Role::Scientist || r == Role::Operator || r == Role::Admin) return true;
            }
            return false;
        case DataClassification::Restricted:
            for (const auto& r : principal.roles) {
                if (r == Role::Admin) return true;
            }
            return false;
    }
    return false;
}

} // namespace etwin::security
