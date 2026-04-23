// Copyright 2026 cppio authors. All rights reserved.

#include "gpu_storage_factory.h"
#include "gpu_factory.h"
#include "nvidia/nvidia_gpu_storage_api.h"
#include "amd/amd_gpu_storage_api.h"
#include "nvidia/nvidia_gpu_memory_manager.h"
#include "amd/amd_gpu_memory_manager.h"
#include "log.h"

namespace CPPIO_NAMESPACE {

std::unique_ptr<GPUStorageApi> GPUStorageFactory::CreateGPUStorage(
    const std::string& vendor,
    const std::string& device_id) {
    
    LOG_INFO("Creating GPU storage for vendor: {}, device: {}", vendor, device_id);

    std::unique_ptr<GPUStorageApi> api = nullptr;

    if (vendor == "nvidia") {
        api = std::make_unique<NVIDIAGPUStorageApi>();
    } else if (vendor == "amd") {
        api = std::make_unique<AMDGPUStorageApi>();
    } else {
        LOG_ERROR("Unsupported GPU vendor: {}", vendor);
        return nullptr;
    }

    Error err = api->Initialize(device_id);
    if (err != ErrorOK) {
        LOG_ERROR("Failed to initialize GPU storage: {}", err->ToString());
        return nullptr;
    }

    LOG_INFO("GPU storage created and initialized successfully");
    return api;
}

Error GPUStorageFactory::DetectAvailableDevices(std::vector<std::string>& devices) {
    Error err = ErrorOK;

    LOG_INFO("Detecting available GPU devices");

    std::vector<std::string> nvidia_devices, amd_devices;

    err = DetectDevicesForVendor("nvidia", nvidia_devices);
    if (err == ErrorOK) {
        devices.insert(devices.end(), nvidia_devices.begin(), nvidia_devices.end());
    }

    err = DetectDevicesForVendor("amd", amd_devices);
    if (err == ErrorOK) {
        devices.insert(devices.end(), amd_devices.begin(), amd_devices.end());
    }

    LOG_INFO("Found {} GPU devices total", devices.size());
    return err;
}

Error GPUStorageFactory::DetectDevicesForVendor(const std::string& vendor,
                                                std::vector<std::string>& devices) {
    Error err = ErrorOK;

    LOG_INFO("Detecting {} GPU devices", vendor);

    std::unique_ptr<GPUMemoryManager> manager = std::move(GPUFactory::CreateGPUMemoryManager(vendor));
    if (!manager) {
        LOG_WARN("Failed to create GPU memory manager for vendor: {}", vendor);
        return MAKE_ERROR(std::errc::device_or_resource_busy, 
                         "Failed to create GPU memory manager");
    }

    err = manager->GetAvailableDevices(devices);
    if (err != ErrorOK) {
        LOG_WARN("Failed to detect {} devices: {}", vendor, err->ToString());
    } else {
        LOG_INFO("Found {} {} GPU devices", devices.size(), vendor);
    }

    return err;
}

} // namespace CPPIO_NAMESPACE
