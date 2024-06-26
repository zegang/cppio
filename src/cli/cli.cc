// Copyright 2025 cppio authors. All rights reserved.

#include <iostream>
#include <algorithm>
#include <iomanip>

#include "cli.h"

namespace CPPIO_NAMESPACE {
namespace cli {

/// @brief Default Help Command in all Apps
CommandDef kHelpCommandDef {
    .name = "help",
    .aliases = std::vector<std::string>(1, "h"),
    .usage = "Shows a list of commands or help for one command",
    .usage_text = "[command]",
    .action = [](std::shared_ptr<Context> ctx) {
        // auto args = ctx.args();
        // if (args.present()) {
        //     return showCommandHelp(ctx, args.first());
        // }
        // showAppHelp(ctx);
        kHelpCommandDef.flags.Parse(ctx->argc(), ctx->argv());

        if (kHelpCommandDef.flags.Args().empty()) {
            std::cout << kHelpCommandDef.usage << std::endl;
            for (auto& c : ctx->app()->commands()) {
                // std::cout << "- " << std::setw(ctx->app()->max_cmd_name_len() + 10)
                //     << std::left << c->name() << c->usage() << std::endl; 
            }
            return nullptr;
        }

        auto cmd = ctx->app()->command(kHelpCommandDef.flags.Args()[0]);
        if (cmd != nullptr) {
            // std::cout << std::setw(ctx->app()->max_cmd_name_len() + 10) << std::left
            //     << cmd->name() << cmd->usage() << std::endl;
        }

        return nullptr;
    },
    .flags = { "Command Help Options",
                [] (std::shared_ptr<boost::program_options::options_description> opt_des) {
                    opt_des->add_options()
                    ("help,h", "more helps")
                    ("positional", boost::program_options::value< std::vector<std::string> >());
                }
    }
};

Command kHelpCommand(kHelpCommandDef);

Error Command::Run(std::shared_ptr<Context> ctx) {
    if (!subcommands_.empty()) {
        return StartApp(ctx);
    }

    if (!action()) {
        command_def_.action = kHelpCommand.action();
    }

    return command_def_.action(ctx);
}

Error Command::StartApp(std::shared_ptr<Context> ctx) {
    return nullptr;
}

std::vector<std::string> Command::NamesWithHiddenAliases() {
    std::vector<std::string> names;
    names.push_back(command_def_.name);
    if (!command_def_.short_name.empty()) {
        names.push_back(command_def_.short_name);
    }
    names.insert(names.end(), command_def_.aliases.begin(), command_def_.aliases.end());
    return names;
}

// has_name returns true if Command.Name or Command.ShortName matches given name
bool Command::has_name(const std::string& name) {
    auto names = NamesWithHiddenAliases();
    auto it = std::find(names.begin(), names.end(), name);
    return it != names.end();
}

Error App::add_command(Command *command) {
    commands_.push_back(command);
    max_cmd_name_len_ = std::max(max_cmd_name_len_, command->name().size());
    return nullptr;
}

// Command returns the named command on App. Returns nil if the command does not exist
Command* App::command(const std::string& name) {
    for (auto& c : commands_) {
        if (c->has_name(name)) {
            return c;
        }
    }

    return nullptr;
}

void App::Setup() {
    if (did_setup_) {
        return;
    }

    did_setup_ = true;
    if (!author_.empty() || !email_.empty()) {
        authors_.emplace_back(author_, email_);
    }

    for (auto& c : commands_) {
        if (c->help_name().empty()) {
            std::ostringstream oss;
            oss << help_name_ << " " << c->name();
            c->set_help_name(oss.str());
        }
    }

    if (command(kHelpCommand.name()) == nullptr) {
        if (!hide_help_command_) {
            commands_.push_back(&kHelpCommand);
        }
    }

    if (writer_ == nullptr) {
        writer_ = &std::cout;
    }

    if (err_writer_ == nullptr) {
        err_writer_ = &std::cerr;
    }
}

Error App::Run(int argc, char* argv[]) {
    Error error = ErrorOK;

    Setup();

    auto context = std::make_shared<Context>(shared_from_this(), argc, argv, nullptr);
    if (before_) {
        error = before_(context);
        if (error) {
            return error;
        }
    }

    auto c = command(argv[1]);
    if (c) {
        context->set_command(c);
        return c->Run(context);
    }

    if (!action_) {
        action_ = kHelpCommand.action();
    }

    error = action_(context);
    return error;
}

std::shared_ptr<App> NewApp(const std::string& app_name) {
    auto global_options = std::make_shared<boost::program_options::options_description>("Global Options");
    global_options->add_options()
      ("config-dir,C", boost::program_options::value< std::string >(),
        "[DEPRECATED] path to legacy configuration directory")
      ("certs-dir,S", boost::program_options::value< std::string >(),
        "path to certs directory")
      ("quiet", "disable startup and info messages")
      ("anonymous", "hide sensitive information from logging");
    
    auto app = std::make_shared<App>(app_name);
    app->set_author("CppIO");
    app->set_version("0.1");
    app->set_usage("Hybrid Storage with freely combine");
    app->set_description("Build hybrid data infrastructure for machine learning, \
                         analytics and application data workloads with CppIO");
    app->set_flags({ global_options });
    app->set_hide_help_command(true);
    return app;
}

} // namespace cli
} // namespace CPPIO_NAMESPACE