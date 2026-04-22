// Copyright 2025 cppio authors. All rights reserved.

#include <filesystem>

#include "fs_storage_api.h"
#include "log.h"
#include "striper.h"

namespace CPPIO_NAMESPACE {

FSStorageApi::~FSStorageApi() {}

Error FSStorageApi::Create(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error FSStorageApi::Read(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}
Error FSStorageApi::Write(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error FSStorageApi::CreateDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

Error FSStorageApi::ReadDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;

    std::string path = io.path_or_key();
    size_t offset = io.offset();
    size_t len = io.len();
    // std::vector<std::byte>& buff = io.buffer;

    LOG_INFO("FSStorageApi::ReadDir(), volume {}, root path {}, path {}",
                   vol_ctx.volume()->name(), vol_ctx.root_path(), path);

    if (vol_ctx.volume()->type() == VOLUMETYPE_LEAFVOLUME) {
        std::filesystem::path root_dir(vol_ctx.root_path());
        std::filesystem::path subpath(path);
        auto final_path = root_dir;
        final_path /= subpath;
        if (final_path == "/" ) {
            final_path = root_dir;
        }
        LOG_INFO("FSStorageApi::ReadDir(), volume {}, root path {}, final path {}",
                        vol_ctx.volume()->name(), vol_ctx.root_path(), final_path.string());
        if (std::filesystem::exists(final_path)) {
            for (const auto& entry : std::filesystem::directory_iterator{final_path}) {
                std::string filename = entry.path().filename();
                LOG_INFO("File: {}", filename);
            }
        } else {
            LOG_INFO("Directory does not exist.");
            err = MAKE_ERROR(std::errc::no_such_file_or_directory, "Directory does not exist.");
        }
    } else {
        auto api = StorageAPIs::GetStorageApi(vol_ctx);
        if (api) {
            err = api.value()->ReadDir(vol_ctx, io);
        }
    }

    return err;
}

Error FSStorageApi::WriteDir(VolumeContext& vol_ctx, IODescript& io) {
    Error err = ErrorOK;
    return err;
}

} // namespace CPPIO_NAMESPACE