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

std::vector<PoolEndpoints>
createPoolEndpoints(std::string serverAddr, std::vector<PoolDisksLayout>& poolsLayout);
void createServerEndpoints(std::string serverAddr,
    std::vector<PoolDisksLayout>& poolArgs, bool legacy);

// boost::program_options::options_description serverCmdOptions("Options");
cli::Command serverCmd {
    .name = "server",
    .usage = "start object storage server",
    .action = cli::ActionFunc(serverMain),
    .flags = { .optionsDescription = std::make_shared<boost::program_options::options_description>("Command Server Options") },
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

    // assert(ctx->command == &serverCmd);
  
    serverCmd.flags.optionsDescription->add_options()
        ("config,C",
            boost::program_options::value< std::string >(),
            "specify server configuration via YAML configuration")
        ("address,A",
            boost::program_options::value< std::string >(),
            "bind to a specific ADDRESS:PORT, ADDRESS can be an IP or hostname")
        ("flag,f", boost::program_options::bool_switch(), "Boolean flag option")
        ("positional", boost::program_options::value< std::vector<std::string> >());

    serverCmd.flags.parse(ctx->argc, ctx->argv);
  
    std::cout << *serverCmd.flags.optionsDescription << std::endl;

    if (serverCmd.flags.count("config")) {
        std::cout << "Your input for --config=" << serverCmd.flags["config"].as< std::string >() << std::endl;
        gLogger->info("Your input for --config={}", serverCmd.flags["config"].as< std::string >());
    }

    for (const auto& arg : serverCmd.flags.args()) {
        std::cout << " " << arg;
    }

    // HttpServer httpServer;
    // httpServer.start();

    buildServerCtxt(ctx, &globalServerCtxt);

    if (globalServerCtxt.layout.pools.empty()) {
        std::cout << "Pls. provide storage pools" << std::endl;
        return err;
    }

    serverHandleCmdArgs(&globalServerCtxt);
    // newObject, err = newObjectLayer(GlobalContext, globalEndpoints)
    // newObject = newErasureServerPools(ctx, endpointServerPools)

    return err;
}

Error buildServerCtxt(cli::Context* ctx, ServerCtxt *ctxt)
{
    if (ctx->command->flags.count("address")) {
        ctxt->addr = ctx->command->flags["address"].as< std::string >();
    }
    
    return mergeDisksLayoutFromArgs(ctx->args(), ctxt);
}

// mergeDisksLayoutFromArgs supports with and without ellipses transparently.
Error mergeDisksLayoutFromArgs(std::vector<std::string> args, ServerCtxt *ctxt)
{
    ctxt->layout.legacy = true;
    for (auto& arg : args) {
        std::cout << "mergeDisksLayoutFromArgs, arg: " << arg << std::endl;
        PoolDisksLayout pdl;
        pdl.cmdLine = arg;
        auto disks = std::vector<std::string>();
        disks.emplace_back(arg);
        pdl.layout.push_back(disks);
        ctxt->layout.pools.push_back(pdl);
    }
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
    createServerEndpoints(globalMinioAddr, ctxt->layout.pools, ctxt->layout.legacy);

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

std::vector<PoolEndpoints>
createPoolEndpoints(std::string serverAddr, std::vector<PoolDisksLayout>& poolsLayout) {
    std::vector<PoolEndpoints> poolEndpoints(1);

    // For single arg, return single drive EC setup.
    Endpoint endpoint;
    endpoint.path = poolsLayout[0].layout[0][0];
    endpoint.poolIndex = 0;
    endpoint.setIndex = 0;
    endpoint.diskIndex = 0;

    poolEndpoints[0].endpoints.push_back(endpoint);
    return poolEndpoints;
}

// CreateServerEndpoints - validates and creates new endpoints from input args, supports
// both ellipses and without ellipses transparently.
void
createServerEndpoints(std::string serverAddr,
                      std::vector<PoolDisksLayout>& poolArgs,
                      bool legacy)
{
    assert(!poolArgs.empty());

    auto poolEndpoints = createPoolEndpoints(serverAddr, poolArgs);
    auto endpointServerPools = &globalEndpoints;

    for (auto i = 0; i < poolEndpoints.size(); ++i) {
        PoolEndpoints eps;
        eps.legacy = legacy;
        eps.setCount = poolArgs[i].layout.size();
        eps.driversPerSet = poolArgs[i].layout[0].size();
        eps.endpoints = poolEndpoints[i].endpoints;
        eps.platform = []() {
                            std::stringstream ss;
                            ss << "OS: " << "your_GOOS_value" << " | Arch: " << "your_GOARCH_value";
                            return ss.str();
                        }();
        eps.cmdLine = poolArgs[i].cmdLine;
        endpointServerPools->add(eps);
        std::cout << "endpointServerPools Index " << i << ", endpoints ";
        for (auto& ep : eps.endpoints) {
            std::cout << " - path: " << ep.path;
        }
        std::cout << std::endl;
    }
}