#include <vector>
#include <string>
#include <boost/program_options.hpp>

#include "include/cli.hpp"

using namespace cppio;

void serverMain(cli::Context* ctx);

std::string config;
std::string address;

boost::program_options::options_description serverCmdOptions("Options");
serverCmdOptions.add_options()
    ("config",
        po::value<std::string>(&config)->default_value(""),
        "specify server configuration via YAML configuration")
    ("address",
        po::value<std::string>(&address)->default_value(":9900"),
        "bind to a specific ADDRESS:PORT, ADDRESS can be an IP or hostname");

cli::Command serverCmd {
    .name = "server",
    .usage = "start object storage server",
    .options = serverCmdOptions,
    .action = ActionFunc(serverMain),
    .customHelpTemplate = "NAME:  {{.HelpName}} - {{.Usage}}",
};

Error serverMain(cli::Context* ctx) {
    Error err = nullptr;

    boost::program_options::variables_map optionSet;
    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(ctx->argc, ctx->argv, serverCmdOptions),
            optionSet);
        boost::program_options::notify(optionSet);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return newError(1, e.what());
    }

    std::cout << "Config: " << config << std::endl;
    std::cout << "Address: " << address << std::endl;

    return err;
}