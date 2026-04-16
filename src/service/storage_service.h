// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_STORAGE_SERVICE_H_
#define CPPIO_STORAGE_SERVICE_H_

#include "error.h"
#include "storage.grpc.pb.h"
#include "server_context.h"
#include "storage_interface.h"
#include "prometheus_metrics.h"

#include <chrono>

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

namespace CPPIO_NAMESPACE {

class StorageServiceImpl final : public StorageService::Service {
public:
    Status Create(ServerContext* context,
                  const IODescript* request,
                  IODescript* response) {
        return Status::OK;
    }

    Status Read(ServerContext* context,
                const IODescript* request,
                IODescript* response) {
        auto start = std::chrono::steady_clock::now();
        *response = *request;
        auto api = StorageAPIs::GetStorageApi(GlobalServerContext::getInstance()->volume_context());
        if (api) {
          api.value()->ReadDir(GlobalServerContext::getInstance()->volume_context(), *response);
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start);
        PrometheusMetrics::Instance().RecordRead(request->len(), elapsed);
        return Status::OK;
    }

    Status Write(ServerContext* context,
                 const IODescript* request,
                 IODescript* response) {
        auto start = std::chrono::steady_clock::now();
        *response = *request;
        auto api = StorageAPIs::GetStorageApi(GlobalServerContext::getInstance()->volume_context());
        if (api) {
          api.value()->Write(GlobalServerContext::getInstance()->volume_context(), *response);
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start);
        PrometheusMetrics::Instance().RecordWrite(request->len(), elapsed);
        return Status::OK;
    }

};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_STORAGE_SERVICE_H_