#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

#include "core/quality.hpp"

namespace etwin::security {

class InputValidator {
public:
    void add_range_check(const std::string& field, double min, double max);
    void add_required(const std::string& field);
    void add_allowed_value(const std::string& field, const std::string& value);

    etwin::core::quality::ValidationReport validate(
        const std::map<std::string, double>& numeric_fields,
        const std::map<std::string, std::string>& string_fields) const;

    static bool is_safe_string(const std::string& s);
    static bool is_safe_path(const std::string& s);

private:
    std::map<std::string, std::pair<double, double>> ranges_;
    std::vector<std::string> required_;
    std::map<std::string, std::vector<std::string>> allowed_values_;
};

} // namespace etwin::security
