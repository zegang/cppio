// Copyright 2025 cppio authors. All rights reserved.

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

#include "all_cmds.h"
#include "log.h"

using namespace CPPIO_NAMESPACE;

// Help template for CPPIO_NAMESPACE.
const auto kCppioHelpTemplate = R"(NAME:
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

int main(int argc, char* argv[]) {
    if (argc == 0) {
        std::cerr << "Error: No arguments provided." << std::endl;
        return 1;
    }

    std::string app_name = std::filesystem::path(argv[0]).filename();
    CPPIOLOG::info("CppIO app name: {}", app_name);

    auto cppio_app = cli::NewApp(app_name);
    cppio_app->add_command(&CPPIO_NAMESPACE::kServerCmd);
    cppio_app->set_custom_app_help_template(kCppioHelpTemplate);

    if (cppio_app->Run(argc, argv) != ErrorOK) {
        return 1;
    }
}