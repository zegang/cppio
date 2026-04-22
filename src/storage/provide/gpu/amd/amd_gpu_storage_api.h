// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_AMD_GPU_STORAGE_API_H_
#define CPPIO_AMD_GPU_STORAGE_API_H_

#include <string>
#include <memory>
#include <cstdint>

#include "gpu_storage_api.h"
#include "error.h"

namespace CPPIO_NAMESPACE {

// Forward declaration
class GPUMemoryManager;

/**
 * AMD GPU Storage API Implementation
 * Handles GPU memory export using AMD-specific interfaces (HIP, ROCm, etc.)
 */
class AMDGPUStorageApi : public GPUStorageApi {
public:
    AMDGPUStorageApi();
    virtual ~AMDGPUStorageApi();

    /**
     * Initialize AMD GPU storage.
     * Sets up GPU memory allocation and host mapping via ROCm/HIP.
     */
    virtual Error Initialize(const std::string& gpu_device_id) override;

    /**
     * Shutdown and cleanup AMD GPU storage.
     */
    virtual Error Shutdown() override;

    /**
     * Get total GPU memory allocated.
     */
    virtual uint64_t GetMemorySize() const override;

    /**
     * Get host-accessible pointer to GPU memory.
     */
    virtual void* GetMappedMemory() const override;

    /**
     * Export GPU memory as file for direct I/O.
     * Uses AMD's P2P and XGMI mechanisms when available.
     */
    virtual Error ExportMemoryAsFile(const std::string& export_path, int& fd) override;

    /**
     * Synchronize GPU memory to file storage.
     */
    virtual Error SyncMemoryToFile(uint64_t offset, uint64_t size) override;

    // StorageApi implementations
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io) override;

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io) override;

private:
    std::shared_ptr<GPUMemoryManager> memory_manager_;
    std::string gpu_device_id_;
    void* mapped_memory_;
    uint64_t memory_size_;
    bool initialized_;

    /**
     * Internal helper: Detect available AMD GPUs.
     */
    Error DetectGPUDevices();

    /**
     * Internal helper: Setup HIP context for device.
     */
    Error SetupHIPContext(const std::string& device_id);
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_AMD_GPU_STORAGE_API_H_
