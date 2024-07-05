#include "include/globals.hpp"

namespace cppio {

const std::string GlobalMinioDefaultPort = "9000";
const std::string globalMinioDefaultRegion = "";
// This is a sha256 output of ``arn:aws:iam::minio:user/admin``,
// this is kept in present form to be compatible with S3 owner ID
// requirements -
//
// ```
//    The canonical user ID is the Amazon S3–only concept.
//    It is 64-character obfuscated version of the account ID.
// ```
// http://docs.aws.amazon.com/AmazonS3/latest/dev/example-walkthroughs-managing-access-example4.html
const std::string globalMinioDefaultOwnerID = "02d6176db174dc93cb1b899f7c6078f08654445fe8cf1b6ce98d8855f66bdbf4";
const std::string globalMinioDefaultStorageClass = "STANDARD";
const std::string globalWindowsOSName = "windows";
const std::string globalMacOSName = "darwin";
const std::string globalMinioModeFS = "mode-server-fs";
const std::string globalMinioModeErasureSD = "mode-server-xl-single";
const std::string globalMinioModeErasure = "mode-server-xl";
const std::string globalMinioModeDistErasure = "mode-server-distributed-xl";
const std::string globalDirSuffix = "__XLDIR__";
const std::string globalDirSuffixWithSlash = globalDirSuffix + "/";


// Limit fields size (except file) to 1Mib since Policy document
// can reach that size according to https://aws.amazon.com/articles/1434
const int64_t maxFormFieldSize = 1 * 1024 * 1024;  // 1 MiB in bytes
// The maximum allowed time difference between the incoming request
// date and server date during signature verification.
const std::chrono::minutes globalMaxSkewTime(15);  // 15 minutes skew allowed
// GlobalStaleUploadsExpiry - Expiry duration after which the uploads in multipart,
// tmp directory are deemed stale.
const std::chrono::hours GlobalStaleUploadsExpiry(24);  // 24 hours
const std::chrono::hours GlobalStaleUploadsCleanupInterval(6);  // 6 hours
const std::chrono::minutes globalRefreshIAMInterval(10);  // 10 minutes
const int64_t maxLocationConstraintSize = 3 * 1024 * 1024;  // 3 MiB in bytes
const int64_t maxBucketSSEConfigSize = 1 * 1024 * 1024;  // 1 MiB in bytes
const double diskFillFraction = 0.99;
const double diskReserveFraction = 0.15;
const int64_t diskAssumeUnknownSize = 1 << 30;  // 1 GB in bytes
const int64_t diskMinInodes = 1000;
const int tlsClientSessionCacheSize = 100;

ServerCtxt globalServerCtxt;
std::atomic<bool> globalIsDistErasure(false);
std::atomic<bool> globalIsErasure(false);
std::atomic<bool> globalIsErasureSD(false);
std::atomic<bool> globalIsTesting(false);
std::atomic<bool> globalBrowserEnabled(true);

std::mutex globalCppioAddr_mtx; // Mutex for synchronization
std::string globalCppioAddr = ""; // Global string variable

const std::runtime_error errSelfTestFailure("self test failed. unsafe to start server"); // Definition of global error variable

void setGlobalCppioAddr(const std::string& addr) {
    std::lock_guard<std::mutex> lock(globalCppioAddr_mtx);
    globalCppioAddr = addr;
}

std::string getLobalCppioAddr() {
    std::lock_guard<std::mutex> lock(globalCppioAddr_mtx);
    return globalCppioAddr;
}

}