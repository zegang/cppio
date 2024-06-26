// Copyright 2025 cppio authors. All rights reserved.

//
// This is to implement a command line library like https://github.com/minio/cli.
//
#ifndef CPPIO_CLI_H_
#define CPPIO_CLI_H_

#include <string>
#include <vector>
#include <map>
#include <ctime> // For time handling
#include <functional>
#include <boost/program_options.hpp>
#include <any>

#include "debug.h"
#include "error.h"

namespace CPPIO_NAMESPACE {

namespace cli {

class Flags;
class App;
class Command;
typedef std::vector<Command*> Commands;
class Context;

// Define function types for callbacks
using BashCompleteFunc = std::function<void()>;
using BeforeFunc = std::function<void()>;
using AfterFunc = std::function<void()>;
using ActionFunc = std::function<::CPPIO_NAMESPACE::Error(std::shared_ptr<Context> ctx)>;
using DefaultOptionsFunc = std::function<void(std::shared_ptr<boost::program_options::options_description>)>;
using OnUsageErrorFunc = std::function<void(const std::string&)>;

class Flags {
private:
    bool is_parsed_ = false;
    boost::program_options::variables_map option_set_;
    boost::program_options::positional_options_description positional_set_;
    std::shared_ptr<boost::program_options::options_description> options_description_;

public:
    Flags() = default;
    Flags(const std::string& des, DefaultOptionsFunc def_opt_func) :
        options_description_(std::make_shared<boost::program_options::options_description>(des)) {
            def_opt_func(options_description_);
    }
    Flags(std::shared_ptr<boost::program_options::options_description> des) : options_description_(des) { }

    bool is_parsed() const { return is_parsed_; }
    boost::program_options::variables_map& option_set() { return option_set_; }
    boost::program_options::positional_options_description& positional_set() { return positional_set_; }
    std::shared_ptr<boost::program_options::options_description> options_description() { return options_description_; }

    void Parse(int argc, char** argv) {
        positional_set_.add("positional", -1);
        boost::program_options::store(
            boost::program_options::command_line_parser(argc-1, &argv[1])
                .options(*options_description_)
                .positional(positional_set_).run(), option_set_);
        boost::program_options::notify(option_set_);
        is_parsed_ = true;
    }

    bool Count(const std::string& flag) const { CPPIO_ASSERT(is_parsed()); return option_set_.count(flag); }
    auto& operator[](const std::string& flag) { CPPIO_ASSERT(is_parsed()); return option_set_[flag]; }
    auto Args() {
        CPPIO_ASSERT(is_parsed());
        if (Count("positional")) {
            return option_set_["positional"].as< std::vector<std::string> >();
        }
        return std::vector<std::string>();
    }
};

/// @brief Author Information
class Author {
private:
    std::string name_;
    std::string email_;

public:
    Author(const std::string& name, const std::string& email) : name_(name), email_(email) {}

    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    // Method to convert Author to string
    std::string ToString() const {
        std::ostringstream oss;
        oss << name_;
        if (!email_.empty()) {
            oss << " <" << email_ << ">";
        }
        return oss.str();
    }
};

// App is the main structure of a cli application. It is recommended that
// an app be created with the cli::newApp() function
class App : public std::enable_shared_from_this<App> {
private:
    // The name of the program. Defaults to path.Base(os.Args[0])
    std::string name_;
    // Full name of command for help, defaults to Name
    std::string help_name_;
    // Description of the program.
    std::string usage_;
    // Text to override the USAGE section of help
    std::string usage_text_;
    // Description of the program argument format.
    std::string args_usage;
    // Version of the program
    std::string version_;
    // Description of the program
    std::string description_;
    // List of commands to execute
    Commands commands_;
    std::size_t max_cmd_name_len_;
    // List of options to parse
    Flags flags_;
    // Boolean to enable bash completion commands
    bool enable_bash_completion_;
    // Boolean to hide built-in help flag
    bool hide_help_;
    // Boolean to hide built-in help command
    bool hide_help_command_;
    // Boolean to hide built-in version flag and the VERSION section of help
    bool hide_version_;
    // Populate on app startup, only gettable through method Categories()
    std::map<std::string, Commands> categories_;
    // An action to execute when the bash-completion flag is set
    std::function<void()> bash_complete_;
    // An action to execute before any subcommands are run, but after the context is ready
    // If a non-nil error is returned, no subcommands are run
    std::function<Error(std::shared_ptr<Context>)> before_;
    // An action to execute after any subcommands are run, but after the subcommand has finished
    // It is run even if Action() panics
    std::function<void()> after_;

    // The action to execute when no subcommands are specified
    // Expects a `cli.ActionFunc` but will accept the *deprecated* signature of `func(*cli.Context) {}`
    // *Note*: support for the deprecated `Action` signature will be removed in a future version
    ActionFunc action_;

    // Execute this function if the proper command cannot be found
    std::function<void()> command_not_found_;
    // Execute this function if an usage error occurs
    std::function<void()> on_usage_error_;
    // Compilation date
    std::time_t compiled_;
    // List of all authors who contributed
    std::vector<Author> authors_;
    // Copyright of the binary if any
    std::string copyright_;
    // Name of Author (Note: Use App.Authors, this is deprecated)
    std::string author_;
    // Email of Author (Note: Use App.Authors, this is deprecated)
    std::string email_;
    // Writer for standard output
    std::ostream* writer_ = nullptr;
    // HelpWriter for help output
    std::ostream* help_writer_ = nullptr;
    // ErrWriter for error output
    std::ostream* err_writer_ = nullptr;
    // Other custom info
    std::map<std::string, std::any> metadata_;
    // Carries a function which returns app specific info.
    std::function<std::map<std::string, std::string>()> extra_info_;
    // CustomAppHelpTemplate the text template for app help topic.
    // cli.go uses text/template to render templates. You can
    // render custom help text by setting this variable.
    std::string custom_app_help_template_;

    bool did_setup_;

public:
    // Constructor
    App(const std::string& name)
        : name_(name), compiled_(std::time(nullptr)), enable_bash_completion_(false),
          hide_help_(false), hide_help_command_(false), hide_version_(false),
          did_setup_(false), max_cmd_name_len_(0) {}

    // Destructor (if needed)
    ~App() {}

    // Methods as needed
    void set_compiled_time(std::time_t time) { compiled_ = time; }

    // Other methods and functionality as per application needs
    Error add_command(Command *command);
    Command* command(const std::string& name);
    Commands commands() const { return commands_; }
    std::size_t max_cmd_name_len() const { return max_cmd_name_len_; }
    auto action() { return action_; }

    void set_flags(Flags flags) { flags_ = flags; }
    void set_version(const std::string& version) { version_ = version; }
    void set_usage(const std::string& usage) { usage_ = usage; }
    void set_description(const std::string& des) { description_ = des; }
    void set_hide_help_command(bool hide) { hide_help_command_ = hide; }
    void set_author(const std::string& author) { author_ = author; }

    void set_custom_app_help_template(const std::string& help_template) { custom_app_help_template_ = help_template; }
    
    void Setup();
    Error Run(int argc, char* argv[]);
};

struct CommandDef {
    std::string name;                       // The name of the command
    std::string short_name;                  // short name of the command. Typically one character (deprecated, use `Aliases`)
    std::vector<std::string> aliases;       // A list of aliases for the command
    std::string usage;                      // A short description of the usage of this command
    std::string usage_text;                  // Custom text to show on USAGE section of help
    std::string description;                // A longer explanation of how the command works
    std::string args_usage;                  // A short description of the arguments of this command
    std::string category;                   // The category the command is part of
    BashCompleteFunc bash_complete;          // The function to call when checking for bash command completions
    BeforeFunc before;                      // An action to execute before any sub-subcommands are run, but after the context is ready
    AfterFunc after;                        // An action to execute after any subcommands are run, but after the subcommand has finished
    ActionFunc action;                      // The function to call when this command is invoked
    OnUsageErrorFunc on_usage_error;          // Execute this function if a usage error occurs
    Flags flags;                            // List of options to parse
    bool skip_flag_parsing;                   // Treat all flags as normal arguments if true
    bool skip_arg_reorder;                    // Skip argument reordering which attempts to move flags before arguments
    bool hide_help;                          // Boolean to hide built-in help flag
    bool hide_help_command;                   // Boolean to hide built-in help command
    bool hidden;                            // Boolean to hide this command from help or completion
    bool hidden_aliases;                     // Boolean to hide aliases for this command from help or completion
    std::string help_name;                   // Full name of command for help, defaults to full command name, including parent commands
    std::vector<std::string> command_name_path; // Path to the command
    std::string prompt;                     // Default prompt, specific to OS
    std::string env_var_set_command;           // Command to set the environment variable, specific to OS
    std::string assignment_operator;         // Assignment operator to set the environment variable, specific to OS
    std::string disable_istory;             // Disable history for security reasons
    std::string enable_history;              // Enable history
    std::string custom_help_template;         // Custom help template for the command
};

// Command structure in C++
class Command {
private:
    CommandDef command_def_;
    Commands subcommands_;

public:
    Command() = default;
    Command(CommandDef& def) : command_def_(def) {}

    const auto name() const { return command_def_.name; }
    auto usage() { return command_def_.usage; }
    auto action() { return command_def_.action; }

    const auto help_name() const { return command_def_.help_name; }
    void set_help_name(const std::string& help_name) { command_def_.help_name = help_name; }

    std::string Help() {
        std::ostringstream oss;
        oss << name() << ": " << usage() << std::endl;
        oss << *options_description() << std::endl;
        return oss.str();
    }

    Error Run(std::shared_ptr<Context> ctx);
    Error StartApp(std::shared_ptr<Context> ctx);

    std::shared_ptr<boost::program_options::options_description> options_description() {
        return command_def_.flags.options_description();
    }

    std::vector<std::string> NamesWithHiddenAliases();
    bool has_name(const std::string& name);

    bool IsParsed() { return command_def_.flags.is_parsed(); }
    void DoParseIfNot(int argc, char** argv) {
        if (!IsParsed()) command_def_.flags.Parse(argc, argv);
    }
    void Parse(int argc, char** argv) { command_def_.flags.Parse(argc, argv); }

    bool IsOptionSet(const std::string& opt) { return command_def_.flags.Count(opt); }
    auto& operator[](const std::string& opt) { return command_def_.flags[opt]; }

    std::vector<std::string> Args() { return command_def_.flags.Args(); }
};

// Context is a type that is passed through to
// each Handler action in a cli application. Context
// can be used to retrieve context-specific Args and
// parsed command-line options.
class Context {
private:
    std::shared_ptr<App> app_;
    // the command currently servicing, e.g. server
    Command* command_;
    bool shell_complete_;
    int argc_;
    char** argv_;
    std::shared_ptr<Context> parent_context_;

    void do_command_parse_ifneeded() {
        if (command_) {
            command_->DoParseIfNot(argc_, argv_);
        }
    }

public:
    Context(std::shared_ptr<App> app, int argc, char* argv[], std::shared_ptr<Context> parent)
        : app_(app), argc_(argc), argv_(argv), parent_context_(parent) {
            if (parent_context_) {
                shell_complete_ = parent_context_->shell_complete();
            }
    }

    auto app() const { return app_; }
    auto command() { return command_; }
    void set_command(auto command) { command_ = command; do_command_parse_ifneeded(); }
    auto argc() { return argc_; }
    auto argv() { return argv_; }
    bool shell_complete() { return shell_complete_; }

    /// @brief Whether Option set on the command or not
    /// @param opt The Option
    /// @return 
    bool IsOptionSet(const std::string& opt) const {
        return command_->IsOptionSet(opt);
    }
    auto& operator[](const std::string& opt) { return (*command_)[opt]; }

    /// @brief All arguments of the command
    /// @return 
    std::vector<std::string> Args() {
        return command_->Args();
    }
};

// Flag structure in C++
// struct Flag {
//     std::string name;           // Name of the flag
//     std::string usage;          // Usage description of the flag
//     std::string envVar;         // Environment variable associated with the flag
//     std::string placeholder;    // Placeholder for the flag
//     std::string valueName;      // Name of the value associated with the flag
//     std::string destination;    // Destination for the flag
//     std::string fileName;       // File name associated with the flag
//     std::string fileType;       // File type associated with the flag
//     std::string completion;     // Completion information for the flag
//     bool noOptDefVal;           // Boolean indicating if there is no optional default value
//     std::string defaultText;    // Default text for the flag
//     bool hidden;                // Boolean indicating if the flag is hidden
//     bool required;              // Boolean indicating if the flag is required
//     bool deprecated;            // Boolean indicating if the flag is deprecated
//     bool isSet;                 // Boolean indicating if the flag is set
//     bool takesValue;            // Boolean indicating if the flag takes a value
// };

/// @brief Create a App per given name
/// @param app_name
/// @return 
std::shared_ptr<App> NewApp(const std::string& app_name);

} // namespace cli

} // namespace CPPIO_NAMESPACE

#endif  // CPPIO_CLI_H_