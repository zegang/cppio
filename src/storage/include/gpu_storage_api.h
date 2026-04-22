// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_GPU_STORAGE_API_H_
#define CPPIO_GPU_STORAGE_API_H_

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include "error.h"
#include "volume.h"
#include "storage_interface.h"

namespace CPPIO_NAMESPACE {

/**
 * GPU Storage API - Provides unified interface for GPU memory export as file storage.
 * Supports memory-mapped file access to GPU memory for direct I/O operations.
 */
class GPUStorageApi : public StorageApi {
public:
    virtual ~GPUStorageApi() {}

    /**
     * Initialize GPU storage with configuration parameters.
     * This typically allocates and maps GPU memory to a host-accessible file.
     */
    virtual Error Initialize(const std::string& gpu_device_id) = 0;

    /**
     * Shutdown GPU storage and release resources.
     */
    virtual Error Shutdown() = 0;

    /**
     * Get total allocated GPU memory size in bytes.
     */
    virtual uint64_t GetMemorySize() const = 0;

    /**
     * Get pointer to mapped GPU memory in host address space.
     * Returns nullptr if not initialized or mapping failed.
     */
    virtual void* GetMappedMemory() const = 0;

    /**
     * Export GPU memory to a file descriptor for direct access.
     * The returned file descriptor provides memory-mapped access to GPU memory.
     */
    virtual Error ExportMemoryAsFile(const std::string& export_path, int& fd) = 0;

    /**
     * Sync GPU memory to file storage.
     * Performs necessary cache flushing and synchronization.
     */
    virtual Error SyncMemoryToFile(uint64_t offset, uint64_t size) = 0;

    // StorageApi interface implementations
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io) override;

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io) override;
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io) override;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_GPU_STORAGE_API_H_
