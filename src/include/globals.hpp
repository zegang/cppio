#ifndef CPPIO_GLOBALS_HPP
#define CPPIO_GLOBALS_HPP

#include <string>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <stdexcept>
#include "layout.hpp"
#include "endpoint.hpp"

namespace cppio {

extern std::string globalMinioAddr;
extern EndpointServerPools globalEndpoints;

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

void setGlobalCppioAddr(const std::string& addr);
std::string getLobalCppioAddr();

extern const std::runtime_error errSelfTestFailure; // Declaration of global error variable

}

#endif  // CPPIO_GLOBALS_HPP