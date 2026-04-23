// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_NVIDIA_GPU_MEMORY_MANAGER_H_
#define CPPIO_NVIDIA_GPU_MEMORY_MANAGER_H_

#include <memory>
#include <unordered_map>
#include <mutex>

#include "../gpu_memory_manager.h"
#include "error.h"

namespace CPPIO_NAMESPACE {

/**
 * NVIDIA-specific GPU memory manager implementation.
 * Uses CUDA APIs for memory allocation, mapping, and synchronization.
 */
class NVIDIAGPUMemoryManager : public GPUMemoryManager {
public:
    NVIDIAGPUMemoryManager();
    virtual ~NVIDIAGPUMemoryManager();

    /**
     * Allocate GPU memory via CUDA and map to host address space.
     */
    virtual Error AllocateMemory(uint64_t size, const std::string& device_id,
                                 GPUMemoryRegion& region) override;

    /**
     * Free GPU memory.
     */
    virtual Error FreeMemory(const GPUMemoryRegion& region) override;

    /**
     * Sync GPU memory to file backing storage.
     */
    virtual Error SyncMemoryToFile(const GPUMemoryRegion& region,
                                   uint64_t offset, uint64_t size) override;

    /**
     * Export GPU memory region as a file descriptor.
     */
    virtual Error ExportAsFile(const GPUMemoryRegion& region,
                               const std::string& export_path, int& fd) override;

    /**
     * Get list of available NVIDIA GPU devices.
     */
    virtual Error GetAvailableDevices(std::vector<std::string>& device_ids) override;

    /**
     * Get NVIDIA GPU device memory statistics.
     */
    virtual Error GetDeviceMemoryStats(const std::string& device_id,
                                       uint64_t& total, uint64_t& free) override;

private:
    std::unordered_map<std::string, GPUMemoryRegion> allocated_regions_;
    std::mutex regions_mutex_;
    bool cuda_initialized_;

    /**
     * Initialize CUDA runtime.
     */
    Error InitializeCUDA();

    /**
     * Cleanup CUDA runtime.
     */
    Error ShutdownCUDA();

    /**
     * Get CUDA device index from device ID string.
     */
    Error GetDeviceIndex(const std::string& device_id, int& device_index);
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_NVIDIA_GPU_MEMORY_MANAGER_H_
