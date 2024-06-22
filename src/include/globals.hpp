#ifndef CPPIO_GLOBALS_HPP
#define CPPIO_GLOBALS_HPP

#include <string>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>

namespace cppio {

// Define PoolDisksLayout struct
struct PoolDisksLayout {
    std::string cmdline;
    std::vector<std::vector<std::string>> layout;
};

// Define DisksLayout struct
struct DisksLayout {
    bool legacy;
    std::vector<PoolDisksLayout> pools;
};

// Define serverCtxt struct
struct ServerCtxt {
    bool json;
    bool quiet;
    bool anonymous;
    bool strictS3Compat;
    std::string addr, consoleAddr, configDir, certsDir;
    bool configDirSet, certsDirSet;
    std::string interface;
    std::string rootUser, rootPwd;
    std::vector<std::string> ftp, sftp;
    std::chrono::seconds userTimeout;
    std::chrono::seconds connReadDeadline;
    std::chrono::seconds connWriteDeadline;
    std::chrono::seconds connClientReadDeadline;
    std::chrono::seconds connClientWriteDeadline;
    std::chrono::seconds shutdownTimeout, idleTimeout, readHeaderTimeout;
    int maxIdleConnsPerHost;
    // Nested struct
    DisksLayout* layout;

    // Constructor for initialization
    ServerCtxt() : json(false), quiet(false), anonymous(false), strictS3Compat(false),
                   configDirSet(false), certsDirSet(false), maxIdleConnsPerHost(0) {}
};

extern const std::string GlobalMinioDefaultPor;
extern const std::string globalMinioDefaultRegion;
// This is a sha256 output of ``arn:aws:iam::minio:user/admin``,
// this is kept in present form to be compatible with S3 owner ID
// requirements -
//
// ```
//    The canonical user ID is the Amazon S3–only concept.
//    It is 64-character obfuscated version of the account ID.
// ```
// http://docs.aws.amazon.com/AmazonS3/latest/dev/example-walkthroughs-managing-access-example4.html
extern const std::string globalMinioDefaultOwnerID;
extern const std::string globalMinioDefaultStorageClass;
extern const std::string globalWindowsOSName;
extern const std::string globalMacOSName;
extern const std::string globalMinioModeFS;
extern const std::string globalMinioModeErasureSD;
extern const std::string globalMinioModeErasure;
extern const std::string globalMinioModeDistErasure;
extern const std::string globalDirSuffix;
extern const std::string globalDirSuffixWithSlash;

// Limit fields size (except file) to 1Mib since Policy document
// can reach that size according to https://aws.amazon.com/articles/1434
extern const int64_t maxFormFieldSize;  // 1 MiB in bytes
// The maximum allowed time difference between the incoming request
// date and server date during signature verification.
extern const std::chrono::minutes globalMaxSkewTime;  // 15 minutes skew allowed
// GlobalStaleUploadsExpiry - Expiry duration after which the uploads in multipart,
// tmp directory are deemed stale.
extern const std::chrono::hours GlobalStaleUploadsExpiry;  // 24 hours
extern const std::chrono::hours GlobalStaleUploadsCleanupInterval;  // 6 hours
extern const std::chrono::minutes globalRefreshIAMInterval;  // 10 minutes
extern const int64_t maxLocationConstraintSize;  // 3 MiB in bytes
extern const int64_t maxBucketSSEConfigSize;  // 1 MiB in bytes
extern const double diskFillFraction;
extern const double diskReserveFraction;
extern const int64_t diskAssumeUnknownSize;  // 1 GB in bytes
extern const int64_t diskMinInodes;
extern const int tlsClientSessionCacheSize;


extern ServerCtxt globalServerCtxt;
extern std::atomic<bool> globalIsDistErasure;
extern std::atomic<bool> globalIsErasure;
extern std::atomic<bool> globalIsErasureSD;
extern std::atomic<bool> globalIsTesting;
extern std::atomic<bool> globalBrowserEnabled;

extern std::mutex globalCppioAddr_mtx;   // Mutex for synchronization
extern std::string globalCppioAddr;      // Global string variable

void setGlobalCppioAddr(const std::string& addr) {
    std::lock_guard<std::mutex> lock(globalCppioAddr_mtx);
    globalCppioAddr = addr;
}

std::string getLobalCppioAddr() {
    std::lock_guard<std::mutex> lock(globalCppioAddr_mtx);
    return globalCppioAddr;
}

extern const std::runtime_error errSelfTestFailure; // Declaration of global error variable

}

#endif  // CPPIO_GLOBALS_HPP