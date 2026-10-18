#include "api/cli.hpp"

#include <iostream>
#include <algorithm>

namespace etwin::api {

Cli::Cli() {
    register_command({"run", "Run a scenario", {"<scenario_config.json>"}});
    register_command({"query", "Query earth state", {"<field>", "<time>"}});
    register_command({"validate", "Validate data or model", {"<target>"}});
    register_command({"reproduce", "Reproduce a previous run", {"<run_manifest.json>"}});
    register_command({"catalog", "List or query data catalog", {"[filter]"}});
    register_command({"checkpoint", "List or restore checkpoints", {"<action>"}});
}

void Cli::register_command(const CliCommand& command) {
    commands_.push_back(command);
}

int Cli::run(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    std::string cmd = argv[1];
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) args.push_back(argv[i]);

    if (cmd == "help" || cmd == "--help" || cmd == "-h") {
        print_help();
        return 0;
    }
    if (cmd == "version" || cmd == "--version") {
        print_version();
        return 0;
    }

    return run_command(cmd, args);
}

int Cli::run_command(const std::string& name, const std::vector<std::string>& args) {
    if (name == "run") return cmd_run(args);
    if (name == "query") return cmd_query(args);
    if (name == "validate") return cmd_validate(args);
    if (name == "reproduce") return cmd_reproduce(args);
    if (name == "catalog") return cmd_catalog(args);
    if (name == "checkpoint") return cmd_checkpoint(args);

    std::cerr << "Unknown command: " << name << "\n";
    print_help();
    return 1;
}

int Cli::cmd_run(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: etwin run <scenario_config.json>\n";
        return 1;
    }
    std::cout << "Running scenario from " << args[0] << "\n";
    return 0;
}

int Cli::cmd_query(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: etwin query <field> <time>\n";
        return 1;
    }
    std::cout << "Querying field '" << args[0] << "' at time " << args[1] << "\n";
    return 0;
}

int Cli::cmd_validate(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: etwin validate <target>\n";
        return 1;
    }
    std::cout << "Validating " << args[0] << "\n";
    return 0;
}

int Cli::cmd_reproduce(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: etwin reproduce <run_manifest.json>\n";
        return 1;
    }
    std::cout << "Reproducing run from manifest " << args[0] << "\n";
    return 0;
}

int Cli::cmd_catalog(const std::vector<std::string>& args) {
    std::cout << "Data catalog:\n";
    if (args.empty()) {
        std::cout << "  (list all datasets)\n";
    } else {
        std::cout << "  (filter by '" << args[0] << "')\n";
    }
    return 0;
}

int Cli::cmd_checkpoint(const std::vector<std::string>& args) {
    std::cout << "Checkpoint management\n";
    return 0;
}

void Cli::print_help() {
    std::cout << "Earth Systems Digital Twin CLI\n";
    std::cout << "Usage: etwin <command> [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  run <config.json>      Run a scenario\n";
    std::cout << "  query <field> <time>   Query earth state\n";
    std::cout << "  validate <target>      Validate data or model\n";
    std::cout << "  reproduce <manifest>   Reproduce a previous run\n";
    std::cout << "  catalog [filter]       List or query data catalog\n";
    std::cout << "  checkpoint <action>    List or restore checkpoints\n";
    std::cout << "  version                Print version\n";
}

void Cli::print_version() {
    std::cout << "Earth Systems Digital Twin v0.1.0\n";
}

} // namespace etwin::api
