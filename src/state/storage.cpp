#include "state/storage.hpp"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace etwin::state {

bool MemoryStorage::store(const EarthState& state, const std::string& key) {
    store_[key] = state;
    return true;
}

std::optional<EarthState> MemoryStorage::load(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) return std::nullopt;
    return it->second;
}

bool MemoryStorage::exists(const std::string& key) const {
    return store_.contains(key);
}

bool MemoryStorage::remove(const std::string& key) {
    return store_.erase(key) > 0;
}

std::vector<std::string> MemoryStorage::list() const {
    std::vector<std::string> result;
    for (const auto& [k, v] : store_) result.push_back(k);
    return result;
}

FileStorage::FileStorage(std::string directory) : directory_(std::move(directory)) {
    std::filesystem::create_directories(directory_);
}

FileStorage::~FileStorage() = default;

std::string FileStorage::path_for(const std::string& key) const {
    return (std::filesystem::path(directory_) / (key + ".json")).string();
}

bool FileStorage::store(const EarthState& state, const std::string& key) {
    nlohmann::json j;
    j["version"] = state.version();
    std::ofstream f(path_for(key));
    if (!f) return false;
    f << j.dump(2);
    return true;
}

std::optional<EarthState> FileStorage::load(const std::string& key) {
    std::ifstream f(path_for(key));
    if (!f) return std::nullopt;
    nlohmann::json j;
    f >> j;
    return EarthState(j.value("version", "0"));
}

bool FileStorage::exists(const std::string& key) const {
    return std::filesystem::exists(path_for(key));
}

bool FileStorage::remove(const std::string& key) {
    return std::filesystem::remove(path_for(key));
}

std::vector<std::string> FileStorage::list() const {
    std::vector<std::string> result;
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        if (entry.path().extension() == ".json") {
            result.push_back(entry.path().stem().string());
        }
    }
    return result;
}

etwin::core::Hash ContentAddressedStore::put(const EarthState& state) {
    etwin::core::Hash hash = state.compute_hash();
    store_[hash] = state;
    return hash;
}

std::optional<EarthState> ContentAddressedStore::get(const etwin::core::Hash& hash) const {
    auto it = store_.find(hash);
    if (it == store_.end()) return std::nullopt;
    return it->second;
}

bool ContentAddressedStore::contains(const etwin::core::Hash& hash) const {
    return store_.contains(hash);
}

} // namespace etwin::state
