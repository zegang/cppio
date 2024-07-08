#ifndef CPPIO_ERASURE_SETS_HPP
#define CPPIO_ERASURE_SETS_HPP

#include <array>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <functional>

#include "format_erasure.hpp"
#include "endpoint.hpp"

namespace cppio {

// setsDsyncLockers is encapsulated type for Close()
using SetsDsyncLockers = std::vector<std::vector<dsync::NetLocker>>;

// erasureSets implements ObjectLayer combining a static list of erasure coded
// object sets. NOTE: There is no dynamic scaling allowed or intended in
// current design.
struct ErasureSets {
    std::vector<std::unique_ptr<ErasureObjects>> sets; // Using unique_ptr for dynamic memory management
    FormatErasureV3* format; // Using raw pointer assuming ownership is managed elsewhere
    std::mutex erasureDisksMu;
    std::vector<std::vector<StorageAPI*>> erasureDisks; // Using raw pointers assuming ownership is managed elsewhere
    SetsDsyncLockers erasureLockers;
    std::string erasureLockOwner;
    PoolEndpoints endpoints;
    std::vector<std::string> endpointStrings;
    int setCount;
    int setDriveCount;
    int defaultParityCount;
    int poolIndex;
    std::chrono::time_point<std::chrono::system_clock> lastConnectDisksOpTime;
    // Channel-like behavior can be achieved using a combination of condition_variable and a queue
    std::function<void(int)> setReconnectEvent;
    std::string distributionAlgo;
    std::array<uint8_t, 16> deploymentID;
};


}

#endif // CPPIO_ERASURE_SETS_HPP