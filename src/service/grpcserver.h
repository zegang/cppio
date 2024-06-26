// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_SERVER_H_
#define CPPIO_SERVER_H_

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include <thread>

#include "storage_service.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Service;

namespace CPPIO_NAMESPACE {

class GrpcServer {
public:
    GrpcServer() : server_address_([]{
        const char* ip = std::getenv("CPPIO_HSD_SVC_ADDRESS");
        const char* port = std::getenv("CPPIO_HSD_SVC_PORT");
        std::string ip_str = ip ? ip : "0.0.0.0";
        std::string port_str = port ? port : "9999";
        return absl::StrFormat("%s:%s", ip_str, port_str);
    }()) { init(); }
    GrpcServer(const std::string& address, int port) : GrpcServer() {
        server_address_ = absl::StrFormat("%s:%d", address, port);
    }
    GrpcServer(const std::string& server_address) : GrpcServer() {
        server_address_ = server_address;
    }

    ~GrpcServer() {
        if (server_) {
            server_->Shutdown();
        }
        if (thread_) {
            thread_->join();
        }
    }

    void RegisterService(Service* service) {
        builder_.RegisterService(service);
    }

    void Run() {
        builder_.RegisterService(&storage_service_);
        builder_.AddListeningPort(server_address_, grpc::InsecureServerCredentials());
        server_ = builder_.BuildAndStart();
        thread_ = std::make_unique<std::thread>([](Server* server) {
            server->Wait();
        }, server_.get());
    }

private:
    void init() {
        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    }

private:
    std::string server_address_;
    ServerBuilder builder_;
    std::unique_ptr<Server> server_;
    std::unique_ptr<std::thread> thread_;

    StorageServiceImpl storage_service_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_SERVER_H_