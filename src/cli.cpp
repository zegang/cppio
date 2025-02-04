#include "include/cli.hpp"
#include <algorithm>

using namespace cppio;

// boost::program_options::options_description helpCmdOpts("Options");
// helpCmdOpts.add_options()("help", "more helps on cppio");

cli::Command helpCommand{
    .name = "help",
    .aliases = std::vector<std::string>(1, "h"),
    .usage = "Shows a list of commands or help for one command",
    .argsUsage = "[command]",
    .action = [](cli::Context* ctx) {
        // auto args = ctx.args();
        // if (args.present()) {
        //     return showCommandHelp(ctx, args.first());
        // }
        // showAppHelp(ctx);
        std::cout << helpCommand.usage << std::endl;
        return nullptr;
    },
};

Error cli::Command::run(cli::Context* ctx) {
    if (!subcommands.empty()) {
        return startApp(ctx);
    }

    if (!action) {
        action = helpCommand.action;
    }

    return action(ctx);
}

Error cli::Command::startApp(cli::Context* ctx) {
    return nullptr;
}

std::vector<std::string> cli::Command::namesWithHiddenAliases() {
    std::vector<std::string> names;
    names.push_back(name);
    if (!shortName.empty()) {
        names.push_back(shortName);
    }
    names.insert(names.end(), aliases.begin(), aliases.end());
    return names;
}

// HasName returns true if Command.Name or Command.ShortName matches given name
bool cli::Command::hasName(const std::string& name) {
    auto names = namesWithHiddenAliases();
    auto it = std::find(names.begin(), names.end(), name);
    return it != names.end();
}

// Command returns the named command on App. Returns nil if the command does not exist
cli::Command* cli::App::command(const std::string& name) {
    for (auto& c : commands) {
        if (c->hasName(name)) {
            return c;
        }
    }

    return nullptr;
}

void cli::App::setup() {
    if (didSetup) {
        return;
    }

    didSetup = true;
    if (!author.empty() || !email.empty()) {
        authors.emplace_back(author, email);
    }

    for (auto& c : commands) {
        if (c->helpName.empty()) {
            std::ostringstream oss;
            oss << helpName << " " << c->name;
            c->helpName = oss.str();
        }
    }

    if (command(helpCommand.name) == nullptr) {
        if (!hideHelpCommand) {
            commands.push_back(&helpCommand);
        }
    }

    if (writer == nullptr) {
        writer = &std::cout;
    }

    if (errWriter == nullptr) {
        errWriter = &std::cerr;
    }
}

Error cli::App::run(int argc, char* argv[]) {
    Error error;

    setup();

    cli::Context context(this, argc, argv, nullptr);
    if (before) {
        error = before(context);
        if (error) {
            return error;
        }
    }

    auto c = command(argv[1]);
    if (c) {
        context.command = c;
        return c->run(&context);
    }

    if (!action) {
        action = helpCommand.action;
    }

    error = action(&context);
    return error;
}