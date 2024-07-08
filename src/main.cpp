#include <iostream>
#include <filesystem> // For std::filesystem
#include <vector>
#include <string>
#include "include/cli.hpp"
#include <boost/program_options.hpp>
#include "include/log.hpp"

using namespace cppio;

namespace fs = std::filesystem;
namespace po = boost::program_options;

std::shared_ptr<spdlog::logger> gLogger;

// Help template for cppio.
auto cppioHelpTemplate = R"(NAME:
  {{.Name}} - {{.Usage}}

DESCRIPTION:
  {{.Description}}

USAGE:
  {{.HelpName}} {{if .VisibleFlags}}[FLAGS] {{end}}COMMAND{{if .VisibleFlags}}{{end}} [ARGS...]

COMMANDS:
  {{range .VisibleCommands}}{{join .Names ", "}}{{ "\t" }}{{.Usage}}
  {{end}}{{if .VisibleFlags}}
FLAGS:
  {{range .VisibleFlags}}{{.}}
  {{end}}{{end}}
VERSION:
  {{.Version}}
)";

extern cli::Command serverCmd;

void init_logging() {
    // Create a file logger
    gLogger = spdlog::basic_logger_mt("file_logger", "logs/cppio.log");
    // Set logging pattern (optional)
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
}

std::shared_ptr<cli::App> newApp(std::string& appName) {
    auto globalOptions = std::make_shared<boost::program_options::options_description>("Global Options");
    globalOptions->add_options()
      ("config-dir,C", po::value< std::string >(), "[DEPRECATED] path to legacy configuration directory")
      ("certs-dir,S", po::value< std::string >(), "path to certs directory")
      ("quiet", "disable startup and info messages")
      ("anonymous", "hide sensitive information from logging");
    
    auto app = std::make_shared<cli::App>(appName);
    app->author = "CppIO";
    app->version = "0.1";
    app->usage = "High Performance Object Storage";
    app->description = "Build high performance data infrastructure for machine learning, analytics and application data workloads with CppIO";
    app->flags = { .optionsDescription = globalOptions },
    app->hideHelpCommand = true;
    app->commands.push_back(&serverCmd);
    app->customAppHelpTemplate = cppioHelpTemplate;
    return app;
}

int main(int argc, char* argv[]) {

    if (argc == 0) {
        std::cerr << "Error: No arguments provided." << std::endl;
        return 1;
    }

    init_logging();

    std::string appName = fs::path(argv[0]).filename();
    gLogger->info("CppIO app name: {}\n", appName);

    if (newApp(appName)->run(argc, argv) != nullptr) {
        return 1;
    }
}