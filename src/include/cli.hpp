#ifndef CPPIO_CLI_HPP
#define CPPIO_CLI_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime> // For time handling
#include <functional>
#include <boost/program_options.hpp>
#include "error.hpp"
#include <any>

namespace cppio {

namespace cli {

class App;
class Command;
typedef std::vector<Command*> Commands;
class Context;

// Define function types for callbacks
using BashCompleteFunc = std::function<void()>;
using BeforeFunc = std::function<void()>;
using AfterFunc = std::function<void()>;
using ActionFunc = std::function<Error(Context* ctx)>;
using OnUsageErrorFunc = std::function<void(const std::string&)>;

struct Flags {
    bool isParsed = false;
    boost::program_options::variables_map optionSet;
    boost::program_options::positional_options_description positionalSet;
    std::shared_ptr<boost::program_options::options_description> optionsDescription;

    void parse(int argc, char** argv) {
        positionalSet.add("positional", -1);
        boost::program_options::store(
            boost::program_options::command_line_parser(argc-1, &argv[1]).options(*optionsDescription).positional(positionalSet).run(),
            optionSet);
        boost::program_options::notify(optionSet);
        isParsed = true;
    }

    bool count(std::string flag) { return optionSet.count(flag); }
    auto& operator[](std::string flag) { return optionSet[flag]; }
    auto args() {
        if (isParsed) {
            if (count("positional")) {
                return optionSet["positional"].as< std::vector<std::string> >();
            }
        }
        return std::vector<std::string>();
    }
};

struct Author {
    std::string name;
    std::string email;

    Author(std::string& name, std::string& email) : name(name), email(email) {}

    // Method to convert Author to string
    std::string toString() const {
        std::ostringstream oss;
        oss << name;
        if (!email.empty()) {
            oss << " <" << email << ">";
        }
        return oss.str();
    }
};

// App is the main structure of a cli application. It is recommended that
// an app be created with the cli.NewApp() function
class App {
public:
    // The name of the program. Defaults to path.Base(os.Args[0])
    std::string name;
    // Full name of command for help, defaults to Name
    std::string helpName;
    // Description of the program.
    std::string usage;
    // Text to override the USAGE section of help
    std::string usageText;
    // Description of the program argument format.
    std::string argsUsage;
    // Version of the program
    std::string version;
    // Description of the program
    std::string description;
    // List of commands to execute
    Commands commands;
    // List of options to parse
    Flags flags;
    // Boolean to enable bash completion commands
    bool enableBashCompletion;
    // Boolean to hide built-in help flag
    bool hideHelp;
    // Boolean to hide built-in help command
    bool hideHelpCommand;
    // Boolean to hide built-in version flag and the VERSION section of help
    bool hideVersion;
    // Populate on app startup, only gettable through method Categories()
    std::map<std::string, Commands> categories;
    // An action to execute when the bash-completion flag is set
    std::function<void()> bashComplete;
    // An action to execute before any subcommands are run, but after the context is ready
    // If a non-nil error is returned, no subcommands are run
    std::function<Error(Context)> before;
    // An action to execute after any subcommands are run, but after the subcommand has finished
    // It is run even if Action() panics
    std::function<void()> after;

    // The action to execute when no subcommands are specified
    // Expects a `cli.ActionFunc` but will accept the *deprecated* signature of `func(*cli.Context) {}`
    // *Note*: support for the deprecated `Action` signature will be removed in a future version
    ActionFunc action;

    // Execute this function if the proper command cannot be found
    std::function<void()> commandNotFound;
    // Execute this function if an usage error occurs
    std::function<void()> onUsageError;
    // Compilation date
    std::time_t compiled;
    // List of all authors who contributed
    std::vector<Author> authors;
    // Copyright of the binary if any
    std::string copyright;
    // Name of Author (Note: Use App.Authors, this is deprecated)
    std::string author;
    // Email of Author (Note: Use App.Authors, this is deprecated)
    std::string email;
    // Writer for standard output
    std::ostream* writer = nullptr;
    // HelpWriter for help output
    std::ostream* helpWriter = nullptr;
    // ErrWriter for error output
    std::ostream* errWriter = nullptr;
    // Other custom info
    std::map<std::string, std::any> metadata;
    // Carries a function which returns app specific info.
    std::function<std::map<std::string, std::string>()> extraInfo;
    // CustomAppHelpTemplate the text template for app help topic.
    // cli.go uses text/template to render templates. You can
    // render custom help text by setting this variable.
    std::string customAppHelpTemplate;

    bool didSetup;

    // Constructor
    App(const std::string& name)
        : name(name), compiled(std::time(nullptr)), enableBashCompletion(false), hideHelp(false), 
          hideHelpCommand(false), hideVersion(false), didSetup(false) {}

    // Destructor (if needed)
    ~App() {}

    // Methods as needed
    void setCompiledTime(std::time_t time) { compiled = time; }

    // Other methods and functionality as per application needs
    Command* command(const std::string& name);
    void setup();
    Error run(int argc, char* argv[]);
};


// Command structure in C++
class Command {

public:
    std::string name;                       // The name of the command
    std::string shortName;                  // short name of the command. Typically one character (deprecated, use `Aliases`)
    std::vector<std::string> aliases;       // A list of aliases for the command
    std::string usage;                      // A short description of the usage of this command
    std::string usageText;                  // Custom text to show on USAGE section of help
    std::string description;                // A longer explanation of how the command works
    std::string argsUsage;                  // A short description of the arguments of this command
    std::string category;                   // The category the command is part of
    BashCompleteFunc bashComplete;          // The function to call when checking for bash command completions
    BeforeFunc before;                      // An action to execute before any sub-subcommands are run, but after the context is ready
    AfterFunc after;                        // An action to execute after any subcommands are run, but after the subcommand has finished
    ActionFunc action;                      // The function to call when this command is invoked
    OnUsageErrorFunc onUsageError;          // Execute this function if a usage error occurs
    Commands subcommands;                   // List of child commands
    Flags flags;                            // List of options to parse
    bool skipFlagParsing;                   // Treat all flags as normal arguments if true
    bool skipArgReorder;                    // Skip argument reordering which attempts to move flags before arguments
    bool hideHelp;                          // Boolean to hide built-in help flag
    bool hideHelpCommand;                   // Boolean to hide built-in help command
    bool hidden;                            // Boolean to hide this command from help or completion
    bool hiddenAliases;                     // Boolean to hide aliases for this command from help or completion
    std::string helpName;                   // Full name of command for help, defaults to full command name, including parent commands
    std::vector<std::string> commandNamePath; // Path to the command
    std::string prompt;                     // Default prompt, specific to OS
    std::string envVarSetCommand;           // Command to set the environment variable, specific to OS
    std::string assignmentOperator;         // Assignment operator to set the environment variable, specific to OS
    std::string disableHistory;             // Disable history for security reasons
    std::string enableHistory;              // Enable history
    std::string customHelpTemplate;         // Custom help template for the command

public:
    Error run(Context* ctx);
    Error startApp(Context* ctx);
    std::vector<std::string> namesWithHiddenAliases();
    bool hasName(const std::string& name);
    // auto getOptions() { return options; }
};

// Context is a type that is passed through to
// each Handler action in a cli application. Context
// can be used to retrieve context-specific Args and
// parsed command-line options.
class Context {
public:
    App* app;
    Command* command;
    bool shellComplete;
    int argc;
    char** argv;
    // boost::program_options::variables_map optionSet;
    std::unordered_map<std::string, bool> setOptions;
    Context* parentContext;

public:
    // Constructor
    Context(App* app, int argc, char* argv[], Context* parent)
        : app(app), argc(argc), argv(argv), parentContext(parent) {
            // boost::program_options::store(
            //     boost::program_options::parse_command_line(argc, argv, *app->options), optionSet);
            // boost::program_options::notify(optionSet);
            if (parentContext) {
                shellComplete = parentContext->shellComplete;
            }
    }

    // Getters
    App* getApp() const { return app; }

    // Methods
    bool isOptionSet(const std::string& optName) const {
        return setOptions.count(optName);
    }

    void setOption(const std::string& optName, bool value) {
        setOptions[optName] = value;
    }

    // Args returns the command line arguments associated with the context.
    std::vector<std::string> args() {
        return command->flags.args();
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

}

}

#endif  // CPPIO_CLI_HPP