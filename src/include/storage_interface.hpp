#ifndef CPPIO_STORAGE_INTERFACE_HPP
#define CPPIO_STORAGE_INTERFACE_HPP

#include <string>
#include <vector>
#include <chrono> // For time related operations
#include <iostream>
#include <functional>
#include <mutex>

#include "endpoint.hpp"
#include "goincpp/io/writer.hpp"

namespace cppio {

// Forward declarations for dependent types
class nsLockMap;
namespace dsync {
    struct NetLocker;
}

// Define the healingTracker struct
struct healingTracker {};

// Define the DiskInfo and DiskInfoOptions structs
struct DiskInfo {};
struct DiskInfoOptions {};

// Define the FileInfo and FileInfoVersions structs
struct FileInfo {};
struct FileInfoVersions {};

// Define the RawFileInfo struct
struct RawFileInfo {};

// Define the BitrotVerifier struct
struct BitrotVerifier {};

// Define the StatInfo struct
struct StatInfo {};

// Define the ReadOptions struct
struct ReadOptions {};

// Define the DeleteOptions struct
struct DeleteOptions {};

// Define the UpdateMetadataOpts struct
struct UpdateMetadataOpts {};

// Define the RenameOptions struct
struct RenameOptions {};

// Define the WalkDirOptions struct
struct WalkDirOptions {};

// Define the ReadMultipleReq and ReadMultipleResp structs
struct ReadMultipleReq {};
struct ReadMultipleResp {};

// StorageAPI interface.
class StorageAPI {
public:
    virtual ~StorageAPI() = default;
    // Stringified version of disk.
    virtual std::string string() const = 0;

    // Storage operations.

	// Returns true if disk is online and its valid i.e valid format.json.
	// This has nothing to do with if the drive is hung or not responding.
	// For that individual storage API calls will fail properly. The purpose
	// of this function is to know if the "drive" has "format.json" or not
	// if it has a "format.json" then is it correct "format.json" or not.
    virtual bool isOnline() const = 0;
    // Returns the last time this disk (re)-connected
    virtual std::chrono::time_point<std::chrono::system_clock> lastConn() const = 0;
    // Indicates if disk is local or not.
    virtual bool isLocal() const = 0;
    // Returns hostname if disk is remote.
    virtual std::string hostname() const = 0;
    // Returns the entire endpoint.
    virtual Endpoint endpoint() const = 0;
    // Close the disk, mark it purposefully closed, only implemented for remote disks.
    virtual void close() = 0;
    // Returns the unique 'uuid' of this disk.
    virtual std::string getDiskID() = 0;
    // Set a unique 'uuid' for this disk, only used when
	// disk is replaced and formatted.
    virtual void setDiskID(const std::string& id) = 0;
	// Returns healing information for a newly replaced disk,
	// returns 'nil' once healing is complete or if the disk
	// has never been replaced.
    virtual const HealingTracker* healing() const = 0;
    virtual DiskInfo diskInfo(const DiskInfoOptions& opts) = 0;
    virtual void nsScanner(DataUsageCache& cache, std::vector<dataUsageEntry>& updates, madmin::HealScanMode scanMode, std::function<bool()> shouldSleep) = 0;

    // Volume operations
    virtual void makeVol(const std::string& volume) = 0;
    virtual void makeVolBulk(const std::vector<std::string>& volumes) = 0;
    virtual std::vector<VolInfo> listVols() = 0;
    virtual VolInfo statVol(const std::string& volume) = 0;
    virtual void deleteVol(const std::string& volume, bool forcedelete) = 0;

    Error walkDir(WalkDirOptions opts, goincpp::io::Writer wr);

    // Metadata operations
    virtual Error deleteVersion(Context& ctx, const std::string& volume, const std::string& path, const FileInfo& fi, bool forceDelMarker, const DeleteOptions& opts) = 0;
    virtual std::vector<Error> deleteVersions(Context& ctx, const std::string& volume, const std::vector<FileInfoVersions>& versions, const DeleteOptions& opts) = 0;
    virtual Error writeMetadata(Context& ctx, const std::string& origVolume, const std::string& volume, const std::string& path, const FileInfo& fi) = 0;
    virtual Error updateMetadata(Context& ctx, const std::string& volume, const std::string& path, const FileInfo& fi, const UpdateMetadataOpts& opts) = 0;
    virtual FileInfo readVersion(Context& ctx, const std::string& origVolume, const std::string& volume, const std::string& path, const std::string& versionID, const ReadOptions& opts) = 0;
    virtual RawFileInfo readXL(Context& ctx, const std::string& volume, const std::string& path, bool readData) = 0;
    virtual uint64_t renameData(Context& ctx, const std::string& srcVolume, const std::string& srcPath, const FileInfo& fi, const std::string& dstVolume, const std::string& dstPath, const RenameOptions& opts) = 0;

    // File operations
    virtual std::vector<std::string> listDir(const std::string& volume, const std::string& dirpath, int count) = 0;
    virtual int64_t readFile(const std::string& volume, const std::string& path, int64_t offset, std::vector<uint8_t>& buf, const BitrotVerifier* verifier) = 0;
    virtual void appendFile(const std::string& volume, const std::string& path, const std::vector<uint8_t>& buf) = 0;
    virtual void createFile(const std::string& volume, const std::string& path, int64_t size, std::istream& reader) = 0;
    virtual std::istream& readfileStream(const std::string& volume, const std::string& path, int64_t offset, int64_t length) = 0;
    virtual void renameFile(const std::string& srcvolume, const std::string& srcpath, const std::string& dstvolume, const std::string& dstpath) = 0;
    virtual void checkParts(const std::string& volume, const std::string& path, const FileInfo& fi) = 0;
    virtual void delete(const std::string& volume, const std::string& path, const DeleteOptions& opts) = 0;
    virtual void verifyFile(const std::string& volume, const std::string& path, const FileInfo& fi) = 0;
    virtual std::vector<StatInfo> statInfoFile(const std::string& volume, const std::string& path, bool glob) = 0;
    virtual void readMultiple(const ReadMultipleReq& req, std::vector<ReadMultipleResp>& resp) = 0;
    virtual void cleanAbandonedData(const std::string& volume, const std::string& path) = 0;

    // Write all data, syncs the data to disk.
	// Should be used for smaller payloads.
    virtual void writeAll(const std::string& volume, const std::string& path, const std::vector<uint8_t>& data) = 0;
    // Read all.
    virtual std::vector<uint8_t> readAll(const std::string& volume, const std::string& path) = 0;
    // Retrieve location indexes.
    void getDiskLoc(int& poolIdx, int& setIdx, int& diskIdx) = 0;
    // Set location indexes.
    void setDiskLoc(int poolIdx, int setIdx, int diskIdx) = 0;
    // Set formatData cached value
    void setFormatData(std::vector<uint8_t> b);
};

}

#endif // CPPIO_STORAGE_INTERFACE_HPP