
// Copyright 2025 cppio authors. All rights reserved.

#include "striper.h"
#include "storage_interface.h"
#include "log.h"

namespace CPPIO_NAMESPACE {

VolumeCacheStriper::~VolumeCacheStriper() {}

Error VolumeCacheStriper::Create(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error VolumeCacheStriper::Read(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error VolumeCacheStriper::Write(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error VolumeCacheStriper::CreateDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error VolumeCacheStriper::ReadDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;

    std::string path = io.path_or_key();
    size_t offset = io.offset();
    size_t len = io.len();
    // std::vector<std::byte>& buff = io.buffer();

    auto cache = vol_ctx.sub_volume_contexts()[0];
    auto backend = vol_ctx.sub_volume_contexts()[1];

    LOG_INFO("VolumeCacheStriper::ReadDir(), Cache Volume {}, Backend Volume {}",
                   cache->volume()->name(), backend->volume()->name());
    
    auto cache_api = StorageAPIs::GetStorageApi(cache);
    if (cache_api) {
        err = cache_api.value()->ReadDir(*cache, io);
    }

    if (!cache_api || (err == std::errc::no_such_file_or_directory)) {
        LOG_WARN("VolumeCacheStriper::ReadDir(), volume {}, path {} not found on cache volume {}",
                       vol_ctx.volume()->name(), path, cache->volume()->name());
        auto backend_api = StorageAPIs::GetStorageApi(backend);
        if (backend_api) {
            err = backend_api.value()->ReadDir(*backend, io);
        }
    }
    return err;
}

Error VolumeCacheStriper::WriteDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

} // namespace CPPIO_NAMESPACE