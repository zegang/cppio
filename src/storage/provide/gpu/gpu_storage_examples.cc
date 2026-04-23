// Copyright 2026 cppio authors. All rights reserved.
// GPU Storage API Usage Examples

#include "gpu_storage_api.h"
#include "gpu_storage_factory.h"
#include "gpu_factory.h"
#include "log.h"

namespace CPPIO_NAMESPACE {

/**
 * Example 1: Simple GPU Memory Export
 * 
 * This example demonstrates exporting GPU memory to a file for direct access.
 */
void ExampleGPUMemoryExport() {
    LOG_INFO("=== Example 1: GPU Memory Export ===");

    // Create NVIDIA GPU storage for device 0
    auto gpu_storage = GPUStorageFactory::CreateGPUStorage("nvidia", "nvidia:0");
    if (!gpu_storage) {
        LOG_ERROR("Failed to create GPU storage");
        return;
    }

    // Export GPU memory as file
    int fd = -1;
    Error err = gpu_storage->ExportMemoryAsFile("/mnt/gpu_memory.img", fd);
    if (err != ErrorOK) {
        LOG_ERROR("Failed to export GPU memory: {}", err->ToString());
        return;
    }

    LOG_INFO("GPU memory exported successfully. FD: {}", fd);
    LOG_INFO("Memory size: {} bytes", gpu_storage->GetMemorySize());

    // Close the file descriptor
    close(fd);
}

/**
 * Example 2: GPU Read/Write Operations
 * 
 * This example shows how to perform read/write operations on GPU memory.
 */
void ExampleGPUReadWrite() {
    LOG_INFO("=== Example 2: GPU Read/Write Operations ===");

    Error err = ERROR_OK;

    // Create AMD GPU storage
    auto gpu_storage = GPUStorageFactory::CreateGPUStorage("amd", "amd:0");

    // Create a volume context (mock)
    VolumeContext vol_ctx;  // In real code, this would be properly initialized

    // Write data to GPU memory
    {
        IODescript io;
        io.set_offset(0);
        io.set_len(4096);
        // In real code: io.set_buffer(data);
        
        err = gpu_storage->Write(vol_ctx, io);
        if (err != ErrorOK) {
            LOG_ERROR("Write failed: {}", err->ToString());
            return;
        }
        LOG_INFO("Write to GPU memory successful");
    }

    // Read data from GPU memory
    {
        IODescript io;
        io.set_offset(0);
        io.set_len(4096);
        
        err = gpu_storage->Read(vol_ctx, io);
        if (err != ErrorOK) {
            LOG_ERROR("Read failed: {}", err->ToString());
            return;
        }
        LOG_INFO("Read from GPU memory successful");
    }

    // Synchronize GPU memory to file storage
    err = gpu_storage->SyncMemoryToFile(0, gpu_storage->GetMemorySize());
    if (err != ErrorOK) {
        LOG_ERROR("Sync failed: {}", err->ToString());
        return;
    }
    LOG_INFO("GPU memory synchronized to file");

    gpu_storage->Shutdown();
}

/**
 * Example 3: Device Detection and Multi-GPU
 * 
 * This example shows how to detect available GPUs and work with multiple devices.
 */
void ExampleMultiGPU() {
    LOG_INFO("=== Example 3: Multi-GPU Detection ===");

    // Detect all available GPU devices
    std::vector<std::string> devices;
    Error err = GPUStorageFactory::DetectAvailableDevices(devices);
    if (err != ErrorOK) {
        LOG_ERROR("Failed to detect GPUs: {}", err->ToString());
        return;
    }

    LOG_INFO("Found {} GPU devices", devices.size());
    for (const auto& device : devices) {
        LOG_INFO("  Device: {}", device);
    }

    // Create storage for each device
    for (const auto& device : devices) {
        // Determine vendor from device ID
        std::string vendor = device.substr(0, device.find(':'));
        
        auto gpu_storage = GPUStorageFactory::CreateGPUStorage(vendor, device);
        if (gpu_storage) {
            LOG_INFO("Created GPU storage for device: {}, Memory: {} MB",
                    device, gpu_storage->GetMemorySize() / (1024 * 1024));
            
            // Use gpu_storage...
            // ...
            
            gpu_storage->Shutdown();
        }
    }
}

/**
 * Example 4: GPU Memory Manager Direct Usage
 * 
 * This example shows direct usage of the GPU memory manager for fine-grained control.
 */
void ExampleGPUMemoryManager() {
    LOG_INFO("=== Example 4: GPU Memory Manager ===");

    // Create NVIDIA memory manager
    auto memory_manager = GPUFactory::CreateGPUMemoryManager("nvidia");
    if (!memory_manager) {
        LOG_ERROR("Failed to create memory manager");
        return;
    }

    // Allocate GPU memory
    GPUMemoryRegion region;
    uint64_t size = 1024 * 1024 * 1024;  // 1GB
    
    Error err = memory_manager->AllocateMemory(size, "nvidia:0", region);
    if (err != ErrorOK) {
        LOG_ERROR("Memory allocation failed: {}", err->ToString());
        return;
    }

    LOG_INFO("Allocated {} bytes at GPU address 0x{:x}, host address 0x{:x}",
            size, region.device_address, region.host_address);

    // Get device memory stats
    {
        uint64_t total = 0, free = 0;
        err = memory_manager->GetDeviceMemoryStats("nvidia:0", total, free);
        if (err == ErrorOK) {
            LOG_INFO("Device memory - Total: {} MB, Free: {} MB",
                    total / (1024 * 1024), free / (1024 * 1024));
        }
    }

    // Export memory as file
    {
        int fd = -1;
        err = memory_manager->ExportAsFile(region, "/mnt/gpu_mem.bin", fd);
        if (err == ErrorOK) {
            LOG_INFO("Memory exported to file with FD: {}", fd);
            close(fd);
        }
    }

    // Sync memory to file
    {
        err = memory_manager->SyncMemoryToFile(region, 0, size);
        if (err == ErrorOK) {
            LOG_INFO("Memory synchronized successfully");
        }
    }

    // Free memory
    err = memory_manager->FreeMemory(region);
    if (err != ErrorOK) {
        LOG_ERROR("Memory free failed: {}", err->ToString());
    } else {
        LOG_INFO("Memory freed successfully");
    }
}

/**
 * Example 5: Error Handling
 * 
 * This example demonstrates proper error handling for GPU operations.
 */
void ExampleErrorHandling() {
    LOG_INFO("=== Example 5: Error Handling ===");

    auto gpu_storage = GPUStorageFactory::CreateGPUStorage("nvidia", "nvidia:0");
    
    // Error: Initialize before use
    VolumeContext vol_ctx;
    IODescript io;
    io.set_offset(0);
    io.set_len(1024);

    Error err = gpu_storage->Read(vol_ctx, io);
    if (err != ErrorOK) {
        LOG_ERROR("Expected error - GPU not initialized: {}", err->ToString());
    }

    // Initialize GPU
    err = gpu_storage->Initialize("nvidia:0");
    if (err != ErrorOK) {
        LOG_ERROR("Initialization error: {}", err->ToString());
        return;
    }

    // Error: Read beyond memory bounds
    uint64_t memory_size = gpu_storage->GetMemorySize();
    io.set_offset(memory_size - 512);  // Start near end
    io.set_len(1024);                  // Read beyond boundary

    err = gpu_storage->Read(vol_ctx, io);
    if (err != ErrorOK) {
        LOG_ERROR("Expected error - Read beyond bounds: {}", err->ToString());
    }

    gpu_storage->Shutdown();
}

} // namespace CPPIO_NAMESPACE

// To compile and run these examples:
// 1. Ensure CUDA/HIP development libraries are installed
// 2. Link against libcuda.so (NVIDIA) or libhip.so (AMD)
// 3. Include proper GPU include directories in compilation flags
// 4. Run with appropriate GPU devices available
