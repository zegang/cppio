// Copyright 2026 cppio authors. All rights reserved.

#include "gpu_memory_manager.h"
#include "log.h"

namespace CPPIO_NAMESPACE {

std::shared_ptr<GPUMemoryManager> CreateGPUMemoryManager(const std::string& vendor) {
    LOG_INFO("Creating GPU memory manager for vendor: {}", vendor);
    
    // Forward declarations for vendor-specific implementations
    // These are defined in vendor-specific files (nvidia_gpu_memory_manager.h, amd_gpu_memory_manager.h)
    
    if (vendor == "nvidia") {
        extern std::shared_ptr<GPUMemoryManager> CreateNVIDIAGPUMemoryManager();
        return CreateNVIDIAGPUMemoryManager();
    } else if (vendor == "amd") {
        extern std::shared_ptr<GPUMemoryManager> CreateAMDGPUMemoryManager();
        return CreateAMDGPUMemoryManager();
    } else {
        LOG_ERROR("Unsupported GPU vendor: {}", vendor);
        return nullptr;
    }
}

} // namespace CPPIO_NAMESPACE
