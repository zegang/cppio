# GPU Storage Implementation Guide

This guide provides detailed information on implementing and extending the GPU Storage API for direct GPU memory export as file storage.

## Overview

The GPU Storage implementation provides a unified interface for exporting GPU memory as file storage. It supports multiple GPU vendors through pluggable implementations and handles memory allocation, mapping, and synchronization.

## Architecture

### Layered Design

```
Application Layer
       ↓
Storage Component (storage_interface.h)
       ↓
GPU Storage Factory (gpu_storage_factory.h/cc)
       ↓
GPU Storage API (gpu_storage_api.h/cc)
       ├─→ NVIDIA Implementation (nvidia_gpu_storage_api.h/cc)
       └─→ AMD Implementation (amd_gpu_storage_api.h/cc)
       ↓
GPU Memory Manager (gpu_memory_manager.h)
       ├─→ NVIDIA Manager (nvidia_gpu_memory_manager.h/cc)
       └─→ AMD Manager (amd_gpu_memory_manager.h/cc)
       ↓
GPU Vendor APIs (CUDA/HIP)
       ↓
GPU Hardware
```

### Component Responsibilities

1. **GPU Storage API** (`gpu_storage_api.h`)
   - High-level interface extending `StorageApi`
   - Device initialization and shutdown
   - Memory export and synchronization
   - Read/Write operations on GPU memory

2. **GPU Storage Factory** (`gpu_storage_factory.h`)
   - Creates vendor-specific GPU storage instances
   - Device detection and enumeration
   - Vendor abstraction

3. **GPU Memory Manager** (`gpu_memory_manager.h`)
   - Manages GPU memory lifecycle
   - Allocation, deallocation, mapping
   - Device statistics and capabilities
   - File export mechanisms

4. **Vendor-Specific Implementations**
   - NVIDIA: CUDA/cuDNN integration
   - AMD: HIP/ROCm integration

## Implementation Details

### NVIDIA GPU Support (CUDA)

#### Required Libraries
```bash
# Development packages
sudo apt-get install cuda-toolkit nvidia-driver-dev

# Headers and libraries typically in:
# /usr/local/cuda/include/cuda.h
# /usr/local/cuda/lib64/libcuda.so
# /usr/include/cuda_runtime.h (if using CUDA Runtime API)
```

#### Key CUDA APIs to Use

1. **Device Management**
   ```cpp
   // Device enumeration
   int device_count = 0;
   cudaError_t err = cudaGetDeviceCount(&device_count);
   
   // Device properties
   cudaDeviceProp prop;
   cudaGetDeviceProperties(&prop, device_id);
   
   // Set active device
   cudaSetDevice(device_id);
   ```

2. **Memory Allocation**
   ```cpp
   // Option 1: Unified Memory (automatic coherency)
   void* device_memory = nullptr;
   cudaMallocManaged(&device_memory, size);
   
   // Option 2: Device memory with mapping
   void* device_ptr = nullptr;
   void* host_ptr = nullptr;
   cudaMalloc(&device_ptr, size);
   
   // Option 3: Pinned memory (fast transfer)
   void* pinned_memory = nullptr;
   cudaMallocHost(&pinned_memory, size);
   ```

3. **Memory Synchronization**
   ```cpp
   // Synchronize GPU
   cudaDeviceSynchronize();
   
   // Prefetch memory
   cudaMemPrefetchAsync(ptr, size, device_id);
   
   // Advise on memory access patterns
   cudaMemAdvise(ptr, size, cudaMemAdviseSetAccessedBy, device_id);
   ```

4. **P2P Access**
   ```cpp
   // Check P2P capabilities
   int can_access = 0;
   cudaDeviceCanAccessPeer(&can_access, device_i, device_j);
   
   // Enable P2P access
   if (can_access) {
       cudaEnablePeerAccess(peer_device, 0);
   }
   ```

#### Implementation Steps

1. In `nvidia_gpu_storage_api.cc::SetupCUDAContext()`:
   ```cpp
   Error NVIDIAGPUStorageApi::SetupCUDAContext(const std::string& device_id) {
       int device_index = std::stoi(device_id.substr(device_id.find(':') + 1));
       
       cudaError_t err = cudaSetDevice(device_index);
       if (err != cudaSuccess) {
           LOG_ERROR("Failed to set CUDA device: {}", cudaGetErrorString(err));
           return MAKE_ERROR(std::errc::device_or_resource_busy, 
                           cudaGetErrorString(err));
       }
       
       err = cudaDeviceSynchronize();
       return (err == cudaSuccess) ? ErrorOK : 
              MAKE_ERROR(std::errc::device_or_resource_busy, 
                        cudaGetErrorString(err));
   }
   ```

2. In `nvidia_gpu_memory_manager.cc::AllocateMemory()`:
   ```cpp
   cudaError_t err = cudaSetDevice(device_index);
   if (err != cudaSuccess) return error;
   
   // Try unified memory first
   err = cudaMallocManaged(&region.host_address, size);
   if (err != cudaSuccess) {
       // Fallback to regular malloc
       err = cudaMalloc(&region.device_address, size);
       if (err != cudaSuccess) return error;
   }
   
   cudaDeviceProp prop;
   cudaGetDeviceProperties(&prop, device_index);
   region.is_coherent = prop.managedMemory;
   ```

### AMD GPU Support (HIP/ROCm)

#### Required Libraries
```bash
# Installation
wget -q -O - https://repo.radeon.com/rocm/rocm.gpg.key | sudo apt-key add -
sudo apt-get install rocm-dev rocm-libs

# Headers and libraries typically in:
# /opt/rocm/include/hip/hip_runtime.h
# /opt/rocm/lib/libamdhip64.so
```

#### Key HIP APIs to Use

1. **Device Management**
   ```cpp
   // Device enumeration
   int device_count = 0;
   hipError_t err = hipGetDeviceCount(&device_count);
   
   // Device properties
   hipDeviceProp_t prop;
   hipGetDeviceProperties(&prop, device_id);
   
   // Set active device
   hipSetDevice(device_id);
   ```

2. **Memory Allocation**
   ```cpp
   // Option 1: Managed Memory (similar to CUDA unified memory)
   void* device_memory = nullptr;
   hipMallocManaged(&device_memory, size);
   
   // Option 2: Device memory
   void* device_ptr = nullptr;
   hipMalloc(&device_ptr, size);
   
   // Option 3: Coherent Memory
   void* host_ptr = nullptr;
   hipHostMalloc(&host_ptr, size, hipHostMallocCoherent);
   ```

3. **Memory Synchronization**
   ```cpp
   // Synchronize GPU
   hipDeviceSynchronize();
   
   // Memory barrier
   hipMemoryBarrier();
   
   // Coherency operations
   hipHccModuleGetFunction(...);
   ```

4. **P2P and XGMI Access**
   ```cpp
   // Check P2P capabilities
   int can_access = 0;
   hipDeviceCanAccessPeer(&can_access, device_i, device_j);
   
   // Enable P2P access
   if (can_access) {
       hipEnablePeerAccess(peer_device, 0);
   }
   
   // Check XGMI connectivity
   hipDeviceGetAttribute(..., hipDeviceAttributeMaxTexture3DWidth, device);
   ```

#### Implementation Steps

1. In `amd_gpu_storage_api.cc::SetupHIPContext()`:
   ```cpp
   Error AMDGPUStorageApi::SetupHIPContext(const std::string& device_id) {
       int device_index = std::stoi(device_id.substr(device_id.find(':') + 1));
       
       hipError_t err = hipSetDevice(device_index);
       if (err != hipSuccess) {
           LOG_ERROR("Failed to set HIP device: {}", hipGetErrorName(err));
           return MAKE_ERROR(std::errc::device_or_resource_busy, 
                           hipGetErrorName(err));
       }
       
       err = hipDeviceSynchronize();
       return (err == hipSuccess) ? ErrorOK : 
              MAKE_ERROR(std::errc::device_or_resource_busy, 
                        hipGetErrorName(err));
   }
   ```

2. In `amd_gpu_memory_manager.cc::AllocateMemory()`:
   ```cpp
   hipError_t err = hipSetDevice(device_index);
   if (err != hipSuccess) return error;
   
   // Try managed memory
   err = hipMallocManaged(&region.host_address, size);
   if (err != hipSuccess) {
       // Fallback to device malloc
       err = hipMalloc(&region.device_address, size);
       if (err != hipSuccess) return error;
   }
   
   hipDeviceProp_t prop;
   hipGetDeviceProperties(&prop, device_index);
   region.is_coherent = (prop.managedMemory != 0);
   ```

## CMake Integration

Create or update `src/storage/provide/gpu/CMakeLists.txt`:

```cmake
# GPU Storage library
file(GLOB GPU_STORAGE_SOURCES
    gpu_storage_api.cc
    gpu_memory_manager.cc
    gpu_storage_factory.cc
    nvidia/*.cc
    amd/*.cc
)

add_library(gpu_storage ${GPU_STORAGE_SOURCES})

target_compile_definitions(gpu_storage PRIVATE 
    CPPIO_NAMESPACE=${CPPIO_NAMESPACE}
    CPPIO_OUT=${CPPIO_OUT}
)

target_include_directories(gpu_storage PRIVATE ${CPPIO_INCLUDE_DIR})
target_include_directories(gpu_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Optional: Link against CUDA libraries
if(CUDA_FOUND)
    target_link_libraries(gpu_storage PRIVATE ${CUDA_LIBRARIES})
    target_include_directories(gpu_storage PRIVATE ${CUDA_INCLUDE_DIRS})
    target_compile_definitions(gpu_storage PRIVATE HAVE_CUDA)
endif()

# Optional: Link against HIP libraries
if(HIP_FOUND)
    target_link_libraries(gpu_storage PRIVATE hip::host)
    target_include_directories(gpu_storage PRIVATE ${HIP_INCLUDE_DIRS})
    target_compile_definitions(gpu_storage PRIVATE HAVE_HIP)
endif()

target_link_libraries(gpu_storage PUBLIC storage_interface)
```

Update `src/storage/CMakeLists.txt`:
```cmake
add_subdirectory(provide/gpu)
```

## File Export Mechanisms

### 1. Memory-Mapped File Export

```cpp
Error ExportMemoryAsFile(const GPUMemoryRegion& region,
                         const std::string& export_path, int& fd) {
    // Create backing file
    fd = open(export_path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd < 0) return error;
    
    // Extend file to GPU memory size
    if (ftruncate(fd, region.size) < 0) {
        close(fd);
        return error;
    }
    
    // Optional: Use mmap for additional control
    void* mmap_addr = mmap(nullptr, region.size,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED, fd, 0);
    
    // Copy GPU memory to file via mmap (or direct pointer if coherent)
    
    return ErrorOK;
}
```

### 2. Direct BAR Mapping (NVIDIA)

```cpp
// Using PCI BAR address for GPU memory
// Requires GPU IOMMU support and proper permissions

// Approach:
// 1. Query GPU BAR address from /sys/class/pci_bus/
// 2. Map BAR memory to host address space
// 3. Provide file interface to mapped memory
```

### 3. P2P Direct Access (AMD XGMI)

```cpp
// For CDNA2 and newer with XGMI
// Direct GPU-to-GPU or GPU-to-CPU fabric access

Error ExportAsFile(const GPUMemoryRegion& region,
                   const std::string& export_path, int& fd) {
    // Enable peer access if available
    for (int i = 0; i < device_count; i++) {
        if (i != device_index) {
            hipEnablePeerAccess(i, 0);
        }
    }
    
    // Create file that directly references GPU memory
    // through P2P or XGMI mechanisms
}
```

## Error Handling

### CUDA Error Checking Pattern

```cpp
#define CUDA_CHECK(call) \
    { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            LOG_ERROR("CUDA error: {}", cudaGetErrorString(err)); \
            return MAKE_ERROR(std::errc::device_or_resource_busy, \
                            cudaGetErrorString(err)); \
        } \
    }
```

### HIP Error Checking Pattern

```cpp
#define HIP_CHECK(call) \
    { \
        hipError_t err = call; \
        if (err != hipSuccess) { \
            LOG_ERROR("HIP error: {}", hipGetErrorName(err)); \
            return MAKE_ERROR(std::errc::device_or_resource_busy, \
                            hipGetErrorName(err)); \
        } \
    }
```

## Memory Coherency Considerations

### Cache Coherency Protocols

1. **NVIDIA**
   - Pascal+: Full GPU-CPU coherency via NVLINK/PCIe
   - Older: Requires explicit synchronization

2. **AMD**
   - CDNA2+: Full system coherency
   - RDNA/CDNA: Partial coherency via XGMI

### Synchronization Points

```cpp
Error SyncMemoryToFile(uint64_t offset, uint64_t size) {
    // 1. Stop GPU operations
    gpu_device_synchronize();
    
    // 2. Flush GPU caches
    gpu_cache_flush();
    
    // 3. Memory barrier
    memory_barrier();
    
    // 4. Invalidate CPU caches if needed
    cpu_cache_invalidate(gpu_memory_ptr + offset, size);
    
    // 5. Persist to file
    fdatasync(file_fd);
    
    return ErrorOK;
}
```

## Performance Optimization

### Bandwidth Optimization

```cpp
// Use large transfers
const size_t OPTIMAL_TRANSFER_SIZE = 256 * 1024 * 1024;  // 256MB

Error OptimizedRead(uint64_t offset, uint64_t size) {
    for (uint64_t i = 0; i < size; i += OPTIMAL_TRANSFER_SIZE) {
        uint64_t chunk_size = std::min(OPTIMAL_TRANSFER_SIZE, size - i);
        // Transfer chunk
    }
}
```

### Latency Optimization

```cpp
// Overlap transfers with computation
Error AsyncRead(uint64_t offset, uint64_t size,
                std::function<void()> callback) {
    // Use CUDA streams / HIP streams for async operations
    // Transfer data asynchronously
    // Call callback when complete
}
```

## Testing Strategy

### Unit Tests

```cpp
// Test device detection
TEST(GPUStorageFactory, DetectDevices) {
    std::vector<std::string> devices;
    ASSERT_EQ(GPUStorageFactory::DetectAvailableDevices(devices), ErrorOK);
    ASSERT_GT(devices.size(), 0);
}

// Test memory allocation
TEST(NVIDIAGPUMemoryManager, AllocateMemory) {
    auto manager = std::make_shared<NVIDIAGPUMemoryManager>();
    GPUMemoryRegion region;
    ASSERT_EQ(manager->AllocateMemory(1024 * 1024, "nvidia:0", region), ErrorOK);
    ASSERT_EQ(region.size, 1024 * 1024);
}
```

### Integration Tests

```cpp
// Test with actual storage operations
TEST(GPUStorageApi, ReadWrite) {
    auto api = std::make_shared<NVIDIAGPUStorageApi>();
    ASSERT_EQ(api->Initialize("nvidia:0"), ErrorOK);
    
    // Write test data
    IODescript write_io;
    // ...perform write...
    ASSERT_EQ(api->Write(vol_ctx, write_io), ErrorOK);
    
    // Read and verify
    IODescript read_io;
    // ...perform read...
    ASSERT_EQ(api->Read(vol_ctx, read_io), ErrorOK);
    
    api->Shutdown();
}
```

## Security Considerations

1. **Memory Protection**
   - Validate all memory access offsets
   - Prevent out-of-bounds access
   - Use address sanitizers in debug builds

2. **File Permissions**
   - Restrict GPU memory file access to authorized users
   - Use appropriate umask for exported files
   - Consider SELinux policies

3. **Resource Limits**
   - Track memory allocations
   - Prevent memory exhaustion attacks
   - Implement quota mechanisms

## Future Enhancements

1. **Multi-GPU Orchestration**
   - Distributed memory management
   - Load balancing across devices
   - Device failover

2. **Advanced Memory Features**
   - GPU-to-GPU memory transfers
   - Compression/decompression
   - Encryption of GPU memory

3. **Performance Features**
   - Memory pooling and caching
   - Predictive prefetching
   - Adaptive data placement

## References

- [CUDA C Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [HIP Documentation](https://rocmdocs.amd.com/)
- [ROCm Memory Management](https://rocmdocs.amd.com/en/docs-4.3.1/conceptual/using-hip/index.html)
