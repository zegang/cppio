#include "include/cli.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include "include/log.hpp"
#include "include/httpserver.hpp"
#include "include/globals.hpp"
#include "include/endpoint.hpp"

using namespace cppio;

Error mergeDisksLayoutFromArgs(std::vector<std::string> args, ServerCtxt *ctxt);
Error buildServerCtxt(cli::Context* ctx, ServerCtxt *ctxt);

std::vector<std::string> serverCmdArgs(cli::Context* ctx);
void serverHandleCmdArgs(ServerCtxt *ctxt);
Error serverMain(cli::Context* ctx);

std::shared_ptr<EndpointServerPools> createServerEndpoints(std::string serverAddr,
    std::vector<PoolDisksLayout> poolArgs, bool legacy);


// boost::program_options::options_description serverCmdOptions("Options");
cli::Command serverCmd {
    .name = "server",
    .usage = "start object storage server",
    .action = cli::ActionFunc(serverMain),
    .flag = { .options = std::make_shared<boost::program_options::options_description>("Command Server Options") },
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
  
    serverCmd.flag.options->add_options()
        ("config,C",
            boost::program_options::value< std::string >(),
            "specify server configuration via YAML configuration")
        ("address,A",
            boost::program_options::value< std::string >(),
            "bind to a specific ADDRESS:PORT, ADDRESS can be an IP or hostname")
        ("flag,f", boost::program_options::bool_switch(), "Boolean flag option")
        ("positional", boost::program_options::value< std::vector<std::string> >());

    serverCmd.flag.parse(ctx->argc, ctx->argv);
  
    std::cout << *serverCmd.flag.options << std::endl;

    if (serverCmd.flag.count("config")) {
        std::cout << "Your input for --config=" << serverCmd.flag["config"].as< std::string >() << std::endl;
        gLogger->info("Your input for --config={}", serverCmd.flag["config"].as< std::string >());
    }

    if (serverCmd.flag.count("positional")) {
        std::cout << "Positional arguments:";
        for (const auto& arg : serverCmd.flag["positional"].as< std::vector<std::string> >()) {
            std::cout << " " << arg;
        }
        std::cout << std::endl;
    }
    
    // HttpServer httpServer;
    // httpServer.start();

    // buildServerCtxt(ctx, &globalServerCtxt);
    // serverHandleCmdArgs(&globalServerCtxt);

    return err;
}

Error buildServerCtxt(cli::Context* ctx, ServerCtxt *ctxt)
{
  return mergeDisksLayoutFromArgs(ctx->args(), ctxt);
}

Error mergeDisksLayoutFromArgs(std::vector<std::string> args, ServerCtxt *ctxt)
{
    ctxt->layout.legacy = true;
    // PoolDisksLayout pdl;
    // pdl.layout.pop_back(args);
    // ctxt->layout.pools.push_back(pdl);
    return nullptr;
}

std::vector<std::string> serverCmdArgs(cli::Context* ctx)
{
    std::vector<std::string> tokens;
    // boost::algorithm::split(tokens, "");
    return tokens;
}

void serverHandleCmdArgs(ServerCtxt *ctxt)
{
    // globalEndpoints, setupType, err = createServerEndpoints(globalMinioAddr, ctxt.Layout.pools, ctxt.Layout.legacy)
    // globalNodes = globalEndpoints.GetNodes()
    // globalIsErasure = (setupType == ErasureSetupType)
    // globalIsDistErasure = (setupType == DistErasureSetupType)
    // if globalIsDistErasure {
    //   globalIsErasure = true
    // }
    // globalIsErasureSD = (setupType == ErasureSDSetupType)
    // if globalDynamicAPIPort && globalIsDistErasure {
    //   logger.FatalIf(errInvalidArgument, "Invalid --address=\"%s\", port '0' is not allowed in a distributed erasure coded setup", ctxt.Addr)
    // }

    // globalLocalNodeName = GetLocalPeer(globalEndpoints, globalMinioHost, globalMinioPort)
}

std::shared_ptr<EndpointServerPools>
createServerEndpoints(std::string serverAddr,
                      std::vector<PoolDisksLayout> poolArgs,
                      bool legacy)
{
    if (poolArgs.empty()) {
        return nullptr;
    }
    return nullptr;
}