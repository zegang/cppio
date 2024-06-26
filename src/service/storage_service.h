// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_STORAGE_SERVICE_H_
#define CPPIO_STORAGE_SERVICE_H_

#include "error.h"
#include "storage.grpc.pb.h"
#include "server_context.h"
#include "storage_interface.h"

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
        *response = *request;
        auto api = StorageAPIs::GetStorageApi(GlobalServerContext::getInstance()->volume_context());
        if (api) {
          api.value()->ReadDir(GlobalServerContext::getInstance()->volume_context(), *response);
        }
        return Status::OK;
    }

    Status Write(ServerContext* context,
                 const IODescript* request,
                 IODescript* response) {
        return Status::OK;
    }

};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_STORAGE_SERVICE_H_