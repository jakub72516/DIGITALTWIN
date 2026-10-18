#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>

#include "core/time.hpp"
#include "core/provenance.hpp"

#include "state/earth_state.hpp"

namespace etwin::state {

class StorageBackend {
public:
    virtual ~StorageBackend() = default;

    virtual bool store(const EarthState& state, const std::string& key) = 0;
    virtual std::optional<EarthState> load(const std::string& key) = 0;
    virtual bool exists(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual std::vector<std::string> list() const = 0;
};

class MemoryStorage : public StorageBackend {
public:
    bool store(const EarthState& state, const std::string& key) override;
    std::optional<EarthState> load(const std::string& key) override;
    bool exists(const std::string& key) const override;
    bool remove(const std::string& key) override;
    std::vector<std::string> list() const override;

private:
    std::map<std::string, EarthState> store_;
};

class FileStorage : public StorageBackend {
public:
    explicit FileStorage(std::string directory);
    ~FileStorage() override;

    bool store(const EarthState& state, const std::string& key) override;
    std::optional<EarthState> load(const std::string& key) override;
    bool exists(const std::string& key) const override;
    bool remove(const std::string& key) override;
    std::vector<std::string> list() const override;

private:
    std::string directory_;
    std::string path_for(const std::string& key) const;
};

class ContentAddressedStore {
public:
    etwin::core::Hash put(const EarthState& state);
    std::optional<EarthState> get(const etwin::core::Hash& hash) const;
    bool contains(const etwin::core::Hash& hash) const;

private:
    std::map<etwin::core::Hash, EarthState> store_;
};

} // namespace etwin::state
