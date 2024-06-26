// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_SERVER_CONTEXT_H_
#define CPPIO_SERVER_CONTEXT_H_

#include <chrono>

#include "volume.h"
// #include "../cli/cli.h"

namespace CPPIO_NAMESPACE {

/// @brief Context of CPPIO Storage Server
class GlobalServerContext {
public:
    GlobalServerContext() : json_(false), quiet_(false), anonymous_(false),
                    strict_s3_compat_(false), config_dir_set_(false),
                    certs_dir_set_(false), max_idle_conns_per_host_(0) {}

    ~GlobalServerContext() = default;

    static std::shared_ptr<GlobalServerContext>& getInstance() {
        static std::shared_ptr<GlobalServerContext> instance;
        if (!instance) {
            instance = std::make_shared<GlobalServerContext>();
        }
        return instance;
    }

    bool json() const { return json_; }
    bool quiet() const { return quiet_; }
    bool anonymous() const { return anonymous_; }
    bool strict_s3_compat() const { return strict_s3_compat_; }
    std::string addr() const { return addr_; }
    std::string console_addr() const { return console_addr_; }
    std::string config_dir() const { return config_dir_; }
    std::string certs_dir() const { return certs_dir_; }
    VolumeContext& volume_context() { return volume_context_; }
    Error set_volume_context(const VolumeContext& volume_context) {
        volume_context_ = volume_context; return ErrorOK;
    }

    void set_json(bool value) { json_ = value; }
    void set_quiet(bool value) { quiet_ = value; }
    void set_anonymous(bool value) { anonymous_ = value; }
    void set_strict_s3_compat(bool value) { strict_s3_compat_ = value; }
    void set_addr(const std::string& value) { addr_ = value; }
    void set_console_addr(const std::string& value) { console_addr_ = value; }
    void set_config_dir(const std::string& value) { config_dir_ = value; }
    void set_certs_dir(const std::string& value) { certs_dir_ = value; }

private:
    bool json_;
    bool quiet_;
    bool anonymous_;
    bool strict_s3_compat_;
    std::string addr_;
    std::string console_addr_;
    std::string config_dir_;
    std::string certs_dir_;
    bool config_dir_set_;
    bool certs_dir_set_;
    std::string interface_;
    std::string root_user_;
    std::string root_password_;
    std::vector<std::string> ftp_;
    std::vector<std::string> sftp_;
    std::chrono::seconds user_timeout_;
    std::chrono::seconds conn_read_deadline_;
    std::chrono::seconds conn_write_deadline_;
    std::chrono::seconds conn_client_read_deadline_;
    std::chrono::seconds conn_client_write_deadline_;
    std::chrono::seconds shutdown_timeout_;
    std::chrono::seconds idle_timeout_;
    std::chrono::seconds read_header_timeout_;
    int max_idle_conns_per_host_;
    VolumeContext volume_context_;
};

// /// @brief Fill GlobalServerContext by parsing cli::Context
// /// @param ctx IN
// /// @param ctxt OUT Server Context
// /// @return 
// Error BuildServerContext(std::shared_ptr<cli::Context> ctx,
//     std::shared_ptr<GlobalServerContext> ctxt);

// void ServerHandleCmdArgs(GlobalServerContext& ctxt);

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_SERVER_CONTEXT_H_