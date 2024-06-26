#ifndef CPPIO_LOG_HPP
#define CPPIO_LOG_HPP

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

extern std::shared_ptr<spdlog::logger> gLogger;

#endif // CPPIO_LOG_HPP