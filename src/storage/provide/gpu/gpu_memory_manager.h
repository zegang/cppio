// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_GPU_MEMORY_MANAGER_H_
#define CPPIO_GPU_MEMORY_MANAGER_H_

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>

#include "error.h"

namespace CPPIO_NAMESPACE {

/**
 * GPU memory allocation metadata.
 */
struct GPUMemoryRegion {
    uint64_t device_address;    // GPU device address
    uint64_t host_address;      // Host-mapped address
    uint64_t size;              // Size in bytes
    std::string device_id;      // GPU device identifier
    bool is_coherent;           // Cache coherency support
};

/**
 * Unified GPU memory manager for handling memory allocation and mapping.
 * Abstracts vendor-specific implementation details.
 */
class GPUMemoryManager {
public:
    GPUMemoryManager() = default;
    virtual ~GPUMemoryManager() = default;

    /**
     * Allocate GPU memory and map to host address space.
     * Returns a GPUMemoryRegion describing the allocation.
     */
    virtual Error AllocateMemory(uint64_t size, const std::string& device_id,
                                 GPUMemoryRegion& region) = 0;

    /**
     * Free previously allocated GPU memory.
     */
    virtual Error FreeMemory(const GPUMemoryRegion& region) = 0;

    /**
     * Synchronize GPU memory to file backing storage.
     * Ensures cache coherency and persistence.
     */
    virtual Error SyncMemoryToFile(const GPUMemoryRegion& region,
                                   uint64_t offset, uint64_t size) = 0;

    /**
     * Export GPU memory region as a file descriptor.
     * Creates a memory-mapped file interface for direct access.
     */
    virtual Error ExportAsFile(const GPUMemoryRegion& region,
                               const std::string& export_path, int& fd) = 0;

    /**
     * Get list of available GPU devices.
     */
    virtual Error GetAvailableDevices(std::vector<std::string>& device_ids) = 0;

    /**
     * Get memory statistics for a device.
     */
    virtual Error GetDeviceMemoryStats(const std::string& device_id,
                                       uint64_t& total, uint64_t& free) = 0;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_GPU_MEMORY_MANAGER_H_
