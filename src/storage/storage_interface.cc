// Copyright 2025 cppio authors. All rights reserved.

#include "storage_interface.h"
#include "include/fs_storage_api.h"
#include "include/obs_storage_api.h"
#include "include/striper.h"

namespace CPPIO_NAMESPACE {

std::mutex StorageAPIs::mutex_;
std::unordered_map<std::string, std::shared_ptr<StorageApi>> StorageAPIs::storage_apis_;

Error StorageComponent::Start() {
    Error err = ERROR_OK;

    CPPIOLOG::info("Starting StorageComponent {}", name());

    StorageAPIs::RegisterStorageApi(StorageApiType_Name(STORAGEAPITYPE_FS),
                                    std::make_shared<FSStorageApi>());
    StorageAPIs::RegisterStorageApi(StorageApiType_Name(STORAGEAPITYPE_CLOUD_OBS),
                                    std::make_shared<OBSStorageApi>());
    StorageAPIs::RegisterStorageApi(VolumeStripeType_Name(VOLUMESTRIPETYPE_CACHE),
                                    std::make_shared<VolumeCacheStriper>());

    return err;
}

Error StorageComponent::Shutdown() {
    Error err = ERROR_OK;

    return err;
}

} // namespace CPPIO_NAMESPACE