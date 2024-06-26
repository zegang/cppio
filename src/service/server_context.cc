// Copyright 2025 cppio authors. All rights reserved.

#include "server_context.h"
#include "../include/error.h"
#include "../layout/topology.h"

namespace CPPIO_NAMESPACE {

// Error BuildServerContext(std::shared_ptr<cli::Context> ctx,
//                          std::shared_ptr<GlobalServerContext> ctxt)
// {
//     Error err = ErrorOK;

//     ctxt->set_json(ctx->IsOptionSet("json"));
//     ctxt->set_quiet(ctx->IsOptionSet("quiet"));
//     ctxt->set_anonymous(ctx->IsOptionSet("anonymous"));
//     ctxt->set_strict_s3_compat(ctx->IsOptionSet("no-compat"));
    
//     if (ctx->IsOptionSet("address")) {
//         ctxt->set_addr(ctx->operator[]("address").as< std::string >());
//     }
    
//     VolumeContext volume_context(ctx->Args()[0]);
//     ctxt->set_volume_context(volume_context);

//     return err;
// }

// void ServerHandleCmdArgs(GlobalServerContext& ctxt) { }

} // namespace CPPIO_NAMESPACE