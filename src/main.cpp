#include <iostream>
#include <filesystem> // For std::filesystem
#include <vector>
#include <string>

#include "include/cli.hpp"

using namespace cppio;

namespace fs = std::filesystem;
namespace po = boost::program_options;

po::options_description globalOptions("Global Options");
globalOptions.add_options()
    ("config-dir,C", po::value< string >(), "[DEPRECATED] path to legacy configuration directory")
    ("certs-dir,S", po::value< string >(), "path to certs directory")
    ("quiet", "disable startup and info messages")
    ("anonymous", "hide sensitive information from logging");

// Help template for cppio.
auto cppioHelpTemplate = R("NAME:
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
");

extern cli::Command serverCmd;

std::shared_ptr<cli::App> newAPP(std::string& appName) {
    auto app = std::make_shared<cli::App>(appName);
    app->author = "CppIO";
    app->version = "0.1";
    app->usage = "High Performance Object Storage";
    app->description = "Build high performance data infrastructure for machine learning, analytics and application data workloads with CppIO";
    app.options = globalOptions;
    app->hideHelpCommand = true;

    app->commands.push_back(serverCmd);
    app->customAppHelpTemplate = cppioHelpTemplate;
    return app;
}

int main(int argc, char* argv[]) {
    if (argc == 0) {
        std::cerr << "Error: No arguments provided." << std::endl;
        return 1;
    }

    std::string appName = fs::path(argv).filename();
    std::cout << "CppIO app name: " << appName << std::endl;

    if (newApp(appName)->run(argc, argv) != nullptr) {
        return 1;
    }
}