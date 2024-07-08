#ifndef CPPIO_ERASURE_HPP
#define CPPIO_ERASURE_HPP

#include <vector>
#include <functional>
#include <string>

#include "endpoint.hpp"

namespace cppio {

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


}

#endif // #define CPPIO_ERASURE_HPP
