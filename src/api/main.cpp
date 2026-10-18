#include "api/cli.hpp"

int main(int argc, char* argv[]) {
    etwin::api::Cli cli;
    return cli.run(argc, argv);
}
