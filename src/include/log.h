// Copyright (C) 2025 cppio authors. All rights reserved.

#ifndef CPPIO_LOG_H_
#define CPPIO_LOG_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace CPPIO_NAMESPACE {

class Logger {
public:
    static std::shared_ptr<spdlog::logger>& getInstance() {
        static std::shared_ptr<spdlog::logger> logger_instance;
        if (!logger_instance) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/cppio.log", true);
            file_sink->set_level(spdlog::level::trace);

            std::vector<spdlog::sink_ptr> sinks { console_sink, file_sink };

            logger_instance = std::make_shared<spdlog::logger>("cppio_logger", sinks.begin(), sinks.end());
            logger_instance->set_level(spdlog::level::trace);

            spdlog::register_logger(logger_instance);
            spdlog::set_default_logger(logger_instance);
        }
        return logger_instance;
    }

private:
    Logger() {} // Private constructor to prevent external instantiation
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#define CPPIOLOG spdlog

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_LOG_H_