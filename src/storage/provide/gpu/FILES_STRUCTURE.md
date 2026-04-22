# GPU Storage API - File Structure and Summary

This document provides a complete overview of all files created for GPU memory export as file storage.

## File Organization

```
src/storage/
├── include/
│   └── gpu_storage_api.h                 # Main GPU storage interface
│
└── provide/
    └── gpu/                               # GPU storage implementation
        ├── CMakeLists.txt                # Build configuration (TODO)
        ├── gpu_storage_api.cc            # Base GPU storage implementation
        ├── gpu_storage_api.h             # Already exists in include/
        ├── gpu_memory_manager.h          # Unified memory manager interface
        ├── gpu_memory_manager.cc         # Memory manager factory
        ├── gpu_storage_factory.h         # GPU storage factory
        ├── gpu_storage_factory.cc        # Factory implementation
        ├── gpu_storage_examples.cc       # Usage examples
        ├── README.md                     # Overview documentation
        ├── IMPLEMENTATION_GUIDE.md       # Detailed implementation guide
        ├── nvidia/                       # NVIDIA-specific implementation
        │   ├── nvidia_gpu_storage_api.h
        │   ├── nvidia_gpu_storage_api.cc
        │   ├── nvidia_gpu_memory_manager.h
        │   └── nvidia_gpu_memory_manager.cc
        └── amd/                          # AMD-specific implementation
            ├── amd_gpu_storage_api.h
            ├── amd_gpu_storage_api.cc
            ├── amd_gpu_memory_manager.h
            └── amd_gpu_memory_manager.cc
```

## File Descriptions

### Header Files (Public Interfaces)

| File | Purpose | Key Classes/Functions |
|------|---------|----------------------|
| `include/gpu_storage_api.h` | Main GPU storage API interface | `GPUStorageApi` (extends `StorageApi`) |
| `gpu/gpu_memory_manager.h` | Unified GPU memory management interface | `GPUMemoryManager`, `GPUMemoryRegion` |
| `gpu/gpu_storage_factory.h` | Factory for creating GPU storage instances | `GPUStorageFactory` |
| `gpu/nvidia/nvidia_gpu_storage_api.h` | NVIDIA GPU storage implementation | `NVIDIAGPUStorageApi` |
| `gpu/nvidia/nvidia_gpu_memory_manager.h` | NVIDIA memory manager implementation | `NVIDIAGPUMemoryManager` |
| `gpu/amd/amd_gpu_storage_api.h` | AMD GPU storage implementation | `AMDGPUStorageApi` |
| `gpu/amd/amd_gpu_memory_manager.h` | AMD memory manager implementation | `AMDGPUMemoryManager` |

### Implementation Files

| File | Purpose | Dependencies |
|------|---------|--------------|
| `gpu/gpu_storage_api.cc` | Base implementation of GPU storage interface | `gpu_storage_api.h` |
| `gpu/gpu_memory_manager.cc` | Factory for creating vendor-specific memory managers | `gpu_memory_manager.h` |
| `gpu/gpu_storage_factory.cc` | GPU storage factory implementation | `gpu_storage_factory.h`, vendor implementations |
| `gpu/nvidia/nvidia_gpu_storage_api.cc` | NVIDIA storage API implementation | CUDA libraries |
| `gpu/nvidia/nvidia_gpu_memory_manager.cc` | NVIDIA memory management implementation | CUDA libraries |
| `gpu/amd/amd_gpu_storage_api.cc` | AMD storage API implementation | HIP/ROCm libraries |
| `gpu/amd/amd_gpu_memory_manager.cc` | AMD memory management implementation | HIP/ROCm libraries |

### Documentation Files

| File | Purpose | Audience |
|------|---------|----------|
| `gpu/README.md` | Architecture overview and features | All developers |
| `gpu/IMPLEMENTATION_GUIDE.md` | Detailed implementation instructions | Implementers |
| `gpu/gpu_storage_examples.cc` | Usage examples and patterns | All developers |
| `FILES_STRUCTURE.md` | This file - file organization overview | All developers |

## Key Interfaces

### 1. GPUStorageApi (Main Interface)

```cpp
class GPUStorageApi : public StorageApi {
    virtual Error Initialize(const std::string& gpu_device_id);
    virtual Error Shutdown();
    virtual uint64_t GetMemorySize() const;
    virtual void* GetMappedMemory() const;
    virtual Error ExportMemoryAsFile(const std::string& export_path, int& fd);
    virtual Error SyncMemoryToFile(uint64_t offset, uint64_t size);
    // StorageApi methods: Create, Read, Write, CreateDir, ReadDir, WriteDir
};
```

### 2. GPUMemoryManager (Memory Management)

```cpp
class GPUMemoryManager {
    virtual Error AllocateMemory(uint64_t size, const std::string& device_id,
                                 GPUMemoryRegion& region);
    virtual Error FreeMemory(const GPUMemoryRegion& region);
    virtual Error SyncMemoryToFile(const GPUMemoryRegion& region,
                                   uint64_t offset, uint64_t size);
    virtual Error ExportAsFile(const GPUMemoryRegion& region,
                               const std::string& export_path, int& fd);
    virtual Error GetAvailableDevices(std::vector<std::string>& device_ids);
    virtual Error GetDeviceMemoryStats(const std::string& device_id,
                                       uint64_t& total, uint64_t& free);
};
```

### 3. GPUStorageFactory (Factory Pattern)

```cpp
class GPUStorageFactory {
    static std::shared_ptr<GPUStorageApi> CreateGPUStorage(
        const std::string& vendor,
        const std::string& device_id);
    static Error DetectAvailableDevices(std::vector<std::string>& devices);
    static Error DetectDevicesForVendor(const std::string& vendor,
                                        std::vector<std::string>& devices);
};
```

## Implementation Status

### Completed
- ✅ Header files with complete interfaces
- ✅ Base class implementations
- ✅ Factory pattern implementation
- ✅ Vendor-specific skeleton implementations
- ✅ Error handling framework
- ✅ Logging integration
- ✅ Documentation and examples

### TODO Items (For Completion)

#### NVIDIA Implementation
- [ ] `SetupCUDAContext()` - Setup CUDA device context
- [ ] `AllocateMemory()` - Use cudaMalloc/cudaMallocManaged
- [ ] `ExportMemoryAsFile()` - Implement BAR mapping
- [ ] `SyncMemoryToFile()` - Implement cudaDeviceSynchronize
- [ ] `DetectGPUDevices()` - Use cudaGetDeviceCount
- [ ] Link against CUDA libraries in CMake

#### AMD Implementation
- [ ] `SetupHIPContext()` - Setup HIP device context
- [ ] `AllocateMemory()` - Use hipMalloc/hipMallocManaged
- [ ] `ExportMemoryAsFile()` - Implement P2P/XGMI mapping
- [ ] `SyncMemoryToFile()` - Implement hipDeviceSynchronize
- [ ] `DetectGPUDevices()` - Use hipGetDeviceCount
- [ ] Link against HIP libraries in CMake

#### CMake and Build System
- [ ] Create `gpu/CMakeLists.txt` for GPU storage library
- [ ] Update main `storage/CMakeLists.txt` to include GPU storage
- [ ] Add optional CUDA/HIP detection in main CMakeLists.txt
- [ ] Integrate GPU storage API registration in `storage_interface.cc`

#### Testing
- [ ] Unit tests for device detection
- [ ] Unit tests for memory allocation
- [ ] Integration tests with StorageComponent
- [ ] Multi-GPU stress tests
- [ ] Error condition tests

## Design Patterns Used

### 1. **Factory Pattern**
- `GPUStorageFactory` creates vendor-specific implementations
- Decouples client code from concrete implementations
- Enables runtime vendor selection

### 2. **Strategy Pattern**
- `GPUMemoryManager` interface with vendor-specific strategies
- `NVIDIAGPUMemoryManager` and `AMDGPUMemoryManager` implement different strategies
- Easy to add new vendors without modifying existing code

### 3. **Bridge Pattern**
- `GPUStorageApi` bridges application layer to low-level vendor APIs
- Abstracts CUDA/HIP differences
- Provides uniform interface regardless of GPU vendor

### 4. **Template Method Pattern**
- Base `GPUStorageApi` provides default implementations
- Vendor classes override specific methods
- Common logic shared in base class

## Naming Conventions

### Device ID Format
- NVIDIA: `"nvidia:0"`, `"nvidia:1"`, etc.
- AMD: `"amd:0"`, `"amd:1"`, etc.
- Format: `"{vendor}:{index}"`

### Class Naming
- Base classes: `GPUStorageApi`, `GPUMemoryManager`
- NVIDIA classes: `NVIDIAGPUStorageApi`, `NVIDIAGPUMemoryManager`
- AMD classes: `AMDGPUStorageApi`, `AMDGPUMemoryManager`
- Utilities: `GPUStorageFactory`

### File Naming
- Headers: `.h` extension
- Implementations: `.cc` extension
- Vendor-specific: `{vendor}_*.{h,cc}` format

## Dependencies

### Required Dependencies
- C++17 or later (for std::optional, std::shared_ptr)
- cppio core libraries (storage_interface, log, error, volume)
- Boost libraries (already used in storage)

### Optional GPU Dependencies
- NVIDIA: CUDA Toolkit (cuda_runtime.h, cuda.h)
- AMD: ROCm development packages (hip/hip_runtime.h)

### External Libraries (Build Time)
- spdlog (logging)
- RocksDB (existing storage dependency)
- AWS SDK (existing storage dependency)

## Integration Points

### 1. Storage Component Registration
Location: `src/storage/storage_interface.cc`

```cpp
Error StorageComponent::Start() {
    // Add GPU storage registration:
    StorageAPIs::RegisterStorageApi("gpu.nvidia",
        std::make_shared<NVIDIAGPUStorageApi>());
    StorageAPIs::RegisterStorageApi("gpu.amd",
        std::make_shared<AMDGPUStorageApi>());
}
```

### 2. Volume Configuration
GPU storage can be used in volume definitions:
- Storage API Type: `STORAGEAPITYPE_GPU` (need to add to proto)
- Device ID: Device identifier string
- Memory Size: Configured allocation size

### 3. Device Detection
Applications can detect available GPUs:
```cpp
std::vector<std::string> devices;
GPUStorageFactory::DetectAvailableDevices(devices);
```

## Performance Characteristics

### Bandwidth
- NVIDIA BAR Memory: ~16 GB/s (PCIe 3.0)
- AMD P2P: Full PCIe bandwidth
- AMD XGMI: 200+ GB/s (CDNA architecture)

### Latency
- GPU-to-host access: Microseconds
- Memory synchronization: Nanoseconds
- Cache coherency operations: Microseconds

### Memory Overhead
- Per-allocation overhead: ~256 bytes (GPU memory region metadata)
- Factory overhead: Negligible (single factory per vendor)

## Security Considerations

1. **Memory Access Control**
   - Validate all offset/size parameters
   - Prevent out-of-bounds memory access
   - Use memory access validation in all Read/Write operations

2. **File Permissions**
   - GPU memory files created with restrictive permissions (0600)
   - Consider SELinux policies
   - Validate export paths

3. **Resource Limits**
   - Track total GPU memory allocations
   - Implement per-device memory quotas
   - Prevent denial-of-service through memory exhaustion

## Future Enhancement Areas

1. **Performance Optimization**
   - Memory pooling and reuse
   - Asynchronous I/O operations
   - Predictive prefetching

2. **Multi-GPU Features**
   - GPU-to-GPU memory transfers
   - Distributed memory management
   - Load balancing

3. **Advanced Capabilities**
   - GPU memory compression
   - Memory encryption
   - Device failover and recovery

## Getting Started

### For Developers Implementing GPU Support

1. Read `README.md` for architecture overview
2. Review `IMPLEMENTATION_GUIDE.md` for detailed implementation steps
3. Study `gpu_storage_examples.cc` for usage patterns
4. Start with NVIDIA or AMD implementation based on available hardware
5. Implement vendor-specific methods in derived classes

### For Application Developers

1. Use `GPUStorageFactory::CreateGPUStorage()` to instantiate GPU storage
2. Call `Initialize()` with device ID
3. Use `ExportMemoryAsFile()` to get file descriptor
4. Perform Read/Write operations as needed
5. Call `Shutdown()` for cleanup

### For System Integrators

1. Add GPU support to `storage_interface.cc`
2. Configure CMake to detect and link GPU libraries
3. Update volume configuration proto if needed
4. Write integration tests
5. Document GPU storage setup in deployment guides
