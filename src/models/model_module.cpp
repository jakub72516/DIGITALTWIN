#include "models/model_module.hpp"

#include <stdexcept>

namespace etwin::models {

ModelFactory& ModelFactory::instance() {
    static ModelFactory factory;
    return factory;
}

void ModelFactory::register_model(const std::string& name, Creator creator) {
    creators_[name] = std::move(creator);
}

std::unique_ptr<ModelModule> ModelFactory::create(const std::string& name) const {
    auto it = creators_.find(name);
    if (it == creators_.end()) {
        throw std::runtime_error("Unknown model: " + name);
    }
    return it->second();
}

std::vector<std::string> ModelFactory::available_models() const {
    std::vector<std::string> result;
    result.reserve(creators_.size());
    for (const auto& [name, c] : creators_) result.push_back(name);
    return result;
}

} // namespace etwin::models
