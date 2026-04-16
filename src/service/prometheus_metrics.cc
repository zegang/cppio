// Copyright 2026 cppio authors. All rights reserved.

#include "prometheus_metrics.h"

#include <iomanip>
#include <sstream>

namespace CPPIO_NAMESPACE {

PrometheusMetrics& PrometheusMetrics::Instance() {
    static PrometheusMetrics instance;
    return instance;
}

PrometheusMetrics::PrometheusMetrics()
    : read_ops_(0), write_ops_(0), read_bytes_(0), write_bytes_(0),
      read_time_us_(0), write_time_us_(0) {}

void PrometheusMetrics::RecordRead(std::size_t bytes, std::chrono::microseconds elapsed) {
    read_ops_.fetch_add(1, std::memory_order_relaxed);
    read_bytes_.fetch_add(static_cast<uint64_t>(bytes), std::memory_order_relaxed);
    read_time_us_.fetch_add(static_cast<uint64_t>(elapsed.count()), std::memory_order_relaxed);
}

void PrometheusMetrics::RecordWrite(std::size_t bytes, std::chrono::microseconds elapsed) {
    write_ops_.fetch_add(1, std::memory_order_relaxed);
    write_bytes_.fetch_add(static_cast<uint64_t>(bytes), std::memory_order_relaxed);
    write_time_us_.fetch_add(static_cast<uint64_t>(elapsed.count()), std::memory_order_relaxed);
}

std::string PrometheusMetrics::Exposition() const {
    std::ostringstream out;
    out << "# HELP cpio_io_read_operations_total Total number of CppIO read operations.\n";
    out << "# TYPE cpio_io_read_operations_total counter\n";
    out << "cpio_io_read_operations_total " << read_ops_.load(std::memory_order_relaxed) << "\n";

    out << "# HELP cpio_io_write_operations_total Total number of CppIO write operations.\n";
    out << "# TYPE cpio_io_write_operations_total counter\n";
    out << "cpio_io_write_operations_total " << write_ops_.load(std::memory_order_relaxed) << "\n";

    out << "# HELP cpio_io_read_bytes_total Total bytes read by CppIO.\n";
    out << "# TYPE cpio_io_read_bytes_total counter\n";
    out << "cpio_io_read_bytes_total " << read_bytes_.load(std::memory_order_relaxed) << "\n";

    out << "# HELP cpio_io_write_bytes_total Total bytes written by CppIO.\n";
    out << "# TYPE cpio_io_write_bytes_total counter\n";
    out << "cpio_io_write_bytes_total " << write_bytes_.load(std::memory_order_relaxed) << "\n";

    out << "# HELP cpio_io_read_seconds_total Total time spent processing read operations in seconds.\n";
    out << "# TYPE cpio_io_read_seconds_total counter\n";
    out << std::fixed << std::setprecision(6)
        << "cpio_io_read_seconds_total " << (read_time_us_.load(std::memory_order_relaxed) / 1000000.0) << "\n";

    out << "# HELP cpio_io_write_seconds_total Total time spent processing write operations in seconds.\n";
    out << "# TYPE cpio_io_write_seconds_total counter\n";
    out << std::fixed << std::setprecision(6)
        << "cpio_io_write_seconds_total " << (write_time_us_.load(std::memory_order_relaxed) / 1000000.0) << "\n";

    return out.str();
}

} // namespace CPPIO_NAMESPACE
