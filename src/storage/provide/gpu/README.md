# GPU Storage API Implementation

This directory contains the GPU Storage API implementation for exporting GPU memory as file storage for direct access.

## Architecture

The GPU storage implementation follows a layered architecture similar to FS and KVS storage:

### Directory Structure
```
gpu/
├── nvidia/                      # NVIDIA GPU implementation
│   ├── nvidia_gpu_storage_api.h
│   ├── nvidia_gpu_storage_api.cc
│   ├── nvidia_gpu_memory_manager.h
│   └── nvidia_gpu_memory_manager.cc
├── amd/                         # AMD GPU implementation
│   ├── amd_gpu_storage_api.h
│   ├── amd_gpu_storage_api.cc
│   ├── amd_gpu_memory_manager.h
│   └── amd_gpu_memory_manager.cc
├── gpu_memory_manager.h         # Unified memory manager interface
├── gpu_memory_manager.cc        # Factory for vendor-specific managers
└── README.md                    # This file
```

## Components

### 1. GPU Storage API (`gpu_storage_api.h`)
Unified interface for GPU storage operations extending `StorageApi`:
- `Initialize(device_id)` - Setup GPU memory allocation and mapping
- `Shutdown()` - Cleanup and resource release
- `ExportMemoryAsFile(path, fd)` - Export GPU memory as memory-mapped file
- `SyncMemoryToFile(offset, size)` - Synchronize GPU memory to file storage
- `Read/Write/Create/ReadDir/WriteDir` - Standard storage operations

### 2. GPU Memory Manager (`gpu_memory_manager.h`)
Abstraction for vendor-specific memory management:
- `AllocateMemory()` - Allocate and map GPU memory
- `FreeMemory()` - Free GPU memory
- `ExportAsFile()` - Export memory region as file descriptor
- `SyncMemoryToFile()` - Synchronize GPU memory
- `GetAvailableDevices()` - Query available GPU devices
- `GetDeviceMemoryStats()` - Query device memory statistics

### 3. NVIDIA Implementation
Uses CUDA/CUDA Graphs APIs:
- **nvidia_gpu_storage_api** - Main storage interface for NVIDIA GPUs
- **nvidia_gpu_memory_manager** - CUDA-specific memory management
- Supports device enumeration via `cudaGetDeviceCount()`
- Memory mapping via CUDA Unified Memory or BAR (Base Address Register)
- P2P access mechanisms for high-bandwidth GPU-to-host transfers

### 4. AMD Implementation
Uses HIP/ROCm APIs:
- **amd_gpu_storage_api** - Main storage interface for AMD GPUs
- **amd_gpu_memory_manager** - HIP-specific memory management
- Supports device enumeration via `hipGetDeviceCount()`
- Memory mapping via HIP Managed Memory
- P2P and XGMI (Infinity Fabric) support when available

## Usage Example

```cpp
#include "gpu_storage_api.h"

// Create and initialize NVIDIA GPU storage
auto gpu_api = std::make_shared<NVIDIAGPUStorageApi>();
Error err = gpu_api->Initialize("nvidia:0");  // GPU 0

// Export GPU memory as file
int fd = -1;
err = gpu_api->ExportMemoryAsFile("/mnt/gpu_memory.img", fd);

// Perform read/write operations
IODescript io;
io.set_offset(0);
io.set_len(4096);
// Set buffer data...
err = gpu_api->Write(vol_ctx, io);

// Sync memory to file storage
err = gpu_api->SyncMemoryToFile(0, gpu_api->GetMemorySize());

// Cleanup
err = gpu_api->Shutdown();
```

## Memory Access Patterns

### NVIDIA (CUDA)
- **Unified Memory (cuMemAllocManaged)**: Automatic memory coherency between GPU and host
- **BAR Mapping**: Map GPU BAR memory region to host address space
- **P2P Access**: Direct GPU-to-GPU and GPU-to-host transfers via PCIe
- **Cache Coherency**: Support via NVIDIA's coherency protocols

### AMD (HIP/ROCm)
- **Managed Memory (hipMallocManaged)**: Automatic memory coherency via HIP runtime
- **P2P Access**: Direct GPU-to-GPU memory access via PCIe P2P
- **XGMI/Infinity Fabric**: High-bandwidth GPU interconnect for multi-GPU systems
- **Cache Coherency**: Support via ROCm coherency protocols

## Implementation Notes

### Memory Synchronization
- GPU cache must be flushed before host access
- Host-GPU memory barriers required for coherency
- Vendor-specific synchronization primitives used

### File Export Mechanisms
1. **Memory-Mapped Files**: Create backing file and map GPU memory region
2. **Direct File Descriptors**: Return FD for direct kernel-level access
3. **Unified Memory**: Leverage automatic coherency for seamless access

### Device Detection
- NVIDIA: Parse CUDA device enumeration
- AMD: Parse HIP device enumeration
- Device ID format: `{vendor}:{index}` (e.g., "nvidia:0", "amd:1")

## TODO Items

### CUDA Implementation (NVIDIA)
- [ ] Implement `cudaSetDevice()` context setup
- [ ] Implement `cudaMalloc()` or `cuMemAllocManaged()` for memory allocation
- [ ] Implement BAR memory mapping for file export
- [ ] Implement `cudaDeviceSynchronize()` for memory sync
- [ ] Add P2P capability detection

### HIP Implementation (AMD)
- [ ] Implement `hipSetDevice()` context setup
- [ ] Implement `hipMalloc()` or `hipMallocManaged()` for memory allocation
- [ ] Implement P2P/XGMI mapping for file export
- [ ] Implement `hipDeviceSynchronize()` for memory sync
- [ ] Add XGMI topology detection

### Common
- [ ] Error handling and validation
- [ ] Resource cleanup and leak prevention
- [ ] Performance profiling and optimization
- [ ] Integration testing with volume system
- [ ] Documentation updates

## Performance Considerations

### Bandwidth
- BAR memory: ~16 GB/s typical (depends on PCIe generation)
- P2P direct: Can approach full PCIe bandwidth
- XGMI (AMD): Up to 200+ GB/s on CDNA architecture

### Latency
- GPU-to-host: microseconds range
- Memory coherency: nanoseconds
- Cache flushing: microseconds

### Scalability
- Multi-GPU support via device enumeration
- Concurrent memory exports supported
- Shared GPU memory regions possible

## References

- CUDA Programming Guide: https://docs.nvidia.com/cuda/cuda-c-programming-guide/
- HIP Documentation: https://rocmdocs.amd.com/
- ROCm Memory Management: https://rocmdocs.amd.com/en/docs-4.3.1/conceptual/using-hip/index.html
- PCIe Peer-to-Peer Access: https://docs.nvidia.com/cuda/cuda-c-programming-guide/#device-to-device-communication
