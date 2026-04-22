// Copyright (C) 2026 cppio authors. All rights reserved.

#ifndef CPPIO_LOG_H_
#define CPPIO_LOG_H_

#include <memory>
#include <string_view>
#include <vector>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <grpcpp/grpcpp.h>
#include "spdlog/fmt/bundled/core.h"

// Specialization for grpc::StatusCode
template <>
struct fmt::formatter<grpc::StatusCode> : formatter<int> {
    auto format(grpc::StatusCode code, format_context& ctx) const {
        return formatter<int>::format(static_cast<int>(code), ctx);
    }
};

namespace CPPIO_NAMESPACE {

enum class LogLevel {
    Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Critical = 5, Off = 6
};

class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void Log(LogLevel level, std::string_view module, std::string_view msg,
                     const char* file = nullptr, int line = 0, const char* func = nullptr) = 0;
    
    virtual void SetLevel(LogLevel level) = 0;
    virtual void EnableSourceInfo(bool enable) = 0;
    virtual void Shutdown() = 0;
};

class SpdlogLogger : public ILogger {
private:
    std::shared_ptr<spdlog::logger> logger_;
    bool include_source_ = false;

    void UpdatePattern() {
        // %v is the message, %n is the logger name, but since we use one logger 
        // with a custom module string, we manually format the module into the pattern or message.
        // Here, we use [%P] as a placeholder for the module name passed in the Log call.
        if (include_source_) {
            logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v [%s:%#] [%!]");
        } else {
            logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        }
    }

public:
    SpdlogLogger() {
        try {
            auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/cppio.log", true);
            sinks_ = { console_sink, file_sink };
            logger_ = std::make_shared<spdlog::logger>("cppio", sinks_.begin(), sinks_.end());
        } catch (...) {
            logger_ = spdlog::stderr_color_mt("fallback");
        }
        logger_->set_level(spdlog::level::trace);
        UpdatePattern();
    }

    void Log(LogLevel level, std::string_view module, std::string_view msg, 
             const char* file, int line, const char* func) override {
        spdlog::source_loc loc{file, line, func};
        // Prepend module name to the message so it fits the pattern
        std::string formatted_msg = fmt::format("[{}] {}", module, msg);
        logger_->log(loc, static_cast<spdlog::level::level_enum>(level), formatted_msg);
    }

    void SetLevel(LogLevel level) override {
        logger_->set_level(static_cast<spdlog::level::level_enum>(level));
    }

    void EnableSourceInfo(bool enable) override {
        include_source_ = enable;
        UpdatePattern();
    }

    void Shutdown() override { spdlog::shutdown(); }

private:
    std::vector<spdlog::sink_ptr> sinks_;
};

class Logger {
public:
    static ILogger& GetInstance() {
        static SpdlogLogger instance;
        return instance;
    }
};

/**
 * @brief Helper to handle variadic formatting before passing to the virtual interface.
 */
template<typename... Args>
inline void LogFormatted(LogLevel level, std::string_view module, const char* file, int line, const char* func, 
                         fmt::format_string<Args...> fmt, Args&&... args) {
    auto msg = fmt::format(fmt, std::forward<Args>(args)...);
    Logger::GetInstance().Log(level, module, msg, file, line, func);
}

// --- Macros ---

// The internal macro now uses the LogFormatted template
#define CPPIO_LOG(level, module, ...) \
    CPPIO_NAMESPACE::LogFormatted(level, module, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// Module specific
#define LOG_TRACE_MOD(mod, ...) CPPIO_LOG(CPPIO_NAMESPACE::LogLevel::Trace, mod, __VA_ARGS__)
#define LOG_DEBUG_MOD(mod, ...) CPPIO_LOG(CPPIO_NAMESPACE::LogLevel::Debug, mod, __VA_ARGS__)
#define LOG_INFO_MOD(mod, ...)  CPPIO_LOG(CPPIO_NAMESPACE::LogLevel::Info,  mod, __VA_ARGS__)
#define LOG_WARN_MOD(mod, ...)  CPPIO_LOG(CPPIO_NAMESPACE::LogLevel::Warn,  mod, __VA_ARGS__)
#define LOG_ERROR_MOD(mod, ...) CPPIO_LOG(CPPIO_NAMESPACE::LogLevel::Error, mod, __VA_ARGS__)

// Default "CPPIO" module
#define LOG_TRACE(...) LOG_TRACE_MOD("CPPIO", __VA_ARGS__)
#define LOG_DEBUG(...) LOG_DEBUG_MOD("CPPIO", __VA_ARGS__)
#define LOG_INFO(...)  LOG_INFO_MOD("CPPIO", __VA_ARGS__)
#define LOG_WARN(...)  LOG_WARN_MOD("CPPIO", __VA_ARGS__)
#define LOG_ERROR(...) LOG_ERROR_MOD("CPPIO", __VA_ARGS__)

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_LOG_H_
