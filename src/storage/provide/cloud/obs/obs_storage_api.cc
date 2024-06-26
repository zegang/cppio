// Copyright (C) 2025 cppio authors. All rights reserved.

#include <filesystem>

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "obs_storage_api.h"
#include "log.h"
#include "striper.h"

namespace CPPIO_NAMESPACE {

OBSStorageApi::OBSStorageApi() {
    // Aws::SDKOptions options;
    // Aws::InitAPI(options);
}

OBSStorageApi::~OBSStorageApi() {
    // Aws::ShutdownAPI(options);
}

Error OBSStorageApi::Create(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error OBSStorageApi::Read(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error OBSStorageApi::Write(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error OBSStorageApi::CreateDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error OBSStorageApi::ReadDir(VolumeContext& vol_ctx, IODescript& io) {
    return ListBucket(vol_ctx, io);
}

Error OBSStorageApi::WriteDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error OBSStorageApi::ListBucket(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;

    std::string path = io.path_or_key();
    size_t offset = io.offset();
    size_t len = io.len();
    // std::vector<std::byte>& buff = io.buffer();

    CPPIOLOG::info("OBSStorageApi::ListBucket(), volume {}, root path {}, path {}",
                    vol_ctx.volume()->name(), vol_ctx.root_path(), path);

    if (vol_ctx.volume()->type() == VOLUMETYPE_LEAFVOLUME) {
        auto outcome = getClient(vol_ctx)->ListBuckets();
        if (!outcome.IsSuccess()) {
            std::cerr << "Failed with error: " << outcome.GetError() << std::endl;
            // CPPIOLOG::info("Failed with error: {}", outcome.GetError().GetMessage());
            err = MAKE_ERROR(std::errc::no_such_file_or_directory, "OBS Not Supported Yet.");
        } else {
            CPPIOLOG::info("Found {} buckets", outcome.GetResult().GetBuckets().size());
            for (auto &&b: outcome.GetResult().GetBuckets()) {
                CPPIOLOG::info("{}", b.GetName());
            }
        }
    } else {
        auto api = StorageAPIs::GetStorageApi(vol_ctx);
        if (api) {
            err = api.value()->ReadDir(vol_ctx, io);
        }
    }

    return err;
}

std::shared_ptr<Aws::S3::S3Client> OBSStorageApi::getClient(VolumeContext& vol_ctx) {
    if (clients_.count(vol_ctx.volume()->name()) == 0) {
        Aws::Client::ClientConfiguration config;
        config.endpointOverride = Aws::String(vol_ctx.volume()->url());
        Aws::Auth::AWSCredentials credentials(
            vol_ctx.volume()->credential().user(),
            vol_ctx.volume()->credential().password());
        clients_[vol_ctx.volume()->name()] = Aws::MakeShared<Aws::S3::S3Client>(
            vol_ctx.volume()->name().c_str(), credentials, config,
            Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
            false);
    }
    return clients_[vol_ctx.volume()->name()];
}

} // namespace CPPIO_NAMESPACE