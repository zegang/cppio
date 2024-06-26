// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_STORAGE_INTERFACE_H_
#define CPPIO_STORAGE_INTERFACE_H_

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <functional>
#include <mutex>
#include <any>
#include <utility>
#include <optional>
#include <unordered_map>

#include "storage.pb.h"
#include "log.h"
#include "error.h"
#include "volume.h"
#include "component.h"

namespace CPPIO_NAMESPACE {

class StorageApi {
public:
    virtual ~StorageApi() {}
    /*
     * File/Object/Key/Block Operations
     */
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io) = 0;
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io) = 0;
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io) = 0;

    /*
     * Directory/Bucket/Group/Set Opertions
     */
    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io) = 0;
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io) = 0;
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& rd) = 0;
};

class StorageAPIs {
public:
    static Error RegisterStorageApi(const std::string& type, std::shared_ptr<StorageApi> api) {
        Error err = ErrorOK;
        std::lock_guard<std::mutex> guard(mutex_);
        storage_apis_[type] = api;
        return err;
    }

    static std::optional<std::shared_ptr<StorageApi>> GetStorageApi(const std::string& type) {
        std::lock_guard<std::mutex> guard(mutex_);
        if (storage_apis_.count(type)) {
            return storage_apis_[type];
        }
        CPPIOLOG::warn("Unsupported Storage or Stipe Type {}", type);
        return nullptr;
    }

    static std::optional<std::shared_ptr<StorageApi>> GetStorageApi(VolumeContext& vol_ctx) {
        if (vol_ctx.volume()->type() == VOLUMETYPE_LEAFVOLUME) {
            return StorageAPIs::GetStorageApi(StorageApiType_Name(vol_ctx.volume()->storage_api_type()));
        } else {
            return StorageAPIs::GetStorageApi(VolumeStripeType_Name(vol_ctx.volume()->stripe_type()));
        }
    }

    static std::optional<std::shared_ptr<StorageApi>> GetStorageApi(
        std::shared_ptr<VolumeContext> vol_ctx) {
        if (vol_ctx->volume()->type() == VOLUMETYPE_LEAFVOLUME) {
            return StorageAPIs::GetStorageApi(StorageApiType_Name(vol_ctx->volume()->storage_api_type()));
        } else {
            return StorageAPIs::GetStorageApi(VolumeStripeType_Name(vol_ctx->volume()->stripe_type()));
        }
    }

private:
    static std::mutex mutex_;
    /* One Per FS/Block/OBS/KVS/Cep/Hadoop type, e.g. fs.btrfs, fs.ext4 */
    static std::unordered_map<std::string, std::shared_ptr<StorageApi>> storage_apis_; 
};

class StorageComponent : public Component {
public:
    StorageComponent(const std::string& name) : Component(name) {}
    ~StorageComponent() {}

    bool Startable() { return true; }
    Error Start();
    Error Shutdown();
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_STORAGE_INTERFACE_H_