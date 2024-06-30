#include "include/cli.hpp"
#include <boost/program_options.hpp>
#include "include/log.hpp"
#include "include/httpserver.hpp"

using namespace cppio;

Error serverMain(cli::Context* ctx);

boost::program_options::options_description serverCmdOptions("Options");
cli::Command serverCmd {
    .name = "server",
    .usage = "start object storage server",
    .action = cli::ActionFunc(serverMain),
    .options = &serverCmdOptions,
    .customHelpTemplate = R"(NAME:
  {{.HelpName}} - {{.Usage}}

USAGE:
  {{.HelpName}} {{if .VisibleFlags}}[FLAGS] {{end}}DIR1 [DIR2..]
  {{.HelpName}} {{if .VisibleFlags}}[FLAGS] {{end}}DIR{1...64}
  {{.HelpName}} {{if .VisibleFlags}}[FLAGS] {{end}}DIR{1...64} DIR{65...128}

DIR:
  DIR points to a directory on a filesystem. When you want to combine
  multiple drives into a single large system, pass one directory per
  filesystem separated by space. You may also use a '...' convention
  to abbreviate the directory arguments. Remote directories in a
  distributed setup are encoded as HTTP(s) URIs.
{{if .VisibleFlags}}
FLAGS:
  {{range .VisibleFlags}}{{.}}
  {{end}}{{end}}
EXAMPLES:
  1. Start CppIO server on "/home/shared" directory.
     {{.Prompt}} {{.HelpName}} /home/shared

  2. Start single node server with 64 local drives "/mnt/data1" to "/mnt/data64".
     {{.Prompt}} {{.HelpName}} /mnt/data{1...64}

  3. Start distributed CppIO server on an 32 node setup with 32 drives each, run following command on all the nodes
     {{.Prompt}} {{.HelpName}} http://node{1...32}.example.com/mnt/export{1...32}

  4. Start distributed CppIO server in an expanded setup, run the following command on all the nodes
     {{.Prompt}} {{.HelpName}} http://node{1...16}.example.com/mnt/export{1...32} \
            http://node{17...64}.example.com/mnt/export{1...64}

  5. Start distributed CppIO server, with FTP and SFTP servers on all interfaces via port 8021, 8022 respectively
     {{.Prompt}} {{.HelpName}} http://node{1...4}.example.com/mnt/export{1...4} \
           --ftp="address=:8021" --ftp="passive-port-range=30000-40000" \
           --sftp="address=:8022" --sftp="ssh-private-key=${HOME}/.ssh/id_rsa"
)",
};

Error serverMain(cli::Context* ctx) {
    Error err = nullptr;
    std::string config;
    serverCmd.options->add_options()
        ("config,C",
            boost::program_options::value< std::string >(&config)->default_value("Config"),
            "specify server configuration via YAML configuration")
        ("address,A",
            boost::program_options::value< std::string >(),
            "bind to a specific ADDRESS:PORT, ADDRESS can be an IP or hostname");

    boost::program_options::variables_map optionSet;
    boost::program_options::store(
        boost::program_options::parse_command_line(ctx->argc, ctx->argv, *(serverCmd.options)),
        optionSet);
    boost::program_options::notify(optionSet);
    std::cout << serverCmdOptions << std::endl;
    gLogger->info("Your input for --config={}", config);

    HttpServer httpServer;
    httpServer.start();

    return err;
}