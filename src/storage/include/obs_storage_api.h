// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_OBS_STORAGE_API_H_
#define CPPIO_OBS_STORAGE_API_H_

#include <string>
#include <vector>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CopyObjectRequest.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/BucketLocationConstraint.h>
#include <aws/s3/model/CreateBucketConfiguration.h>
#include <aws/core/utils/UUID.h>
#include <aws/core/utils/StringUtils.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/core/auth/AWSCredentials.h>

#include "error.h"
#include "volume.h"
#include "storage_interface.h"

namespace CPPIO_NAMESPACE {

class OBSStorageApi : public StorageApi {
public:
    OBSStorageApi();
    virtual ~OBSStorageApi();
    virtual Error Create(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io);

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io);

    Error CreateBucket(VolumeContext& vol_ctx, IODescript& io);
    Error GetBucket(VolumeContext& vol_ctx, IODescript& io);
    Error ListBucket(VolumeContext& vol_ctx, IODescript& io);
    Error DeleteBucket(VolumeContext& vol_ctx, IODescript& io);

    Error PutObject(VolumeContext& vol_ctx, IODescript& io);
    Error GetObject(VolumeContext& vol_ctx, IODescript& io);
    Error ListObject(VolumeContext& vol_ctx, IODescript& io);
    Error ObjectExists(VolumeContext& vol_ctx, IODescript& io);
    Error DeleteObject(VolumeContext& vol_ctx, IODescript& io);

private:
    using OBSClientPtr = std::shared_ptr<Aws::S3::S3Client>;
    std::unordered_map<std::string, OBSClientPtr> clients_;

private:
    OBSClientPtr getClient(VolumeContext& vol_ctx);
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_OBS_STORAGE_API_H_