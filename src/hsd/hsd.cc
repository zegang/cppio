// Copyright 2025 cppio authors. All rights reserved.

#include "hsd.h"
#include "service/grpcserver.h"
#include "service/prometheus_metrics.h"

namespace CPPIO_NAMESPACE {

HSD::HSD(const std::string& name) : name_(name) {
    setState(HSDState::HSDSTATE_UNKNOWN);
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_required_.store(false);
    }
}

HSD::~HSD() {
    Shutdown();
}

Error HSD::Start(int argc, char* argv[]) {
    Error err = ERROR_OK;
    setState(HSDState::HSDSTATE_STARTREQ);
    setState(HSDState::HSDSTATE_STARTING);
    CPPIOLOG::info("CppIO HSD {}: main thread starting", name());

    RegisterComponent(std::make_unique<StorageComponent>("storage"));

    for (auto& comp : components_) {
        comp->Start();
    }

    VolumeContext volume_context(argv[2]);
    GlobalServerContext::getInstance()->set_volume_context(volume_context);

    int metrics_port = 9399;
    const char* metrics_port_env = std::getenv("CPPIO_METRICS_PORT");
    if (metrics_port_env) {
        try {
            metrics_port = std::stoi(metrics_port_env);
        } catch (...) {
            metrics_port = 9399;
        }
    }

    metrics_server_ = std::make_unique<MetricsHttpServer>(
        metrics_port,
        []() { return PrometheusMetrics::Instance().Exposition(); });
    metrics_server_->Start();

    auto grpc_server = std::make_shared<GrpcServer>();
    grpc_server->Run();
    setState(HSDState::HSDSTATE_STARTTED);

    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return stop_required_.load(); });
    }

    setState(HSDState::HSDSTATE_SHUTDOWNING);
    for (auto& subsys : components_) {
        subsys->Shutdown();
    }
    if (metrics_server_) {
        metrics_server_->Stop();
    }
    setState(HSDState::HSDSTATE_SHUTDOWNDONE);

    return err;
}

Error HSD::Shutdown() {
    Error err = ERROR_OK;
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_required_.store(true);
    }
    setState(HSDState::HSDSTATE_SHUTDOWNREQ);
    cv_.notify_all();
    return err;
}

Error HSD::Stop() {
    Error err = ERROR_OK;
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_required_.store(true);
    }
    cv_.notify_all();
    return err;
}

Error HSD::RegisterComponent(std::unique_ptr<Component>&& component) {
    Error err = ERROR_OK;
    components_.push_back(std::move(component));
    return err;
}

Error HSD::setState(HSDState state) {
    Error err = ERROR_OK;
    CPPIOLOG::info("CppIO HSD {}: old state {}, new state {}",
                   name(), EnumToString(state_.load()), EnumToString(state));
    state_.store(state);
    return err;
}

} // namespace CPPIO_NAMESPACE