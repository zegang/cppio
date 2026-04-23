// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_GPU_STORAGE_FACTORY_H_
#define CPPIO_GPU_STORAGE_FACTORY_H_

#include <string>
#include <memory>

#include "gpu_storage_api.h"
#include "error.h"

namespace CPPIO_NAMESPACE {

/**
 * Factory for creating GPU storage API instances based on vendor.
 * Provides a unified interface for GPU storage initialization.
 */
class GPUStorageFactory {
public:
    /**
     * Create a GPU storage API instance for the specified vendor.
     * 
     * @param vendor - GPU vendor identifier ("nvidia" or "amd")
     * @param device_id - Device identifier (e.g., "nvidia:0", "amd:1")
     * @return Unique pointer to GPUStorageApi, or nullptr on failure
     */
    static std::unique_ptr<GPUStorageApi> CreateGPUStorage(
        const std::string& vendor,
        const std::string& device_id);

    /**
     * Detect available GPU devices of all vendors.
     * 
     * @param devices - Output vector of available device identifiers
     * @return Error status
     */
    static Error DetectAvailableDevices(std::vector<std::string>& devices);

    /**
     * Detect GPU devices for a specific vendor.
     * 
     * @param vendor - GPU vendor ("nvidia" or "amd")
     * @param devices - Output vector of available device identifiers
     * @return Error status
     */
    static Error DetectDevicesForVendor(const std::string& vendor,
                                        std::vector<std::string>& devices);
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_GPU_STORAGE_FACTORY_H_
