
// Copyright 2025 cppio authors. All rights reserved.

#include "striper.h"
#include "storage_interface.h"
#include "log.h"

namespace CPPIO_NAMESPACE {

VolumeRAID1Striper::~VolumeRAID1Striper() {}

Error VolumeRAID1Striper::Create(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error VolumeRAID1Striper::Read(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error VolumeRAID1Striper::Write(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error VolumeRAID1Striper::CreateDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error VolumeRAID1Striper::ReadDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;

    std::string path = io.path_or_key();
    size_t offset = io.offset();
    size_t len = io.len();
    // std::vector<std::byte>& buff = io.buffer();

    auto cache = vol_ctx.sub_volume_contexts()[0];
    auto backend = vol_ctx.sub_volume_contexts()[1];

    CPPIOLOG::info("VolumeRAID1Striper::ReadDir(), Cache Volume {}, Backend Volume {}",
                   cache->volume()->name(), backend->volume()->name());
    
    auto cache_api = StorageAPIs::GetStorageApi(cache);
    if (cache_api) {
        err = cache_api.value()->ReadDir(vol_ctx, io);
    }

    if (!cache_api || (err && err->code() == std::errc::no_such_file_or_directory)) {
        CPPIOLOG::warn("VolumeRAID1Striper::ReadDir(), volume {}, path {} not found on cache volume {}",
                       vol_ctx.volume()->name(), path, cache->volume()->name());
        auto backend_api = StorageAPIs::GetStorageApi(backend);
        if (backend_api) {
            err = backend_api.value()->ReadDir(vol_ctx, io);
        }
    }
    return err;
}

Error VolumeRAID1Striper::WriteDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

} // namespace CPPIO_NAMESPACE