// Copyright 2025 cppio authors. All rights reserved.

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include <thread>

#include "cli.h"
#include "log.h"
#include "storage.grpc.pb.h"

namespace CPPIO_NAMESPACE {

std::vector<std::string> serverCmdArgs(std::shared_ptr<cli::Context> ctx);

Error ServerMain(std::shared_ptr<cli::Context> ctx);

// boost::program_options::options_description serverCmdOptions("Options");
cli::CommandDef kServerCmdDef {
    .name = "server",
    .usage = "start object storage server",
    .action = cli::ActionFunc(ServerMain),
    .flags = { "Command Server Options",
               [] (std::shared_ptr<boost::program_options::options_description> opt_des) {
                opt_des->add_options()
                ("help,h", "help")
                ("config,C",
                    boost::program_options::value< std::string >(),
                    "specify server configuration via YAML configuration")
                ("address,A",
                    boost::program_options::value< std::string >(),
                    "bind to a specific ADDRESS:PORT, ADDRESS can be an IP or hostname")
                ("flag,f", boost::program_options::bool_switch(), "Boolean flag option")
                ("positional", boost::program_options::value< std::vector<std::string> >());
               }
    },
    .custom_help_template = R"(NAME:
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
)"
};

cli::Command kServerCmd(kServerCmdDef);

Error ServerMain(std::shared_ptr<cli::Context> ctx) {
    Error err = ErrorOK;
  
    if (kServerCmd.IsOptionSet("help")) {
        std::cout << kServerCmd.Help();
        return ErrorOK;
    }

    if (kServerCmd.IsOptionSet("config")) {
        CPPIOLOG::info("--config={}", kServerCmd["config"].as< std::string >());
    }

    const char* svc_name_env = std::getenv("CPPIO_HSD_SVC_NAME");
    const char* svc_port_env = std::getenv("CPPIO_HSD_SVC_PORT");
    std::string svc_name = svc_name_env ? svc_name_env : "cppio-hsd-pod";
    std::string svc_port = svc_port_env ? svc_port_env : "9999";
    std::string grpc_svr_target = svc_name + ":" + svc_port;
    CPPIOLOG::info("CppIO HDS Service Address - {}", grpc_svr_target);
    auto channel = grpc::CreateChannel(grpc_svr_target, grpc::InsecureChannelCredentials());
    auto stub = StorageService::NewStub(channel);
    grpc::ClientContext client_context;
    std::string path = ctx->Args()[0];
    IODescript io_i, io_o;
    io_i.set_path_or_key(path);
    auto status = stub->Read(&client_context, io_i, &io_o);
    if (status.ok()) {
      CPPIOLOG::info("Read successful for path: {}", path);
    } else {
      CPPIOLOG::error("Read failed for path: {} - Code: {}, Message: {}", 
               path, status.error_code(), status.error_message());
    }
    return err;
}

} // namespace CPPIO_NAMESPACE