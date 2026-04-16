// Copyright 2026 cppio authors. All rights reserved.

#ifndef CPPIO_PROMETHEUS_METRICS_H_
#define CPPIO_PROMETHEUS_METRICS_H_

#include <atomic>
#include <chrono>
#include <string>

namespace CPPIO_NAMESPACE {

class PrometheusMetrics {
public:
    static PrometheusMetrics& Instance();

    void RecordRead(std::size_t bytes, std::chrono::microseconds elapsed);
    void RecordWrite(std::size_t bytes, std::chrono::microseconds elapsed);

    std::string Exposition() const;

private:
    PrometheusMetrics();

    std::atomic<uint64_t> read_ops_;
    std::atomic<uint64_t> write_ops_;
    std::atomic<uint64_t> read_bytes_;
    std::atomic<uint64_t> write_bytes_;
    std::atomic<uint64_t> read_time_us_;
    std::atomic<uint64_t> write_time_us_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_PROMETHEUS_METRICS_H_
