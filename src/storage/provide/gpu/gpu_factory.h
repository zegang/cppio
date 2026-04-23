// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_GPU_FACTORY_H_
#define CPPIO_GPU_FACTORY_H_

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "gpu_memory_manager.h"

namespace CPPIO_NAMESPACE {

using GPUMmCreator = std::function<std::unique_ptr<GPUMemoryManager>()>;

class GPUFactory {
public:
    static void RegisterGPUMemoryManager(const std::string& vendor, GPUMmCreator creator) {
        GetGPUMemoryManagerRegistry()[vendor] = creator;
    }

    static std::unique_ptr<GPUMemoryManager> CreateGPUMemoryManager(const std::string& vendor) {
        auto& reg = GetGPUMemoryManagerRegistry();
        if (reg.find(vendor) != reg.end()) {
            return reg[vendor]();
        }
        return nullptr;
    }

private:
    // Using a function-static map avoids the "static initialization order fiasco"
    static std::map<std::string, GPUMmCreator>& GetGPUMemoryManagerRegistry() {
        static std::map<std::string, GPUMmCreator> registry;
        return registry;
    }
};

// Helper class to trigger registration
struct GPURegistrar {
    GPURegistrar(const std::string& vendor, GPUMmCreator creator) {
        GPUFactory::RegisterGPUMemoryManager(vendor, creator);
    }
};

// Macro to make it easy for vendors
#define REGISTER_GPU_VENDOR(vendor, vendor_str, class_name) \
    static GPURegistrar global_registrar_##vendor_##class_name(vendor_str, [] { \
        return std::make_unique<class_name>(); \
    });

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_GPU_FACTORY_H_
