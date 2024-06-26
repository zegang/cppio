// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_FS_STORAGE_API_H_
#define CPPIO_FS_STORAGE_API_H_

#include <string>

#include "error.h"
#include "volume.h"
#include "storage_interface.h"

namespace CPPIO_NAMESPACE {

class FSStorageApi : public StorageApi {
public:
    virtual ~FSStorageApi();
//     virtual std::vector<std::string> listDir(const std::string& volume, const std::string& dirpath, int count) = 0;
//     virtual int64_t readFile(const std::string& volume, const std::string& path, int64_t offset, std::vector<uint8_t>& buf, const BitrotVerifier* verifier) = 0;
//     virtual void appendFile(const std::string& volume, const std::string& path, const std::vector<uint8_t>& buf) = 0;
//     virtual void createFile(const std::string& volume, const std::string& path, int64_t size, std::istream& reader) = 0;
//     virtual std::istream& readfileStream(const std::string& volume, const std::string& path, int64_t offset, int64_t length) = 0;
//     virtual void renameFile(const std::string& srcvolume, const std::string& srcpath, const std::string& dstvolume, const std::string& dstpath) = 0;
//     virtual void checkParts(const std::string& volume, const std::string& path, const FileInfo& fi) = 0;
//     virtual void delete(const std::string& volume, const std::string& path, const DeleteOptions& opts) = 0;
//     virtual void verifyFile(const std::string& volume, const std::string& path, const FileInfo& fi) = 0;
//     virtual std::vector<StatInfo> statInfoFile(const std::string& volume, const std::string& path, bool glob) = 0;
//     virtual void readMultiple(const ReadMultipleReq& req, std::vector<ReadMultipleResp>& resp) = 0;
//     virtual void cleanAbandonedData(const std::string& volume, const std::string& path) = 0;

    virtual Error Create(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Read(VolumeContext& vol_ctx, IODescript& io);
    virtual Error Write(VolumeContext& vol_ctx, IODescript& io);

    virtual Error CreateDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error ReadDir(VolumeContext& vol_ctx, IODescript& io);
    virtual Error WriteDir(VolumeContext& vol_ctx, IODescript& io);
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_FS_STORAGE_API_H_