// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_ERASURE_H_
#define CPPIO_ERASURE_H_

#include <vector>
#include <functional>
#include <string>

#include "endpoint.h"

namespace CPPIO_NAMESPACE {

// erasureObjects - Implements ER object layer.
struct ErasureObjects {
    int setDriveCount;
    int defaultParityCount;
    int setIndex;
    int poolIndex;

    // Function pointers to return lists
    std::function<std::vector<StorageAPI>()> getDisks;
    std::function<std::pair<std::vector<dsync::NetLocker>, std::string>()> getLockers;
    std::function<std::vector<Endpoint>()> getEndpoints;
    std::function<std::vector<std::string>()> getEndpointStrings;

    // Pointer to mutex map
    nsLockMap* nsMutex;
};

} // namespace CPPIO_NAMESPACE

#endif // #define CPPIO_ERASURE_H_
