#ifndef CPPIO_STORAGE_DATATYPES_HPP
#define CPPIO_STORAGE_DATATYPES_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_map>
#include <ctime>    // For std::tm
#include <chrono>   // For std::chrono::system_clock
#include <cstdint>  // For fixed-width integer types
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "last_minute.hpp"
#include "xl_storage_format_v1.hpp"

using namespace boost::property_tree;

namespace cppio {

// DiskMetrics has the information about XL Storage APIs
// the number of calls of each API and the moving average of
// the duration of each API.
struct DiskMetrics {
    std::unordered_map<std::string, AccElem> lastMinute;
    std::unordered_map<std::string, uint64_t> apiCalls;
    uint32_t totalWaiting = 0;
    uint64_t totalErrorsAvailability = 0;
    uint64_t totalErrorsTimeout = 0;
    uint64_t totalWrites = 0;
    uint64_t totalDeletes = 0;

    // Serialization to JSON
    void toJson(std::ostream& out) const {
        ptree pt;
        pt.put("apiLatencies.totalWaiting", totalWaiting);
        pt.put("apiLatencies.totalErrsAvailability", totalErrorsAvailability);
        pt.put("apiLatencies.totalErrsTimeout", totalErrorsTimeout);
        pt.put("apiLatencies.totalWrites", totalWrites);
        pt.put("apiLatencies.totalDeletes", totalDeletes);

        for (const auto& elem : lastMinute) {
            pt.put("apiLatencies." + elem.first, elem.second);
        }

        for (const auto& elem : apiCalls) {
            pt.put("apiCalls." + elem.first, elem.second);
        }

        write_json(out, pt);
    }

    // Deserialization from JSON
    void fromJson(std::istream& in) {
        ptree pt;
        read_json(in, pt);

        totalWaiting = pt.get<uint32_t>("apiLatencies.totalWaiting", 0);
        totalErrorsAvailability = pt.get<uint64_t>("apiLatencies.totalErrsAvailability", 0);
        totalErrorsTimeout = pt.get<uint64_t>("apiLatencies.totalErrsTimeout", 0);
        totalWrites = pt.get<uint64_t>("apiLatencies.totalWrites", 0);
        totalDeletes = pt.get<uint64_t>("apiLatencies.totalDeletes", 0);

        for (const auto& kv : pt.get_child("apiLatencies")) {
            if (kv.first != "totalWaiting" &&
                kv.first != "totalErrsAvailability" &&
                kv.first != "totalErrsTimeout" &&
                kv.first != "totalWrites" &&
                kv.first != "totalDeletes") {
                lastMinute[kv.first] = kv.second.get_value<AccElem>();
            }
        }

        for (const auto& kv : pt.get_child("apiCalls")) {
            apiCalls[kv.first] = kv.second.get_value<uint64_t>();
        }
    }
};

// DiskInfo is an extended type which returns current
// disk usage per path.
// The above means that any added/deleted fields are incompatible.
//
// The above means that any added/deleted fields are incompatible.
//
//msgp:tuple DiskInfo
struct DiskInfo {
    uint64_t total = 0;
    uint64_t free = 0;
    uint64_t used = 0;
    uint64_t usedInodes = 0;
    uint64_t freeInodes = 0;
    uint32_t major = 0;
    uint32_t minor = 0;
    uint64_t nrRequests = 0;
    std::string fsType;
    bool rootDisk = false;
    bool healing = false;
    bool scanning = false;
    std::string endpoint;
    std::string mountPath;
    std::string id;
    bool rotational = false;
    DiskMetrics metrics;
    std::string error; // carries the error over the network
};

// VolInfo - represents volume stat information.
// The above means that any added/deleted fields are incompatible.
//
// The above means that any added/deleted fields are incompatible.
//
//msgp:tuple VolInfo
struct VolInfo {
    std::string name;
    std::chrono::system_clock::time_point created;
};

// VolsInfo is a collection of volume(bucket) information
typedef std::vector<VolInfo> VolsInfo



// RawFileInfo - represents raw file stat information as byte array.
// The above means that any added/deleted fields are incompatible.
// Make sure to bump the internode version at storage-rest-common.go
struct RawFileInfo {
    // Content of entire xl.meta (may contain data depending on what was requested by the caller.
    std::vector<uint8_t> buf;
};

// FileInfo - represents file stat information.
// The above means that any added/deleted fields are incompatible.
// Make sure to bump the internode version at storage-rest-common.go
struct FileInfo {
    std::string volume;                 // Name of the volume
    std::string name;                   // Name of the file
    std::string versionID;              // Version of the file
    bool isLatest;                      // Indicates if the version is the latest
    bool deleted;                       // Deleted marker for versioned bucket
    std::string transitionStatus;       // Status for transitioned entries
    std::string transitionedObjName;    // Object name on remote tier
    std::string transitionTier;         // Storage class label for remote tier
    std::string transitionVersionID;    // Version ID of object on remote tier
    bool expireRestored;                // Indicates expiration of restored object
    std::string dataDir;                // Data directory of the file
    bool xLV1;                          // Indicates if object is in V1 format
    std::chrono::system_clock::time_point modTime;     // Date and time of last modification
    int64_t size;                       // Total file size
    uint32_t mode;                      // File mode bits
    uint64_t writtenByVersion;          // Version of MinIO that created this version
    std::unordered_map<std::string, std::string> metadata; // File metadata
    std::vector<ObjectPartInfo> parts;  // All parts per object
    ErasureInfo erasure;                // Erasure info for all objects
    bool markDeleted;                   // Mark this version as deleted
    std::string replicationState;       // Internal replication state
    std::vector<uint8_t> data;          // Object data (optionally)
    int numVersions;                    // Number of versions
    std::chrono::system_clock::time_point successorModTime; // Time of successor modification
    bool fresh;                         // Indicates first-time call to write FileInfo
    int idx;                            // Position in a multi-object delete call
    std::vector<uint8_t> checksum;      // Combined checksum when object was uploaded
    bool versioned;                     // Indicates if file is versioned
};

}

#endif // CPPIO_STORAGE_DATATYPES_HPP