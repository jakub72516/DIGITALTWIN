#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace etwin::api {

struct CliCommand {
    std::string name;
    std::string description;
    std::vector<std::string> arguments;
};

class Cli {
public:
    Cli();

    int run(int argc, char* argv[]);

    void register_command(const CliCommand& command);
    const std::vector<CliCommand>& commands() const { return commands_; }

    int run_command(const std::string& name, const std::vector<std::string>& args);

    static void print_help();
    static void print_version();

private:
    std::vector<CliCommand> commands_;

    int cmd_run(const std::vector<std::string>& args);
    int cmd_query(const std::vector<std::string>& args);
    int cmd_validate(const std::vector<std::string>& args);
    int cmd_reproduce(const std::vector<std::string>& args);
    int cmd_catalog(const std::vector<std::string>& args);
    int cmd_checkpoint(const std::vector<std::string>& args);
};

} // namespace etwin::api
