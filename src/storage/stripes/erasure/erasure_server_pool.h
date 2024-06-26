// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_ERASURE_SERVER_POOL_H_
#define CPPIO_ERASURE_SERVER_POOL_H_

#include <array>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>

#include "erasure_sets.h"

namespace CPPIO_NAMESPACE {

struct ErasureServerPools {
    std::mutex poolMetaMutex;
    PoolMeta poolMetaInstance;

    std::mutex rebalMu;
    std::unique_ptr<rebalanceMeta> rebalMeta; // Using unique_ptr for single ownership semantics

    std::array<uint8_t, 16> deploymentID;
    std::string distributionAlgo;

    std::vector<std::unique_ptr<ErasureSets>> serverPools; // Using unique_ptr for dynamic memory management

    std::vector<std::function<void()>> decommissionCancelers;

    S3PeerSys* s3Peer; // Using raw pointer assuming ownership is managed elsewhere
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_ERASURE_SERVER_POOL_H_