// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_STRIPER_API_H_
#define CPPIO_STRIPER_API_H_

#include <string>
#include <memory>
#include <cstddef>

#include "error.h"
#include "volume.h"
#include "storage_interface.h"

namespace CPPIO_NAMESPACE {

class VolumeRAID1Striper : public StorageApi {
public:
    virtual ~VolumeRAID1Striper();
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io);

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io);
};

class VolumeCacheStriper : public StorageApi {
public:
    virtual ~VolumeCacheStriper();
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io);

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io);
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_STRIPER_API_H_